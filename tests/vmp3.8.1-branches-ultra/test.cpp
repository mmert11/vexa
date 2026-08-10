#include <vexa/vexa.h>

#include "../tests.hpp"

/*
original function:

uint64_t calculate(uint64_t key)
{
    uint64_t x = 0;
    if (key == 1293)
        x = key * 2 + 15;
    else
        x = key - 30;

    return x;
}

*/

int main()
{
    std::string bin_file = "vmp3.8.1-branches-ultra.exe";
    std::string output_bin =

#if defined(__linux__)
        "output.bin"
#else
        "output.exe"
#endif
        ;

    uint64_t hash = 0xe1b346566002e4df;
    uint64_t va = 0x1400014D4;

    if (hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    vexa::binary binary(bin_file);
    engine.map_binary(binary);
    engine.run(va);
    engine.optimize();

    write_file("output.ll", engine.ir);

#if defined(__linux__)
    auto recompiled = engine.recompile();
    engine.patch(binary, recompiled, va);
    binary.write(output_bin);
    if (!make_executable(output_bin)) {
        delete_file(output_bin);
        return 1;
    }

    if (is_wine_installed()) {
        int result =
            run_command("wine " + output_bin + " 1293").output.find("2601") != std::string::npos;

        result &=
            run_command("wine " + output_bin + " 1292").output.find("1262") != std::string::npos;
        delete_file(output_bin);
        return !result;
    }

#else

    int result = run_command(output_bin + " 1293").output.find("2601") != std::string::npos;
    result &= run_command(output_bin + " 1292").output.find("1262") != std::string::npos;
    delete_file(output_bin);
    return !result;
#endif

    return 0;
}