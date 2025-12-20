#include <vexa/vexa.h>

vexa::symex::symex(std::shared_ptr<vexa::context> _context) : context(_context)
{}

z3::expr vexa::symex::symbolic(std::string name, int size)
{
    return context->z3_context->bv_const(name.c_str(), size);
}

z3::expr vexa::symex::concrete(uint64_t val, int size)
{
    return context->z3_context->bv_val(val, size);
}

z3::expr vexa::symex::get(llvm::Value* v)
{
    return *vars.at(v);
}

void vexa::symex::set(llvm::Value* v, z3::expr e)
{
    if (vars.find(v) != vars.end())
    {
        if (!llvm::isa<llvm::Constant>(v) && get(v).as_uint64() == llvm::dyn_cast<llvm::ConstantInt>(v)->getZExtValue())
            THROW("SSA error, something wrong here");
        return;
    }

    vars[v] = std::make_shared<z3::expr>(e.simplify());
}