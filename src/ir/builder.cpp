#include <vexa/vexa.h>

#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/ValueSymbolTable.h>

#define EXEC(val) executor->execute(llvm::dyn_cast<llvm::Instruction>(val))

llvm::Function *vexa::ir::builder::create_function(std::string name, std::vector<llvm::Type *> args)
{
    llvm::FunctionType *func_type = llvm::FunctionType::get(getInt64Ty(), args, false);
    llvm::Function *_function =
        llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name, context->llvm_module.get());
    return _function;
}

void vexa::ir::builder::add_attribute(llvm::Attribute::AttrKind attr)
{
    function->addFnAttr(attr);
}

void vexa::ir::builder::inline_asm(std::string asmCode)
{
    CreateCall(llvm::InlineAsm::get(llvm::FunctionType::get(getVoidTy(), false), asmCode, "", true));
}

void vexa::ir::builder::set_function(llvm::Function* f)
{
    function = f;
}

llvm::Function* vexa::ir::builder::get_function()
{
    return function;
}

int vexa::ir::builder::instr_count()
{
    int count = 0;
    for (const llvm::BasicBlock &BB : *function)
        count += BB.size();
    return count;
}

llvm::BasicBlock* vexa::ir::builder::basic_block(std::string name)
{
    return llvm::BasicBlock::Create(*context->llvm_context, name, function);
}

vexa::global vexa::ir::builder::global_var(llvm::Type* type, std::string name)
{
    llvm::GlobalVariable* var = new llvm::GlobalVariable(
        *context->llvm_module, type, false, llvm::GlobalValue::ExternalLinkage, nullptr, name
    );
    var->setDSOLocal(true);
    return vexa::global(var, &context->llvm_module->getDataLayout());
}

vexa::dual_value vexa::ir::builder::inbounds_gep(llvm::Type* type, llvm::Value* base, llvm::Value* offset)
{
    llvm::Value* gep = CreateInBoundsGEP(type, base, offset);
    // calculate it's symbolic expression
    vexa::shared_pointer _base = vexa::dyn_cast<vexa::pointer>(symex->get(base));
    vexa::shared_value _offset = symex->get(offset);
    vexa::shared_value new_v = symex->value(_base->as_expr() + _offset->as_expr());
    vexa::shared_pointer new_p = symex->pointer(new_v, _base->get_page());
    // set in symex
    symex->set(gep, new_p);
    return vexa::dual_value(gep, new_p);
}

vexa::dual_value vexa::ir::builder::inttoptr(llvm::Value* v, llvm::Type* destTy, std::shared_ptr<vexa::mem_page> page)
{
    llvm::Value* ptr = CreateIntToPtr(v, destTy);
    vexa::shared_pointer ptr_ = symex->pointer(symex->get(v), page);
    symex->set(ptr, ptr_);
    return vexa::dual_value(ptr, ptr_);
}

vexa::dual_value vexa::ir::builder::get_value_by_name(std::string name)
{
    llvm::ValueSymbolTable *ST = function->getValueSymbolTable();
    llvm::Value* V = ST->lookup(name);
    VEXA_ASSERT(V);
    return vexa::dual_value(V, symex->get(V));
}

void vexa::ir::builder::push_ip()
{
    insert_points.push(saveIP());
}

void vexa::ir::builder::pop_ip()
{
    VEXA_ASSERT(!insert_points.empty());
    restoreIP(insert_points.top());
    insert_points.pop();
}