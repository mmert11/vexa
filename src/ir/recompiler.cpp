#include <vexa/vexa.h>

std::vector<uint8_t> vexa::ir::builder::recompile(vexa::arch arch, llvm::Function* mainFunction, bool optimize)
{
    TRY();

    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86Disassembler();

    // create a register mapping wrapper to make the code be runnable
    if (mainFunction)
    {
        // set sysv calling convetion for all functions in module
        for (llvm::Function &F : *context->llvm_module)
            F.setCallingConv(arch == arch::x86_64 ? llvm::CallingConv::X86_64_SysV : THROW("unsupported arch"));

        llvm::Function* old_function = get_function();
        llvm::Function* entry_func = create_function("vexa_entry", std::vector<llvm::Type*>());
        set_function(entry_func);
        add_attribute(llvm::Attribute::Naked);

        llvm::BasicBlock* entry_bb = basic_block("entry");
        set_ip(entry_bb);

        std::string asmCode = 
        ".intel_syntax noprefix\n"
        "mov r10, rsp\n"
        
        "pushfq\n"
        "push r15\n"
        "push r14\n"
        "push r13\n"
        "push r12\n"
        "push r11\n"
        "push r10\n"
        "push r9\n"
        "push r8\n"
        "push r10\n"
        "push rbp\n"

        "mov r9, rdi\n"
        "mov r8, rsi\n"
        "xchg rcx, rdx\n"
        "mov rdi, rax\n"
        "mov rsi, rbx\n"

        "call " + mainFunction->getName().str() + "\n"
        "add rsp, 88\n"
        "ret\n";

        inline_asm(asmCode);
        ret(get_const_int(0, 64));
    }

    llvm::SmallVector<char, 0> ObjBuffer;
    llvm::raw_svector_ostream dest(ObjBuffer);

    llvm::Triple T(llvm::sys::getDefaultTargetTriple());
    T.setArch(arch == vexa::arch::x86_64 ? llvm::Triple::x86_64 : THROW("unimplemented arch!")); 

    std::string TripleStr = T.str();
    context->llvm_module->setTargetTriple(TripleStr);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TripleStr, Error);
    if (!Target) {
        THROW("target is null!");
    }

    llvm::TargetOptions opt;
    auto RM = llvm::Reloc::Model::PIC_;
    std::unique_ptr<llvm::TargetMachine> TM(
        Target->createTargetMachine(TripleStr, "generic", "", opt, RM, std::nullopt,
            optimize
            ? llvm::CodeGenOptLevel::Aggressive
            : llvm::CodeGenOptLevel::None
        )
    );

    llvm::legacy::PassManager pass;
    
    // i didnt understand why this code returns false if success...
    if (TM->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        THROW("re-compilation passes failed");
    }

    pass.run(*context->llvm_module);
    return std::vector<uint8_t>(ObjBuffer.begin(), ObjBuffer.end());
    CATCH();
}
