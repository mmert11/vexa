#include <vexa/vexa.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <CLI/CLI.hpp>

using namespace std;


/*
if (false)
{
    vexa::binary binary("/media/mert/Yeni Birim/input.exe");
    engine.map_binary(binary);
    rip = 0x1400016d0;
}
*/

/*

auto callback = [&](vexa::engine& e) -> void
    {
        auto cpu = e.get_cpu();
        auto symex = e.get_symex();
        auto memory = e.get_memory();

        if (cpu->instruction.pc != 0x40228c)
            return;

        auto* rbp = cpu->read_register(vexa::amd64::RBP)->simplify();
        auto* stack_ptr = vexa::to_ptr(symex->get(cpu->stack_ptr)->simplify());

        if (!rbp->is_concrete())
        {
            std::cout << "RBP veya stack pointer symbolic\n";
            return;
        }

        const int64_t rbp_offset = static_cast<int64_t>(rbp->as_uint64());

        const int64_t stack_page_address =
            static_cast<int64_t>(stack_ptr->as_uint64())
            + rbp_offset
            - 0x2c;

        auto* pointer = symex->pointer(
            stack_page_address,
            stack_ptr->get_page());

        auto* value = memory->read(pointer, 32);

        std::cout << value->as_expr() << '\n';
        getchar();

        cpu->VPC = value->as_uint64();
        cpu->VJMP = true;
    };
*/

int main(int argc, char** argv)
{
    std::string input_file, output_file;
    uint64_t address;
    bool no_opt = false;
    bool no_statistics = false;
    bool quiet = false;
    bool no_constant_propagation = false;
    bool no_opaque_solving = false;
    bool cfg_recovery = false;
    bool vcfg_recovery = false;
    bool vcfg_path_sensitive = false;

    CLI::App cli{"vexa-cli: command-line interface for lifting & deobfuscating binaries statically"};

    cli.add_option("-i,--input", input_file,"Path to the input binary")
    ->required();
    cli.add_option("-a,--address", address, "Virtual address of the function that will be lifted (with hex prefix 0x)")
    ->required();
    cli.add_flag("-q,--quiet", quiet, "Silent execution, no logging");
    cli.add_flag("--no-statistics", no_statistics, "Print no statistics, only ir");
    cli.add_flag("--no-opt", no_opt, "Disable optimization");
    cli.add_flag("--no-constant-propagation", no_constant_propagation, "Disable Z3 powered constant propagation");
    cli.add_flag("--no-opaque-solving", no_opaque_solving, "Disables opaque predicate solving with Z3");
    cli.add_flag("--cfg-recovery", cfg_recovery, "Enables CFG recovery mode");
    cli.add_flag("--vcfg-recovery", vcfg_recovery, "Enables Virtual CFG recovery mode");
    cli.add_flag("--cfg-path-sensitive", vcfg_path_sensitive, "Specialize VCFG paths while preserving path-local loops");
    auto recompile_option = cli.add_option("-r,--recompile", output_file, "Output path for recompiling the lifted IR (default: 'recompiled')")
                            ->expected(0, 1);

    if (cfg_recovery && vcfg_recovery)
        return -1;

    CLI11_PARSE(cli, argc, argv);

    if (output_file.empty())
        output_file = "recompiled";

    vexa::init(quiet ? vexa::logging_mode::NONE : vexa::logging_mode::DEBUG);

    vexa::engine engine;
    vexa::binary bin(input_file);
    engine.map_binary(bin);

    if (cfg_recovery)
        engine.set_option(vexa::option::MODE, vexa::mode_t::CFG_RECOVERY);
    else if (vcfg_recovery)
        engine.set_option(vexa::option::MODE, vexa::mode_t::VCFG_RECOVERY);
    else
        engine.set_option(vexa::option::MODE, vexa::mode_t::SYMBOLIC_EXPLORATION);

    if (vcfg_path_sensitive)
        engine.set_option(vexa::option::CFG_JOIN_POLICY, vexa::cfg_join_policy_t::SPECIALIZE_BY_PATH);

    engine.set_option(vexa::option::OPAQUE_SOLVING, !no_opaque_solving);
    engine.set_option(vexa::option::Z3_CONSTANT_PROPAGATION, !no_constant_propagation);
    engine.set_option(vexa::option::LOOP_REROLL, 0);
    engine.set_option(vexa::option::STATE_CLEANUP, 1);

    bool first_dispatch = true;
    uint64_t handler_table;

    auto v_dispatch = [&](vexa::engine& e) -> void
    {
        auto cpu = e.get_cpu();
        vexa::value* r9 = e.get_cpu()->read_register(vexa::amd64::R9);

        if (first_dispatch)
        {
            //std::cout << "Found handler table: " << std::hex << r9->as_uint64() << std::endl;
            handler_table = r9->as_uint64();
            first_dispatch = false;
        }
        else
        {
            if (r9->as_uint64() == handler_table)
            {
                uint64_t VPC = cpu->read_register(vexa::amd64::RAX)->as_uint64();
                cpu->VPC = VPC;
                cpu->VJMP = true;
                std::cout << "Next VPC: " << std::hex << VPC << std::endl;

                if (VPC == 0xe028)
                {
                    e.set_option(vexa::option::OPAQUE_SOLVING, false);
                }
                else {
                    e.set_option(vexa::option::OPAQUE_SOLVING, true);
                }
            }
        }
    };

    auto cond_br = [&](vexa::engine& e) -> void
    {
        auto cpu = e.get_cpu();

        if (cpu->instruction.pc == 0xdd94)
        {
            e.set_option(vexa::option::OPAQUE_SOLVING, false);
        }
        else if (cpu->instruction.pc == 0xddbc)
        {
            e.set_option(vexa::option::OPAQUE_SOLVING, true);
        }
    };

    if (engine.get_option(vexa::option::MODE) == vexa::mode_t::VCFG_RECOVERY)
    {
        engine.set_callback(vexa::event_kind::INDIRECT_JUMP, v_dispatch);
        //engine.set_callback(vexa::event_kind::INSTRUCTION_LIFT, cond_br);
    }

    auto cpu = engine.get_cpu();
    auto symex = engine.get_symex();

    engine.run(address);
    if (!no_opt)
        engine.optimize();

    if (no_statistics)
        std::cout << engine.ir << std::endl;
    else
        engine.print();

    if (!recompile_option->empty())
    {
        auto recompiled = engine.recompile(true);
        
        //std::ofstream out(output_file, std::ios::binary);
        //out.write(reinterpret_cast<const char*>(recompiled.data()), recompiled.size());
        //out.close();

        engine.patch(bin, recompiled, address);
        bin.write("output.bin");
    }

    return 0;
}