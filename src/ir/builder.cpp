#include <vexa/vexa.h>

vexa::ir::builder::builder(std::shared_ptr<vexa::context> _context, std::shared_ptr<vexa::symex> _symex)
    : llvm::IRBuilder<>(*_context->llvm_context), context(_context), symex(_symex), DL(&_context->llvm_module->getDataLayout())
{
}

llvm::Type *vexa::ir::builder::get_int_ty(unsigned int size)
{
    return getIntNTy(size);
}

llvm::Function *vexa::ir::builder::create_function(std::string name, std::vector<llvm::Type *> args)
{
    llvm::FunctionType *func_type = llvm::FunctionType::get(getInt64Ty(), args, false);
    llvm::Function *_function =
        llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name, context->llvm_module.get());
    return _function;
}

vexa::value vexa::ir::builder::symvar(int param_idx, std::string name)
{
    llvm::Argument *arg = function->getArg(param_idx);
    arg->setName(name);

    vexa::value val(arg, DL,
                    symex->symbolic(name, DL->getTypeSizeInBits(arg->getType())), symex);
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
    llvm::Value *v = getIntN(bit_size, value);
    vexa::value vx(v, DL, symex->concrete(value, bit_size), symex);
    return vx;
}

llvm::Function *vexa::ir::builder::get_function()
{
    return function;
}

void vexa::ir::builder::set_function(llvm::Function *_function)
{
    function = _function;
}

llvm::BasicBlock *vexa::ir::builder::basic_block(std::string name)
{
    return llvm::BasicBlock::Create(context->llvm_module->getContext(), name, function);
}

llvm::BasicBlock *vexa::ir::builder::basic_block(std::string name, llvm::Function *_function)
{
    return llvm::BasicBlock::Create(context->llvm_module->getContext(), name, _function);
}

void vexa::ir::builder::set_ip(llvm::BasicBlock *bb)
{
    SetInsertPoint(bb);
}

void vexa::ir::builder::jump(llvm::BasicBlock *bb)
{
    CreateBr(bb);
}

void vexa::ir::builder::jump_if(vexa::value cond, llvm::BasicBlock *then_bb, llvm::BasicBlock *else_bb)
{
    CreateCondBr(cond.as_llvm(), then_bb, else_bb);
}

void vexa::ir::builder::unreachable()
{
    CreateUnreachable();
}

vexa::value vexa::ir::builder::extract(vexa::value value, uint8_t high, uint8_t low, std::string name)
{
    uint8_t v_size = value.size();
    VEXA_ASSERT(high < v_size);
    VEXA_ASSERT(low < v_size);
    VEXA_ASSERT(high >= low);

    uint8_t width = high - low + 1;
    uint64_t mask = width == 64 ? -1ULL : (1ULL << width) - 1;

    vexa::value shifted = bshr(value, get_const_int(low, v_size), name + "_shr");
    vexa::value res = band(shifted, get_const_int(mask, v_size), name);
    return res;
}

vexa::value vexa::ir::builder::resize(vexa::value value, unsigned int size, bool sign_extend)
{
    if (size == value.size())
        return value;

    llvm::Value *lvalue = value.as_llvm();
    llvm::Value *v = sign_extend ? CreateSExtOrTrunc(lvalue, getIntNTy(size)) : CreateZExtOrTrunc(lvalue, getIntNTy(size));

    // extend or truncate in z3
    z3::expr zvalue = value.as_expr();
    z3::expr final_zvalue = value.size() < size
                                ? sign_extend
                                    ? z3::sext(zvalue, size - zvalue.get_sort().bv_size())
                                    : z3::zext(zvalue, size - zvalue.get_sort().bv_size())
                                : zvalue.extract(size - 1, 0);

    vexa::value new_value(v, DL, final_zvalue, symex);
    return new_value;
}

void vexa::ir::builder::normalize(vexa::value &lhs, vexa::value &rhs, bool sign_extend)
{
    auto l_bitw = lhs.size();
    auto r_bitw = rhs.size();

    if (l_bitw == r_bitw)
        return;
    unsigned max_bitw = std::max(l_bitw, r_bitw);

    if (l_bitw != max_bitw)
        lhs = resize(lhs, max_bitw, sign_extend);
    if (r_bitw != max_bitw)
        rhs = resize(rhs, max_bitw, sign_extend);
}

vexa::value vexa::ir::builder::cmpeq(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateICmpEQ(lhs.as_llvm(), rhs.as_llvm(), name);

    z3::expr eq = lhs.as_expr() == rhs.as_expr();
    z3::expr b = z3::ite(eq, symex->concrete(1, 1), symex->concrete(0, 1));

    return vexa::value(v, DL, b, symex);
}

vexa::value vexa::ir::builder::select(vexa::value cond, vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    if (cond.size() > 1)
        THROW("condition must be 1-bit integer");

    llvm::Value *lv = CreateSelect(cond.as_llvm(), lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr cond_expr = cond.as_expr();
    z3::expr cond_bool = cond_expr.is_bool() ? cond_expr : (cond_expr == symex->concrete(1, cond.size()));
    z3::expr zv = z3::ite(cond_bool, lhs.as_expr(), rhs.as_expr());

    return vexa::value(lv, DL, zv, symex);
}

vexa::value vexa::ir::builder::add(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateAdd(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() + rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::sub(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateSub(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() - rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::mul(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateMul(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() * rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::band(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateAnd(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() & rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::bshl(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateShl(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = z3::shl(lhs.as_expr(), rhs.as_expr());
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::bshr(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateLShr(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = z3::lshr(lhs.as_expr(), rhs.as_expr());
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::bor(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateOr(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() | rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::bxor(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateXor(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() ^ rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::bnot(vexa::value lhs, std::string name)
{
    llvm::Value *v = CreateNot(lhs.as_llvm(), name);
    z3::expr zv = ~lhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

void vexa::ir::builder::ret(vexa::value v)
{
    CreateRet(v.as_llvm());
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
