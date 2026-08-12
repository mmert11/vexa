#include <vexa/vexa.h>

#include <fstream>
#include <iostream>
#include <vector>

#include <CLI/CLI.hpp>

using namespace std;

int main(int argc, char **argv)
{
    {
          // initialize
        vexa::init();
        vexa::engine engine;

  // load and map binary
        vexa::binary bin("example.bin");
        engine.map_binary(bin);

  // explore and lift until recovering the function at 0x140001000
        engine.run(0x140001000);

  // apply optimizations
        engine.optimize();

  // print the lifted IR with statistics
        engine.print();

  // recompile the IR and insert in a new binary
        std::vector<uint8_t> recompiled = engine.recompile();
        engine.patch(bin, recompiled, 0x140001000);
        bin.write("output.bin");
    }
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
    bool no_fork_at_cmovs = false;

    CLI::App cli{
        "vexa-cli: command-line interface for lifting & deobfuscating binaries statically"};

    cli.add_option("-i,--input", input_file, "Path to the input binary")->required();
    cli.add_option(
           "-a,--address",
           address,
           "Virtual address of the function that will be lifted (with hex prefix 0x)")
        ->required();
    cli.add_flag("-q,--quiet", quiet, "Silent execution, no logging");
    cli.add_flag("--no-statistics", no_statistics, "Print no statistics, only ir");
    cli.add_flag("--no-opt", no_opt, "Disable optimization");
    cli.add_flag(
        "--no-constant-propagation",
        no_constant_propagation,
        "Disable Bitwuzla powered constant propagation");
    cli.add_flag(
        "--no-opaque-solving",
        no_opaque_solving,
        "Disables opaque predicate solving with Bitwuzla");
    cli.add_flag("--cfg-recovery", cfg_recovery, "Enables CFG recovery mode");
    cli.add_flag("--vcfg-recovery", vcfg_recovery, "Enables Virtual CFG recovery mode");
    cli.add_flag(
        "--cfg-path-sensitive", vcfg_path_sensitive, "Do not merge paths while preserving loops.");
    cli.add_flag("--no-fork-at-cmovs", no_fork_at_cmovs, "Doesn't fork paths at conditional move instructions.");

    auto recompile_option =
        cli.add_option(
               "-r,--recompile",
               output_file,
               "Output path for recompiling the lifted IR back into a new binary (default: 'output.bin')")
            ->expected(0, 1);

    CLI11_PARSE(cli, argc, argv);

    if (cfg_recovery && vcfg_recovery)
        return -1;

    if (output_file.empty())
        output_file = "output.bin";

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
        engine.set_option(
            vexa::option::CFG_JOIN_POLICY, vexa::cfg_join_policy_t::SPECIALIZE_BY_PATH);

    if (no_fork_at_cmovs)
            engine.set_option(vexa::option::FORK_AT_CMOVS, 0);

    engine.set_option(vexa::option::OPAQUE_SOLVING, !no_opaque_solving);
    engine.set_option(vexa::option::CONSTANT_PROPAGATION, !no_constant_propagation);
    engine.set_option(vexa::option::LOOP_REROLL, 0);
    engine.set_option(vexa::option::STATE_CLEANUP, 1);

    auto cpu = engine.get_cpu();
    auto symex = engine.get_symex();

    engine.run(address);
    if (!no_opt)
        engine.optimize();

    if (no_statistics)
        std::cout << engine.ir << std::endl;
    else
        engine.print();

    if (!recompile_option->empty()) {
        auto recompiled = engine.recompile(true);

        // std::ofstream out(output_file, std::ios::binary);
        // out.write(reinterpret_cast<const char*>(recompiled.data()), recompiled.size());
        // out.close();

        engine.patch(bin, recompiled, address);
        bin.write(output_file);
    }

    return 0;
}