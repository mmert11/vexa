#include <vexa/vexa.h>

vexa::ir::recompiler::recompiler(std::shared_ptr<vexa::context> c) : context(c) {}

std::vector<uint8_t> vexa::ir::recompiler::recompile(vexa::arch arch, bool optimize)
{
    TRY();
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();

    llvm::SmallVector<char, 0> ObjBuffer;
    llvm::raw_svector_ostream dest(ObjBuffer);

    llvm::Triple T(llvm::sys::getDefaultTargetTriple());
    T.setArch(arch == vexa::arch::x64 ? llvm::Triple::x86_64 : THROW("unimplemented arch!")); 

    std::string TripleStr = T.str();
    context->llvm_module->setTargetTriple(TripleStr);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TripleStr, Error);
    if (!Target) {
        THROW("target is null! but why?");
    }

    llvm::TargetOptions opt;
    auto RM = llvm::Reloc::Model::PIC_;
    std::unique_ptr<llvm::TargetMachine> TM(
        Target->createTargetMachine(TripleStr, "generic", "", opt, RM, std::nullopt,
            optimize ?
            llvm::CodeGenOptLevel::Aggressive :
            llvm::CodeGenOptLevel::None
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
