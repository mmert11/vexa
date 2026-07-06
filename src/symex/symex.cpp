#include <vexa/vexa.h>

vexa::symex::symex(vexa::context* _context) : context(_context)
{}

std::shared_ptr<vexa::value> vexa::symex::symbolic(std::string name, int size)
{
    return std::make_shared<vexa::value>(vexa::value(context->z3_context->bv_const(name.c_str(), size)));
}

std::shared_ptr<vexa::value> vexa::symex::concrete(uint64_t val, int size)
{
    return std::make_shared<vexa::value>(context->z3_context->bv_val(val, size));
}

vexa::shared_pointer vexa::symex::pointer(vexa::shared_value value, std::shared_ptr<vexa::mem_page> page)
{
    vexa::shared_pointer p = std::make_shared<vexa::pointer>(*value, page);
    return p;
}

std::shared_ptr<vexa::value> vexa::symex::value(z3::expr e)
{
    std::shared_ptr<vexa::value> v = std::make_shared<vexa::value>(e);
    return v;
}

std::shared_ptr<vexa::value> vexa::symex::get(llvm::Value* v)
{
    VEXA_ASSERT(v);

    auto it = vars.find(v);
    if (it != vars.end())
        return it->second;

    if (auto* C = llvm::dyn_cast<llvm::ConstantInt>(v))
        return concrete(C->getZExtValue(), C->getBitWidth());

    v->print(llvm::outs());
    llvm::outs() << "\n";
    THROW("value not in symex vars");
    return nullptr;
}

void vexa::symex::set(llvm::Value* v, std::shared_ptr<vexa::value> e)
{
    //e->simplify();
    vars[v] = e;
}

bool vexa::symex::is_sync(llvm::Value* v)
{
    if (vars.count(v))
        return true;
    return false;
}