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

define ptr @vexa_lifted(ptr noalias returned captures(ret: address, provenance) initializes((2216, 2224)) %state, i64 %program_counter, ptr noalias readnone captures(none) %memory) local_unnamed_addr #0 {
entry:
  %RCX95892 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX95892, align 8
  %1 = sub i64 1292, %0
  %2 = sub i64 -9223372036854774515, %0
  %3 = and i64 %2, %1
  %4 = shl i64 %0, 1
  %5 = add i64 %4, 15
  %6 = add i64 %0, -30
  %7 = icmp slt i64 %3, 0
  %8 = select i1 %7, i64 %5, i64 %6
  %RAX95890 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %8, ptr %RAX95890, align 8
  ret ptr %state
}

*/

int main()
{
    std::string bin_file = "vmp2-branches-ultra.exe";
    std::string output_bin =

#if defined(__linux__)
        "output.bin"
#else
        "output.exe"
#endif
        ;

    uint64_t hash = 0x69822fb4567f07ca;
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

    delete_file(output_bin);
    return 0;
}