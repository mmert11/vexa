#include <vexa/vexa.h>

vexa::value::value(llvm::Value* v, const llvm::DataLayout* DL) : DL(DL)
{
    if (llvm::Constant* C = llvm::dyn_cast<llvm::Constant>(v))
    {
        val = C;
        return;
    }

    if (llvm::Instruction* I = llvm::dyn_cast<llvm::Instruction>(v))
    {
        if (llvm::Constant* F = llvm::ConstantFoldInstruction(I, *DL, nullptr))
        {
            val = F;
            return;
        }
    }

    val = v;
}

std::string vexa::value::name() const
{
    return val->getName().str();
}

llvm::Value* vexa::value::as_llvm() const
{
    return val;
}

vexa::value::types vexa::value::type() const
{
    return llvm::isa<llvm::Constant>(val) ? types::concrete : types::symbolic;
}

bool vexa::value::is_symbolic() const
{
    return type() == types::symbolic;
}

bool vexa::value::is_concrete() const
{
    return type() == types::concrete;
}

uint64_t vexa::value::as_uint64() const
{
    return is_concrete() ? llvm::dyn_cast<llvm::ConstantInt>(val)->getZExtValue() : THROW("cannot concretize value");
}

uint64_t vexa::value::size() const
{
    llvm::Type *Ty = val->getType();
    uint64_t bits = DL->getTypeSizeInBits(Ty);
    return bits;
}
