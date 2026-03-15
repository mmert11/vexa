#include <vexa/vexa.h>

vexa::value::value(llvm::Value* v, const llvm::DataLayout* DL, z3::expr e, std::shared_ptr<vexa::symex> _symex) :
    val(v), DL(DL), symex(_symex)
{
    // if this value is already synchronized with symex, dont sync again
    if (symex->is_sync(v))
        return;

    z3::expr symbolic_expr = e.simplify();
    symex->set(v, symbolic_expr);
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
    return llvm::isa<llvm::ConstantInt>(val)
        ? types::concrete
        : as_expr().is_numeral()
            ? types::concrete
            : types::symbolic;
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
    return is_concrete()
        ? llvm::isa<llvm::ConstantInt>(val) // if value is concrete in llvm or z3, we find which is in and return it
            ? llvm::dyn_cast<llvm::ConstantInt>(val)->getZExtValue()
            : as_expr().get_numeral_uint64()
        : THROW("cannot concretize value");
}

uint64_t vexa::value::size() const
{
    llvm::Type *Ty = val->getType();
    uint64_t bits = DL->getTypeSizeInBits(Ty);
    return bits;
}

z3::expr vexa::value::as_expr() const
{
    return symex->get(val);
}
