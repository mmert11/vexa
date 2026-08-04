#include <vexa/vexa.h>

#include "../tests.hpp"

int main()
{
    std::string bin_file = "covirt_branches.covirt";
    uint64_t hash = 3792093018525610720;
    uint64_t va = 0x2150;

    if (vexa::utils::hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    vexa::binary binary(bin_file);
    engine.map_binary(binary);
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

    int result = 0;
    result &= run_command("./output.bin 1293").exit_code;
    result &= run_command("./output.bin 911").exit_code;
    result &= run_command("./output.bin 1453").exit_code;
    result &= run_command("./output.bin 43242").exit_code == 0;

    delete_file("output.bin");
    return result;
#endif

    return 0;
}