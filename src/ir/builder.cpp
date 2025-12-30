#include <vexa/vexa.h>

vexa::ir::builder::builder(std::shared_ptr<vexa::context> _context, std::shared_ptr<vexa::symex> _symex)
    : llvm::IRBuilder<>(*_context->llvm_context), context(_context), symex(_symex), DL(&_context->llvm_module->getDataLayout())
{
}

llvm::Type* vexa::ir::builder::get_int_ty(unsigned int size)
{
    return getIntNTy(size);
}

llvm::Function* vexa::ir::builder::create_function(std::string name, std::vector<llvm::Type*> args)
{
    llvm::FunctionType* func_type = llvm::FunctionType::get(getInt64Ty(), args, false);
    llvm::Function* _function = 
        llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name, context->llvm_module.get());
    return _function;
}

vexa::value vexa::ir::builder::symvar(int param_idx, std::string name)
{
    llvm::Argument* arg = function->getArg(param_idx);
    arg->setName(name);

    vexa::value val(arg, &context->llvm_module->getDataLayout());
    symex->set(arg, symex->symbolic(name, DL->getTypeSizeInBits(arg->getType())));
    return val;
}

int vexa::ir::builder::get_instr_count()
{
    int count = 0;
    for (const llvm::BasicBlock &BB : *function)
        count += BB.size();
    return count;
}

vexa::value vexa::ir::builder::get_const_int(uint64_t value, int bit_size)
{
    llvm::Value* v = getIntN(bit_size, value);
    vexa::value vx(v, DL);
    symex->set(v, symex->concrete(value, bit_size));
    return vx;
}

llvm::Function* vexa::ir::builder::get_function()
{
    return function;
}

void vexa::ir::builder::set_function(llvm::Function* _function)
{
    function = _function;
}

llvm::BasicBlock* vexa::ir::builder::basic_block(std::string name)
{
    return llvm::BasicBlock::Create(context->llvm_module->getContext(), name, function);
}

llvm::BasicBlock* vexa::ir::builder::basic_block(std::string name, llvm::Function* _function)
{
    return llvm::BasicBlock::Create(context->llvm_module->getContext(), name, _function);
}

void vexa::ir::builder::set_ip(llvm::BasicBlock* bb)
{
    SetInsertPoint(bb);
}

void vexa::ir::builder::jump(llvm::BasicBlock* bb)
{
    CreateBr(bb);
}

void vexa::ir::builder::jump_if(vexa::value cond, llvm::BasicBlock* then_bb, llvm::BasicBlock* else_bb)
{
    CreateCondBr(cond.as_llvm(), then_bb, else_bb);
}

vexa::value vexa::ir::builder::resize(vexa::value value, unsigned int size, bool sign_extend)
{
    if (size == value.size())
        return value;

    llvm::Value* lvalue = value.as_llvm();
    llvm::Value* v = sign_extend ?
                    CreateSExtOrTrunc(lvalue, getIntNTy(size), lvalue->getName().str()) :
                    CreateZExtOrTrunc(lvalue, getIntNTy(size), lvalue->getName().str());
    vexa::value new_value(v, DL);

    // extend or truncate in z3
    z3::expr zvalue = symex->get(lvalue);
    symex->set(
        v,
        value.size() < size
            ? (sign_extend ?
                z3::sext(zvalue, size - zvalue.get_sort().bv_size()) :
                z3::zext(zvalue, size - zvalue.get_sort().bv_size()))
            : symex->get(lvalue).extract(size - 1, 0)
    );

    return new_value;
}

void vexa::ir::builder::normalize(vexa::value& lhs, vexa::value& rhs, bool sign_extend)
{
    auto l_bitw = lhs.size();
    auto r_bitw = rhs.size();
    
    if (l_bitw == r_bitw) return;
    unsigned max_bitw = std::max(l_bitw, r_bitw);

    if (l_bitw != max_bitw) lhs = resize(lhs, max_bitw, sign_extend);
    if (r_bitw != max_bitw) rhs = resize(rhs, max_bitw, sign_extend);
}

vexa::value vexa::ir::builder::cmpeq(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateICmpEQ(lhs.as_llvm(), rhs.as_llvm(), name);

    z3::expr eq = symex->get(lhs.as_llvm()) == symex->get(rhs.as_llvm());
    z3::expr b = z3::ite(eq, symex->concrete(1, 1), symex->concrete(0, 1));
    symex->set(v, b);
    
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::select(vexa::value cond, vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    if (cond.size() > 1)
        THROW("condition must be 1-bit integer");

    llvm::Value* lv = CreateSelect(cond.as_llvm(), lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr cond_expr = symex->get(cond.as_llvm());
    z3::expr cond_bool = cond_expr.is_bool() ? cond_expr : (cond_expr == symex->concrete(1, cond.size()));
    z3::expr zv = z3::ite(cond_bool, symex->get(lhs.as_llvm()), symex->get(rhs.as_llvm()));
    
    symex->set(lv, zv);
    return vexa::value(lv, DL);
}

vexa::value vexa::ir::builder::add(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateAdd(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) + symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::sub(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateSub(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) - symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::mul(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateMul(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) * symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::band(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateAnd(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) & symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::bshl(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateShl(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, z3::shl(symex->get(lhs.as_llvm()), symex->get(rhs.as_llvm())));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::bshr(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateLShr(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, z3::lshr(symex->get(lhs.as_llvm()), symex->get(rhs.as_llvm())));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::bor(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateOr(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) | symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::bxor(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value* v = CreateXor(lhs.as_llvm(), rhs.as_llvm(), name);
    symex->set(v, symex->get(lhs.as_llvm()) ^ symex->get(rhs.as_llvm()));
    return vexa::value(v, DL);
}

vexa::value vexa::ir::builder::bnot(vexa::value lhs, std::string name)
{
    llvm::Value* v = CreateNot(lhs.as_llvm(), name);
    symex->set(v, ~symex->get(lhs.as_llvm()));
    return vexa::value(v, DL);
}

void vexa::ir::builder::ret(llvm::Value* v)
{
    CreateRet(v);
}

void vexa::ir::builder::optimize()
{
    TRY()
    if (!this->context->llvm_module)
		THROW("module is null!");

	if (llvm::verifyModule(*this->context->llvm_module, &llvm::errs()))
		THROW("invalid IR before optimization!");

	llvm::LoopAnalysisManager LAM;
	llvm::FunctionAnalysisManager FAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::ModuleAnalysisManager MAM;
	llvm::PassBuilder PB;

	PB.registerModuleAnalyses(MAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
	MPM.run(*this->context->llvm_module, MAM);
    CATCH()
}
