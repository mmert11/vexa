#include <vexa/vexa.h>

vexa::global::global(llvm::GlobalVariable* _lval, const llvm::DataLayout* _DL) : val(_lval), DL(_DL)
{

}

uint64_t vexa::global::size() const
{
    llvm::Type *Ty = val->getType();
    uint64_t bits = DL->getTypeSizeInBits(Ty);
    return bits;
}

std::string vexa::global::name() const
{
    return val->getName().str();
}
llvm::Value* vexa::global::as_llvm() const
{
    return val;
}