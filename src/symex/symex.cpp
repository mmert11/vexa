#include <vexa/vexa.h>

vexa::symex::symex(vexa::context *_context)
    : context(_context)
{}

vexa::value *vexa::symex::symbolic(std::string name, int size)
{
    values.emplace_back(context, context->bitwright_ctx.symbol(std::move(name), size));
    return &values.back();
}

vexa::value *vexa::symex::concrete(uint64_t val, int size)
{
    values.emplace_back(context, context->bitwright_ctx.constant(size, val));
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

vexa::value *vexa::symex::value(bwr::Expr e)
{
    values.emplace_back(context, std::move(e));
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

    if (auto *C = llvm::dyn_cast<llvm::ConstantInt>(v)) {
        uint64_t val = (C->getBitWidth() <= 64) ? C->getZExtValue() : C->getValue().getRawData()[0];
        return concrete(val, C->getBitWidth());
    }

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
