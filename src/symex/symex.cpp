#include <vexa/vexa.h>

vexa::symex::symex(vexa::context *_context)
    : term_manager(&_context->term_manager), solver(_context->bitwuzla.get()), context(_context)
{}

vexa::value *vexa::symex::symbolic(std::string name, int size)
{
    bw::Sort sort = term_manager->mk_bv_sort(size);
    values.emplace_back(
        context, term_manager->mk_const(sort, std::move(name)), *term_manager, *solver);
    return &values.back();
}

vexa::value *vexa::symex::concrete(uint64_t val, int size)
{
    bw::Sort sort = term_manager->mk_bv_sort(size);
    values.emplace_back(
        context, term_manager->mk_bv_value_uint64(sort, val), *term_manager, *solver);
    return &values.back();
}

vexa::pointer *vexa::symex::pointer(vexa::value *value, std::shared_ptr<vexa::mem_page> page)
{
    VEXA_ASSERT(value);
    pointers.emplace_back(*value, std::move(page));
    return &pointers.back();
}

vexa::pointer *vexa::symex::pointer(uint64_t value, std::shared_ptr<vexa::mem_page> page)
{
    return pointer(concrete(value, 64), std::move(page));
}

vexa::value *vexa::symex::value(bw::Term e)
{
    values.emplace_back(context, std::move(e), *term_manager, *solver);
    return &values.back();
}

vexa::value *vexa::symex::get(llvm::Value *v)
{
    VEXA_ASSERT(v);

    auto it = vars.find(v);
    if (it != vars.end()) {
        vexa::value *original = it->second;
        return original;
    }

    if (auto *C = llvm::dyn_cast<llvm::ConstantInt>(v))
        return concrete(C->getZExtValue(), C->getBitWidth());

    if (auto *inst = llvm::dyn_cast<llvm::Instruction>(v)) {
        if (!llvm::isa<llvm::LoadInst>(inst) && !llvm::isa<llvm::StoreInst>(inst)) {
            vexa::value *evaluated = context->cpu->emulate->visit(inst);
            vars[inst] = evaluated;
            return evaluated;
        }
    }

    v->print(llvm::outs());
    llvm::outs() << "\n";
    THROW("value not in symex vars");
    return nullptr;
}

void vexa::symex::set(llvm::Value *v, vexa::value *e)
{
    vars[v] = e;
}

void vexa::symex::erase(llvm::Value *v)
{
    vars.erase(v);
}

void vexa::symex::clear()
{
    vars.clear();
    pointers.clear();
    values.clear();
}

bool vexa::symex::is_sync(llvm::Value *v)
{
    return vars.count(v);
}
