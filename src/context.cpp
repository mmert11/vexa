#include <vexa/vexa.h>

vexa::context::context(vexa::arch _arch)
{
    llvm_context = std::make_unique<llvm::LLVMContext>();
    z3_context = std::make_shared<z3::context>();

    auto shared = (this);
    memory = std::make_shared<vexa::memory>(shared);
    symex = std::make_shared<vexa::symex>(shared);
    builder = std::make_shared<ir::builder>(shared);

    // init cpu
    if (_arch == arch::x86_64)
        cpu = std::make_shared<vexa::amd64::Amd64Cpu>(shared);
    else
        THROW("only x86_64 is supported");

    MarkerFunc = llvm_module->getOrInsertFunction(
                 "vexa.unroll.marker",
                 llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), false));

    llvm::dyn_cast<llvm::Function>(MarkerFunc.getCallee())->setOnlyWritesMemory();
    llvm::dyn_cast<llvm::Function>(MarkerFunc.getCallee())->setVisibility(llvm::GlobalValue::DefaultVisibility);
    llvm::dyn_cast<llvm::Function>(MarkerFunc.getCallee())->setLinkage(llvm::GlobalValue::ExternalLinkage);
}