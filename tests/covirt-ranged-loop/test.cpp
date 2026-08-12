#include <vexa/vexa.h>

#include "../tests.hpp"

/*
original function:

#include "covirt_stub.h"
uint64_t calculate(uint64_t key)
{
	__covirt_vm_start();
	
	uint64_t x = 0;

	for (unsigned int i = 0; i < 16; i++)
	{
		x += key;
	}

	__covirt_vm_end();

    return x;
}

*/

int main()
{
    std::string bin_file = "covirt-ranged-loop.covirt";
    uint64_t hash = 0x599e1944a3180377;
    uint64_t va = 0x2150;

    if (hash_file_fnv1a64(bin_file) != hash)
        return 1;

    vexa::init();
    vexa::engine engine;

    vexa::binary binary(bin_file);
    engine.map_binary(binary);


    auto vbranch = [&](vexa::engine &e) -> void {
        //getchar();
    };

    bool first_dispatch = true;
    uint64_t handler_table;

    auto v_dispatch = [&](vexa::engine &e) -> void {
        auto cpu = e.get_cpu();
        vexa::value *r9 = e.get_cpu()->read_register(vexa::amd64::R9);

        if (first_dispatch) {
            handler_table = r9->as_uint64();
            first_dispatch = false;
            //std::cout << "Found handler table: " << std::hex << r9->as_uint64() << std::endl;
        }
        else {
            if (r9->as_uint64() == handler_table) {
                uint64_t VPC = cpu->read_register(vexa::amd64::RAX)->as_uint64();
                cpu->VPC = VPC;
                cpu->VJMP = true;

                // you need to find the vpc of virtual branching instruction that is responsible from loop condition
                // there are many ways to do that
                //
                if (VPC == 0xe041)
                    e.set_option(vexa::option::OPAQUE_SOLVING, 0);
                else
                    e.set_option(vexa::option::OPAQUE_SOLVING, 1);

                //std::cout << "Next VPC: " << std::hex << VPC << std::endl;
            }
        }
    };

    engine.set_callback(vexa::event_kind::INDIRECT_JUMP, v_dispatch);
    engine.set_callback(vexa::event_kind::CONDITIONAL_FALLTHROUGH, vbranch);
    engine.set_callback(vexa::event_kind::CONDITIONAL_TAKEN, vbranch);
    engine.set_option(vexa::option::MODE, vexa::mode_t::VCFG_RECOVERY);


    engine.run(va);
    engine.optimize();

    write_file("output.ll", engine.ir);

#if defined(__linux__)
    auto recompiled = engine.recompile();
    engine.patch(binary, recompiled, va);
    binary.write("output.bin");
    if (!make_executable("output.bin")) {
        delete_file("output.bin");
        return 1;
    }

    int result = run_command("./output.bin 1293").output.find("20688") != std::string::npos;
    result &= run_command("./output.bin 129").output.find("2064") != std::string::npos;

    delete_file("output.bin");
    return !result;
#endif

    delete_file("output.bin");
    return 0;
}