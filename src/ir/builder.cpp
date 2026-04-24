#include <vexa/vexa.h>

vexa::ir::builder::builder(vexa::context* _context)
    : llvm::IRBuilder<>(*_context->llvm_context), context(_context), symex(context->symex), memory(context->memory), DL(&_context->llvm_module->getDataLayout())
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

void vexa::ir::builder::set_section(std::string section)
{
    function->setSection(section);
}

void vexa::ir::builder::add_attribute(llvm::Attribute::AttrKind attr)
{
    function->addFnAttr(attr);
}

void vexa::ir::builder::inline_asm(std::string asmCode)
{
    CreateCall(llvm::InlineAsm::get(llvm::FunctionType::get(getVoidTy(), false), asmCode, "", true));
}

void vexa::ir::builder::call(llvm::Function* func)
{
    CreateCall(func);
}

llvm::BasicBlock* vexa::ir::builder::get_insert_block()
{
    return GetInsertBlock();
}


vexa::value vexa::ir::builder::argument(int param_idx, std::string name)
{
    llvm::Argument *arg = function->getArg(param_idx);
    arg->setName(name);

    // if this function called multiple times with same param id,
    // we dont assign any new symbolic expression for it
    // in vexa::value, it checks if it already has a value
    // and if so, doesnt update with new one
    // so its fine to call this function with same param ids
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

void vexa::ir::builder::set_ip(llvm::Instruction *i)
{
    SetInsertPoint(i);
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

vexa::value vexa::ir::builder::concat(vexa::value high, vexa::value low, std::string name)
{
    unsigned high_size = high.size();
    unsigned low_size = low.size();
    unsigned total_size = high_size + low_size;

    vexa::value high_ext = resize(high, total_size, false);
    vexa::value low_ext = resize(low, total_size, false);
    vexa::value shifted_high = bshl(high_ext, get_const_int(low_size, total_size), name + "_shift");
    vexa::value res_llvm = bor(shifted_high, low_ext, name);

    z3::expr res_z3 = z3::concat(high.as_expr(), low.as_expr());
    return vexa::value(res_llvm.as_llvm(), DL, res_z3, symex);
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

vexa::global vexa::ir::builder::global_var(llvm::Type* type, std::string name)
{
    llvm::GlobalVariable* var = new llvm::GlobalVariable(
        *context->llvm_module, type, false, llvm::GlobalValue::ExternalLinkage, nullptr, name
    );
    var->setDSOLocal(true);
    return vexa::global(var, DL);
}

vexa::value vexa::ir::builder::alloca(llvm::Type *ty, z3::expr symbol, std::string name, uint64_t arraySize)
{
    llvm::AllocaInst *allocated = CreateAlloca(ty, get_const_int(arraySize, 64).as_llvm(), name);
    return vexa::value(allocated, DL, symbol, symex);
}

vexa::value vexa::ir::builder::alloca(llvm::Type *ty, std::string name, uint64_t arraySize)
{
    llvm::AllocaInst *allocated = CreateAlloca(ty, get_const_int(arraySize, 64).as_llvm(), name);
    return vexa::value(allocated, DL, symex->symbolic(name, DL->getTypeSizeInBits(ty)), symex);
}

#define PTR_TYPE() llvm::PointerType::getUnqual(*context->llvm_context)
vexa::value vexa::ir::builder::inttoptr(vexa::value v, std::string name, llvm::Type *ptr_ty)
{
    llvm::Value *lv = CreateIntToPtr(v.as_llvm(), !ptr_ty ? PTR_TYPE() : ptr_ty, name);
    z3::expr zv = v.as_expr();
    return vexa::value(lv, DL, zv, symex);
}

vexa::value vexa::ir::builder::ptrtoint(vexa::value v, uint8_t int_size, std::string name)
{
    llvm::Value *lv = CreatePtrToInt(v.as_llvm(), get_int_ty(int_size), name);
    z3::expr zv = v.as_expr();
    return vexa::value(lv, DL, zv, symex);
}

vexa::value vexa::ir::builder::inbounds_gep(llvm::Type *ty, vexa::value ptr, vexa::value offset, std::string name)
{
    llvm::Value *stackPtr = CreateInBoundsGEP(ty, ptr.as_llvm(), offset.as_llvm(), name);
    z3::expr symPtr = ptr.as_expr() + offset.as_expr();
    return vexa::value(stackPtr, DL, symPtr, symex);
}

vexa::value vexa::ir::builder::load(llvm::Type *ty, vexa::value ptr, std::string name)
{
    llvm::LoadInst *inst = CreateLoad(ty, ptr.as_llvm(), name);
    z3::expr z3_read = memory->read(ptr.as_expr(), DL->getTypeSizeInBits(ty));
    return vexa::value(inst, DL, z3_read, symex);
}

vexa::value vexa::ir::builder::_load(llvm::Type* ty, vexa::value ptr, z3::expr expression, std::string name)
{
    llvm::LoadInst *inst = CreateLoad(ty, ptr.as_llvm(), name);
    return vexa::value(inst, DL, expression, symex);
}

void vexa::ir::builder::store(vexa::value v, vexa::value ptr)
{
    llvm::Instruction *inst = CreateStore(v.as_llvm(), ptr.as_llvm());
    memory->write(ptr.as_expr(), v.as_expr());
}

void vexa::ir::builder::_store(vexa::value v, vexa::value ptr)
{
    llvm::Instruction *inst = CreateStore(v.as_llvm(), ptr.as_llvm());
}

vexa::value vexa::ir::builder::cmpeq(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateICmpEQ(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr eq = lhs.as_expr() == rhs.as_expr();
    z3::expr b = z3::ite(eq, symex->concrete(1, 1), symex->concrete(0, 1));
    return vexa::value(v, DL, b, symex);
}

vexa::value vexa::ir::builder::cmpne(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateICmpNE(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr ne = lhs.as_expr() != rhs.as_expr();
    z3::expr b = z3::ite(ne, symex->concrete(1, 1), symex->concrete(0, 1));
    return vexa::value(v, DL, b, symex);
}

vexa::value vexa::ir::builder::cmpltu(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateICmpULT(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr ult = z3::ult(lhs.as_expr(), rhs.as_expr());
    z3::expr b = z3::ite(ult, symex->concrete(1, 1), symex->concrete(0, 1));
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

vexa::value vexa::ir::builder::sdiv(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs, true);
    llvm::Value *v = CreateSDiv(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = lhs.as_expr() / rhs.as_expr();
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::srem(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs, true);
    llvm::Value *v = CreateSRem(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = z3::srem(lhs.as_expr(), rhs.as_expr());
    return vexa::value(v, DL, zv, symex);
}

vexa::value vexa::ir::builder::urem(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateURem(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = z3::urem(lhs.as_expr(), rhs.as_expr());
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

vexa::value vexa::ir::builder::bashr(vexa::value lhs, vexa::value rhs, std::string name)
{
    normalize(lhs, rhs);
    llvm::Value *v = CreateAShr(lhs.as_llvm(), rhs.as_llvm(), name);
    z3::expr zv = z3::ashr(lhs.as_expr(), rhs.as_expr());
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

vexa::value vexa::ir::builder::cttz(vexa::value val, std::string name)
{
    uint32_t size = val.size();
    llvm::Function *f = llvm::Intrinsic::getOrInsertDeclaration(
                            context->llvm_module.get(), llvm::Intrinsic::cttz, {get_int_ty(size)});

    llvm::Value *lres = CreateCall(f, {val.as_llvm(), getInt1(false)}, name);

    z3::expr x = val.as_expr();
    z3::expr res_z3 = symex->symbolic(name + "_z3", size);

    if (x.is_numeral())
    {
        uint64_t v = x.as_uint64();
        uint64_t count = 0;
        if (v == 0)
            count = size;
        else
        {
            while ((v & 1) == 0)
            {
                v >>= 1;
                count++;
            }
        }
        res_z3 = symex->concrete(count, size);
    }

    return vexa::value(lres, DL, res_z3, symex);
}

vexa::value vexa::ir::builder::bswap(vexa::value val, std::string name)
{
    uint32_t size = val.size();

    llvm::Function *f = llvm::Intrinsic::getOrInsertDeclaration(
                            context->llvm_module.get(), llvm::Intrinsic::bswap, {get_int_ty(size)});

    llvm::Value *lres = CreateCall(f, {val.as_llvm()}, name);

    z3::expr x = val.as_expr();
    z3::expr_vector args(*context->z3_context);

    if (size == 16)
    {
        args.push_back(x.extract(7, 0));
        args.push_back(x.extract(15, 8));
    }
    else if (size == 32)
    {
        args.push_back(x.extract(7, 0));
        args.push_back(x.extract(15, 8));
        args.push_back(x.extract(23, 16));
        args.push_back(x.extract(31, 24));
    }
    else if (size == 64)
    {
        args.push_back(x.extract(7, 0));
        args.push_back(x.extract(15, 8));
        args.push_back(x.extract(23, 16));
        args.push_back(x.extract(31, 24));
        args.push_back(x.extract(39, 32));
        args.push_back(x.extract(47, 40));
        args.push_back(x.extract(55, 48));
        args.push_back(x.extract(63, 56));
    }

    z3::expr res_z3 = (args.size() > 0) ? z3::concat(args) : x;

    return vexa::value(lres, DL, res_z3, symex);
}

void vexa::ir::builder::ret(vexa::value v)
{
    CreateRet(v.as_llvm());
}