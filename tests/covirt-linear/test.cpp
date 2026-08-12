#include <vexa/vexa.h>

#include "../tests.hpp"

int main()
{
    std::string bin_file = "covirt_linear.covirt";
    uint64_t hash = 1309043602961784921;
    uint64_t va = 0x2180;

    if (hash_file_fnv1a64(bin_file) != hash)
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

    auto result = run_command("./output.bin 735291846");
    delete_file("output.bin");
    return result.exit_code;
#endif

    delete_file("output.bin");
    return 0;
}