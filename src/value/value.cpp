#include <vexa/vexa.h>

vexa::value::value(llvm::Value* v, const llvm::DataLayout* DL, z3::expr e, std::shared_ptr<vexa::symex> _symex) :
    val(v), DL(DL), symex(_symex)
{
    // if this value is already synchronized with symex, dont sync again
    if (symex->is_sync(v))
        return;

    z3::expr symbolic_expr = e.simplify();
    symex->set(v, symbolic_expr);

    // if value is already a constant
    if (llvm::ConstantInt* C = llvm::dyn_cast<llvm::ConstantInt>(v))
    {
        val = C;
        goto final;
    }
    // if not, use llvm's constant folding
    else if (llvm::Instruction* I = llvm::dyn_cast<llvm::Instruction>(v))
    {
        if (llvm::Constant* folded = llvm::ConstantFoldInstruction(I, *DL, nullptr))
        {
            if (llvm::ConstantInt* F = llvm::dyn_cast<llvm::ConstantInt>(folded))
            {
                val = F;
                goto final;
            }
        }
    }
    // if z3 can concretize it while llvm cant, get the concrete value from z3
    // except pointers 
    if (symbolic_expr.is_numeral() && !v->getType()->isPointerTy() && true)
        val = llvm::ConstantInt::get(v->getType(), symbolic_expr.get_numeral_uint64());

final:
    if (v != val) // means constant folded in some way
        symex->set(val, symbolic_expr);
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
