#include <vexa/vexa.h>

#include "../tests.hpp"

int main()
{
    std::string bin_file = "covirt_symbolic_loop.covirt";
    uint64_t hash = 0xc9e89fe974bc4bde;
    uint64_t va = 0x2150;

    if (hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    vexa::binary binary(bin_file);
    engine.map_binary(binary);

    bool first_dispatch = true;
    uint64_t handler_table;
    auto v_dispatch = [&](vexa::engine& e) -> void
    {
        auto cpu = e.get_cpu();
        vexa::value* r9 = e.get_cpu()->read_register(vexa::amd64::R9);

        if (first_dispatch)
        {
            handler_table = r9->as_uint64();
            first_dispatch = false;
            //std::cout << "Found handler table: " << std::hex << r9->as_uint64() << std::endl;
        }
        else
        {
            if (r9->as_uint64() == handler_table)
            {
                uint64_t VPC = cpu->read_register(vexa::amd64::RAX)->as_uint64();
                cpu->VPC = VPC;
                cpu->VJMP = true;
                //std::cout << "Next VPC: " << std::hex << VPC << std::endl;
            }
        }
    };

    engine.set_callback(vexa::event_kind::INDIRECT_JUMP, v_dispatch);
    engine.set_option(vexa::option::MODE, vexa::mode_t::VCFG_RECOVERY);

    engine.run(va);
    engine.optimize();

    write_file("output.ll", engine.ir);

#if defined (__linux__)
    auto recompiled = engine.recompile();
    engine.patch(binary, recompiled, va);
    binary.write("output.bin");
    if (!make_executable("output.bin")) {
        delete_file("output.bin");
        return 1;
    }

    auto result = run_command("./output.bin 123");
    delete_file("output.bin");
    return std::stoi(result.output) != 7503;
#endif

    return 0;
}