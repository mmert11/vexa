#include <vexa/vexa.h>

vexa::context::context()
{
    llvm_context = std::make_shared<llvm::LLVMContext>();
    llvm_module = std::make_shared<llvm::Module>("vexa", *llvm_context);
    z3_context = std::make_shared<z3::context>();
}