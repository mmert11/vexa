#include <vexa/vexa.h>

#include <remill/OS/OS.h>
#include <remill/Arch/Name.h>

vexa::amd64::Amd64Cpu::Amd64Cpu(vexa::context* _context) : vexa::cpu(_context)
{
    str_to_reg = {
        {"RAX", RAX}, {"RBX", RBX}, {"RCX", RCX}, {"RDX", RDX},
        {"RSI", RSI}, {"RDI", RDI}, {"RSP", RSP}, {"RBP", RBP},
        {"RIP", RIP}, {"R8", R8},   {"R9", R9},   {"R10", R10},
        {"R11", R11}, {"R12", R12}, {"R13", R13}, {"R14", R14},
        {"R15", R15}, {"R8D", R8D}, {"R9D", R9D}, {"R10D", R10D},
        {"R11D", R11D}, {"R12D", R12D}, {"R13D", R13D}, {"R14D", R14D},
        {"R15D", R15D}, {"R8W", R8W}, {"R9W", R9W}, {"R10W", R10W},
        {"R11W", R11W}, {"R12W", R12W}, {"R13W", R13W}, {"R14W", R14W},
        {"R15W", R15W}, {"EAX", EAX}, {"EBX", EBX}, {"ECX", ECX},
        {"EDX", EDX}, {"ESI", ESI}, {"EDI", EDI}, {"ESP", ESP},
        {"EBP", EBP}, {"EIP", EIP}, {"AX", AX},   {"BX", BX},
        {"CX", CX},   {"DX", DX},   {"SI", SI},   {"DI", DI},
        {"SP", SP},   {"BP", BP},   {"IP", IP},   {"AH", AH},
        {"BH", BH},   {"CH", CH},   {"DH", DH},   {"AL", AL},
        {"BL", BL},   {"CL", CL},   {"DL", DL},   {"SIL", SIL},
        {"DIL", DIL}, {"SPL", SPL}, {"BPL", BPL}, {"R8B", R8B},
        {"R9B", R9B}, {"R10B", R10B}, {"R11B", R11B}, {"R12B", R12B},
        {"R13B", R13B}, {"R14B", R14B}, {"R15B", R15B}, {"PC", amd64::PC},
        {"SS", SS},   {"ES", ES},   {"GS", GS},   {"FS", FS},
        {"DS", DS},   {"CS", CS},   {"GSBASE", GSBASE}, {"FSBASE", FSBASE},
        {"XMM0", XMM0}, {"XMM1", XMM1}, {"XMM2", XMM2}, {"XMM3", XMM3},
        {"XMM4", XMM4}, {"XMM5", XMM5}, {"XMM6", XMM6}, {"XMM7", XMM7},
        {"XMM8", XMM8}, {"XMM9", XMM9}, {"XMM10", XMM10}, {"XMM11", XMM11},
        {"XMM12", XMM12}, {"XMM13", XMM13}, {"XMM14", XMM14}, {"XMM15", XMM15},
        {"ST0", ST0}, {"ST1", ST1}, {"ST2", ST2}, {"ST3", ST3},
        {"ST4", ST4}, {"ST5", ST5}, {"ST6", ST6}, {"ST7", ST7},
        {"MM0", MM0}, {"MM1", MM1}, {"MM2", MM2}, {"MM3", MM3},
        {"MM4", MM4}, {"MM5", MM5}, {"MM6", MM6}, {"MM7", MM7},
        {"AF", AF},   {"CF", CF},   {"DF", DF},   {"OF", OF},
        {"PF", PF},   {"SF", SF},   {"ZF", ZF}
    };

    arch = remill::Arch::Build(context->llvm_context.get(), remill::kOSLinux, remill::kArchAMD64);
    initialize_arch();
}

remill::Register* vexa::amd64::Amd64Cpu::get_return_register()
{
    return get_register(amd64::RAX);
}