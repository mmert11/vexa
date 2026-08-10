#include <vexa/vexa.h>

#include "../tests.hpp"

int main()
{
    std::string bin_file = "binaryshield.exe";
    std::string output_bin = 

#if defined(__linux__)
    "output.bin"
#else
    "output.exe"
#endif
    ;

    uint64_t hash = 0x8368a2bce524798c;
    uint64_t va = 0x1400016d0;

    if (hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    engine.set_option(vexa::option::FORK_AT_CMOVS, 1);

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
        int result = run_command("echo \"1859\" | wine " + output_bin).output.find("Correct Key!")
                      != std::string::npos;
        result &= run_command("echo \"123\" | wine " + output_bin).output.find("Incorrect Key.")
                  != std::string::npos;

        delete_file(output_bin);
        return !result;
    }

#else

    int result = run_command("echo \"1859\" | " + output_bin).output.find("Correct Key!")
                 != std::string::npos;
    result &= run_command("echo \"123\" | " + output_bin).output.find("Incorrect Key.")
              != std::string::npos;

    delete_file(output_bin);
    return !result;
#endif

    return 0;
}