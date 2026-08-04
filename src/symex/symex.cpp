#include <vexa/vexa.h>

vexa::symex::symex(vexa::context* _context)
    : z3_context(_context->z3_context), context(_context)
{}

vexa::value* vexa::symex::symbolic(std::string name, int size)
{
    values.emplace_back(z3_context->bv_const(name.c_str(), size));
    return &values.back();
}

vexa::value* vexa::symex::concrete(uint64_t val, int size)
{
    values.emplace_back(z3_context->bv_val(val, size));
    return &values.back();
}

vexa::pointer* vexa::symex::pointer(vexa::value* value, std::shared_ptr<vexa::mem_page> page)
{
    VEXA_ASSERT(value);
    pointers.emplace_back(*value, std::move(page));
    return &pointers.back();
}

vexa::pointer* vexa::symex::pointer(uint64_t value, std::shared_ptr<vexa::mem_page> page)
{
    return pointer(concrete(value, 64), std::move(page));
}

vexa::value* vexa::symex::value(z3::expr e)
{
    values.emplace_back(std::move(e));
    return &values.back();
}

vexa::value* vexa::symex::get(llvm::Value* v)
{
    VEXA_ASSERT(v);

    auto it = vars.find(v);
    if (it != vars.end())
        return it->second;

    if (auto* C = llvm::dyn_cast<llvm::ConstantInt>(v))
        return concrete(C->getZExtValue(), C->getBitWidth());

    // experimental
    //return context->cpu->emulate->run(llvm::dyn_cast<llvm::Instruction>(v)).v;

    v->print(llvm::outs());
    llvm::outs() << "\n";
    THROW("value not in symex vars");
    return nullptr;
}

void vexa::symex::set(llvm::Value* v, vexa::value* e)
{
    vars[v] = e;
}

void vexa::symex::clear()
{
    vars.clear();
    pointers.clear();
    values.clear();
}

bool vexa::symex::is_sync(llvm::Value* v)
{
    if (vars.count(v))
        return true;
    return false;
}