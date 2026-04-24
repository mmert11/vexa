#include <vexa/vexa.h>

#include <format>

std::vector<uint8_t> vexa::ir::builder::recompile(vexa::arch arch, llvm::Function* mainFunction, bool optimize)
{
    TRY()

    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86Disassembler();

    if (mainFunction)
    {
        vexa::ir::pass_manager manager(context);
        manager.add_pass<passes::strength_recovery::pre_compilation>();
        manager.run();

        // transform every direct load/store address (which uses inttoptr(constant)) to global variables
        // so we can fix them during relinking/relocation
        std::unordered_map<uint64_t, vexa::global> globals;
        for (auto &BB : *mainFunction)
        {
            for (auto &I : BB)
            {
                for (unsigned i = 0; i < I.getNumOperands(); ++i)
                {
                    llvm::Value* OP = I.getOperand(i);
                    if (auto *CE = llvm::dyn_cast<llvm::ConstantExpr>(OP))
                    {
                        if (CE->getOpcode() == llvm::Instruction::IntToPtr)
                        {
                            if (auto *AddrConst = llvm::dyn_cast<llvm::ConstantInt>(CE->getOperand(0)))
                            {
                                uint64_t addr = AddrConst->getZExtValue();
                                llvm::Type* type = nullptr;

                                // get the type of operand
                                if (auto* SI = llvm::dyn_cast<llvm::StoreInst>(&I))
                                    type = SI->getValueOperand()->getType();
                                else if (auto* LI = llvm::dyn_cast<llvm::LoadInst>(&I))
                                    type = LI->getType();
                                else
                                    type = llvm::Type::getInt8Ty(I.getContext());

                                // if this address is already iterated before, use saved
                                vexa::global variable = globals.count(addr)
                                                        ? globals[addr]
                                                        : globals[addr] = global_var(type, "var_" + std::format("{:x}", addr));

                                I.setOperand(i, variable.as_llvm());
                            }
                        }
                    }
                }
            }
        }

#if 0
        std::string ir_string;
        llvm::raw_string_ostream rso(ir_string);
        context->llvm_module->print(rso, nullptr);
        std::cout << ir_string << std::endl;
#endif

        // create a register mapping wrapper to make the code runnable
        // set sysv calling convetion for all functions in module
        for (llvm::Function &F : *context->llvm_module)
            F.setCallingConv(arch == arch::x86_64 ? llvm::CallingConv::X86_64_SysV : THROW("unsupported arch"));

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

    llvm::Triple T;
    if (arch == vexa::arch::x86_64)
    {
        T.setArch(llvm::Triple::x86_64);
        T.setVendor(llvm::Triple::UnknownVendor);
        T.setOS(llvm::Triple::Linux);
        T.setEnvironment(llvm::Triple::GNU);
        T.setObjectFormat(llvm::Triple::ELF);
    }
    else
    {
        THROW("unsupported arch!");
    }

    std::string TripleStr = T.str();
    context->llvm_module->setTargetTriple(TripleStr);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TripleStr, Error);
    if (!Target) {
        THROW("target is null!");
    }

    llvm::TargetOptions opt;
    llvm::Reloc::Model RM = llvm::Reloc::Model::PIC_;

    std::unique_ptr<llvm::TargetMachine> TM(
        Target->createTargetMachine(TripleStr, "generic", "", opt, RM, std::nullopt,
                                    optimize
                                    ? llvm::CodeGenOptLevel::Aggressive
                                    : llvm::CodeGenOptLevel::None
                                   )
    );
    context->llvm_module->setDataLayout(TM->createDataLayout());

    llvm::legacy::PassManager pass;
    if (TM->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        THROW("re-compilation passes failed");
    }

    pass.run(*context->llvm_module);
    return std::vector<uint8_t>(ObjBuffer.begin(), ObjBuffer.end());
    
    CATCH()
}
