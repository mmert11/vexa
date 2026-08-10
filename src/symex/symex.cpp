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
    values.emplace_back(context, specialize(std::move(e)), *term_manager, *solver);
    return &values.back();
}

vexa::value *vexa::symex::get(llvm::Value *v)
{
    VEXA_ASSERT(v);

    auto cached = specialized_vars.find(v);
    if (cached != specialized_vars.end())
        return cached->second;

    auto it = vars.find(v);
    if (it != vars.end()) {
        // specialized variables, these are not safe to concretize
        //
        vexa::value *original = it->second;
        bw::Term term = specialize(original->as_expr());
        if (term == original->as_expr())
            return original;

        values.emplace_back(context, std::move(term), *term_manager, *solver);
        vexa::value *specialized = &values.back();
        if (auto *ptr = vexa::to_ptr(original))
            specialized = pointer(specialized, ptr->get_page());
        specialized_vars.insert({v, specialized});
        return specialized;
    }

    if (auto *C = llvm::dyn_cast<llvm::ConstantInt>(v))
        return concrete(C->getZExtValue(), C->getBitWidth());

    // experimental
    // return context->cpu->emulate->run(llvm::dyn_cast<llvm::Instruction>(v)).v;

    v->print(llvm::outs());
    llvm::outs() << "\n";
    THROW("value not in symex vars");
    return nullptr;
}

void vexa::symex::set(llvm::Value *v, vexa::value *e)
{
    vars[v] = e;
    if (!specialized_vars.empty())
        specialized_vars.erase(v);
}

void vexa::symex::clear()
{
    vars.clear();
    clear_specialization();
    pointers.clear();
    values.clear();
}

void vexa::symex::specialize(const bw::Term &from, const bw::Term &to)
{
    auto it = substitutions.find(from);
    if (it != substitutions.end() && it->second == to)
        return;

    substitutions.insert_or_assign(from, to);
    specialized_vars.clear();
}

void vexa::symex::clear_specialization()
{
    specialized_vars.clear();
    substitutions.clear();
}
bw::Term vexa::symex::specialize(bw::Term term)
{
    if (substitutions.empty())
        return term;

    bw::Term specialized = term_manager->substitute_term(term, substitutions);
    if (specialized != term)
        return solver->simplify(specialized);
    return term;
}

bool vexa::symex::is_sync(llvm::Value *v)
{
    return vars.contains(v);
}

vexa::symex_state vexa::symex::take_snapshot() const
{
    return {specialized_vars, substitutions};
}

void vexa::symex::restore_snapshot(vexa::symex_state state)
{
    specialized_vars = std::move(state.specialized_vars);
    substitutions = std::move(state.substitutions);
}
