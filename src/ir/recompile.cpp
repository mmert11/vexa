#include <vexa/vexa.h>

#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/InitializePasses.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/PassRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>

#include <format>

std::vector<uint8_t>
    vexa::ir::builder::recompile(vexa::arch arch, llvm::Function *func, bool optimize)
{
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86Disassembler();

    VEXA_ASSERT(func != nullptr);
    if (llvm::verifyModule(*context->llvm_module, &llvm::errs())) {
        THROW("Module invalid before codegen!");
    }

#if 0
    std::string ir_string;
    llvm::raw_string_ostream rso(ir_string);
    context->llvm_module->print(rso, nullptr);
    std::cout << ir_string << std::endl;
#endif

    // set sysv calling convetion for all functions in module
    if (arch == arch::x86_64)
        func->setCallingConv(llvm::CallingConv::X86_64_SysV);

    llvm::Function *entry_func = create_function("vexa_entry", std::vector<llvm::Type *>());
    set_function(entry_func);
    add_attribute(llvm::Attribute::Naked);

    llvm::BasicBlock *entry_bb = basic_block("entry");
    SetInsertPoint(entry_bb);

    auto cpu = context->cpu;
    auto &DL = context->llvm_module->getDataLayout();
    auto *struct_DL = DL.getStructLayout(cpu->arch->StateStructType());
    uint64_t state_struct_size = struct_DL->getSizeInBytes().getFixedValue();

    // create a register mapping wrapper
    std::string inlineasm = ".intel_syntax noprefix\n"
                            "sub rsp, "
                            + std::to_string(state_struct_size) + "\n";

    // prologue
    //
    for (auto &reg : cpu->registers) {
        auto *r = reg.second;
        if (!r->parent && r->name != "RIP" && !r->name.ends_with("BASE") && r->size == 8
            && !r->name.starts_with("MM"))
        {
            std::string reg_lower_case = r->name;
            std::transform(
                reg_lower_case.begin(),
                reg_lower_case.end(),
                reg_lower_case.begin(),
                [](unsigned char c) {
                    return std::tolower(c);
                });

            std::string s =
                std::format("mov qword ptr [rsp + {}], {}\n", r->offset, reg_lower_case);
            inlineasm.append(s);
        }
    }
    std::string prologue_rsp_fix = std::format(
        "add qword ptr [rsp + {}], {}\n", cpu->registers[amd64::SP]->offset, state_struct_size);
    inlineasm.append(prologue_rsp_fix);
    inlineasm.append("mov rdi, rsp\n"); // STATE POINTER
    inlineasm.append("xor rsi, rsi\n"); // PROGRAM COUNTER

    // MEMORY POINTER
    vexa::global base = global_var(getInt8Ty(), "IMAGE_BASE");
    inlineasm.append("lea rdx, [rip + " + base.name() + "]\n");
    //inlineasm.append("xor rdx, rdx\n");

    // init fsbase
    auto *fsbase = cpu->registers[amd64::FSBASE];
    inlineasm.append("rdfsbase rax\n");
    inlineasm.append(std::format("mov qword ptr [rsp + {}], rax\n", fsbase->offset));

    // function call
    //
    inlineasm.append("call " + func->getName().str() + "\n");

    // epilogue
    //
    for (auto &reg : cpu->registers) {
        auto *r = reg.second;
        if (!r->parent && r->name != "RIP" && r->name != "RSP" && !r->name.ends_with("BASE")
            && r->size == 8 && !r->name.starts_with("MM"))
        {
            std::string reg_lower_case = r->name;
            std::transform(
                reg_lower_case.begin(),
                reg_lower_case.end(),
                reg_lower_case.begin(),
                [](unsigned char c) {
                    return std::tolower(c);
                });

            std::string s =
                std::format("mov {}, qword ptr [rsp + {}]\n", reg_lower_case, r->offset);
            inlineasm.append(s);
        }
    }
    inlineasm.append(std::format("add rsp, {}\n", state_struct_size));

    inline_asm(inlineasm);
    CreateRet(getIntN(0, 64));

    llvm::SmallVector<char, 0> ObjBuffer;
    llvm::raw_svector_ostream dest(ObjBuffer);

    llvm::Triple T;
    if (arch == vexa::arch::x86_64) {
        T.setArch(llvm::Triple::x86_64);
        T.setVendor(llvm::Triple::UnknownVendor);
        T.setOS(llvm::Triple::Linux);
        T.setEnvironment(llvm::Triple::GNU);
        T.setObjectFormat(llvm::Triple::ELF);
    }
    else {
        THROW("unsupported arch!");
    }
    context->llvm_module->setTargetTriple(T);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(T, Error);
    VEXA_ASSERT(Target);

    llvm::TargetOptions opt;
    llvm::Reloc::Model RM = llvm::Reloc::Model::PIC_;
    std::unique_ptr<llvm::TargetMachine> TM(Target->createTargetMachine(
        T,
        "generic",
        "",
        opt,
        RM,
        std::nullopt,
        optimize ? llvm::CodeGenOptLevel::Aggressive : llvm::CodeGenOptLevel::None));
    VEXA_ASSERT(TM);
    context->llvm_module->setDataLayout(TM->createDataLayout());

    llvm::legacy::PassManager pass;
    if (TM->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        THROW("compilation passes failed");
    }

    pass.run(*context->llvm_module);
    return std::vector<uint8_t>(ObjBuffer.begin(), ObjBuffer.end());
}
