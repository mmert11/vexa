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

after lifting + cobra:

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define ptr @vexa_lifted(ptr noalias returned captures(ret: address, provenance) initializes((2216, 2224)) %state, i64 %program_counter, ptr noalias readnone captures(none) %memory) local_unnamed_addr #0 {
entry:
  %RCX53521 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX53521, align 8
  %.not = icmp eq i64 %0, 1293
  %1 = add i64 %0, -30
  %2 = select i1 %.not, i64 2601, i64 %1
  %RAX53519 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %2, ptr %RAX53519, align 8
  ret ptr %state
}


*/

int main()
{
    std::string bin_file = "vmp3.3.1-branches-ultra.exe";
    std::string output_bin =

#if defined(__linux__)
        "output.bin"
#else
        "output.exe"
#endif
        ;

    uint64_t hash = 0x50e58a1eac156fe0;
    uint64_t va = 0x1400014D4;

    if (hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    // required for vmp 3.3.1
    engine.set_option(vexa::option::FORK_AT_CMOVS, 0);

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

    delete_file(output_bin);
    return 0;
}