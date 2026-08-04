#include <vexa/vexa.h>

void vexa::cpu::emulator::run_block(llvm::BasicBlock* BB)
{
    for (auto &I : llvm::make_early_inc_range(*BB))
    {
        if (I.isTerminator())
            continue;

        llvm::Value* I_ptr = &I;
        vexa::value* res = visit(I);

        symex->set(I_ptr, res);
    }
}


void vexa::cpu::emulator::write_memory_intrinsic(llvm::CallInst& call, size_t size)
{
    builder->SetInsertPoint(&call);
    // get pointer operand expression
    vexa::dual_value ptr = cpu->value_to_pointer(call.getOperand(1));
    vexa::pointer* ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // get value operand expression
    llvm::Value* val = call.getOperand(2);
    vexa::value* val_sym = VEXA_SYM_VAL(val);
    VEXA_ASSERT(val_sym->size() == val->getType()->getPrimitiveSizeInBits());

    // create actual store
    run(builder->CreateStore(val, ptr.l));
    call.replaceAllUsesWith(call.getArgOperand(0));
}

void vexa::cpu::emulator::read_memory_intrinsic(llvm::CallInst& call, size_t size)
{
    builder->SetInsertPoint(&call);
    // get pointer operand expression
    vexa::dual_value ptr = cpu->value_to_pointer(call.getOperand(1));
    vexa::pointer* ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // create actual load
    llvm::Value* load = run(builder->CreateLoad(builder->getIntNTy(size), ptr.l)).l;
    call.replaceAllUsesWith(load);
}

vexa::value* vexa::cpu::emulator::visitCallInst(llvm::CallInst& I)
{
    llvm::Function* callee = I.getCalledFunction();
    VEXA_ASSERT(callee);
    if (callee->isIntrinsic()) // if the callee is an llvm intrinsic
    {
        auto id = callee->getIntrinsicID();
        switch (id) {
        case llvm::Intrinsic::ctpop:
        {
            vexa::value* src = symex->get(I.getOperand(0));
            uint8_t src_bit_width = src->size();
            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width);

            // extract the first bit and zero extend to target size
            z3::expr total = z3::zext(src->as_expr().extract(0, 0), dst_bit_width - 1);
            // then extract other bits and add to the total
            for (size_t i = 1; i < src_bit_width; i++)
            {
                z3::expr bit = z3::zext(src->as_expr().extract(i, i), dst_bit_width - 1);
                total = total + bit;
            }
            return symex->value(total);
        }
        case llvm::Intrinsic::fshl:
        {
            vexa::value* src_x = symex->get(I.getOperand(0));
            vexa::value* src_y = symex->get(I.getOperand(1));
            vexa::value* shift = symex->get(I.getOperand(2));

            uint8_t bit_width = src_x->size();
            z3::expr x = src_x->as_expr();
            z3::expr y = src_y->as_expr();
            z3::expr s = shift->as_expr();

            z3::expr s_mod = z3::urem(s, bit_width);  // normalize shift count, s_mod = s % bit_width
            z3::expr s_wide = z3::zext(s_mod, bit_width); // extend the shift count to the bit_width
            z3::expr wide = z3::concat(x, y); // combine x and y
            z3::expr shifted = z3::shl(wide, s_wide); // shift left

            // extract the bits from left
            z3::expr result = shifted.extract(2 * bit_width - 1, bit_width);
            // check if the result value's size matches with the expected in llvm
            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());

            return symex->value(result);
        }
        case llvm::Intrinsic::fshr:
        {
            vexa::value* src_x = symex->get(I.getOperand(0));
            vexa::value* src_y = symex->get(I.getOperand(1));
            vexa::value* shift = symex->get(I.getOperand(2));

            uint8_t bit_width = src_x->size();
            z3::expr x = src_x->as_expr();
            z3::expr y = src_y->as_expr();
            z3::expr s = shift->as_expr();

            z3::expr s_mod = z3::urem(s, bit_width); // normalize shift count, s_mod = s % bit_width
            z3::expr s_wide = z3::zext(s_mod, bit_width);// extend the shift count to the bit_width
            z3::expr wide = z3::concat(x, y); // combine x and y
            z3::expr shifted = z3::lshr(wide, s_wide); // shift right

            // extract the bits from right
            z3::expr result = shifted.extract(bit_width - 1, 0);
            // check if the result value's size matches with the expected in llvm
            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());

            return symex->value(result);
        }
        case llvm::Intrinsic::bswap:
        {
            vexa::value* src = symex->get(I.getOperand(0));
            uint32_t bit_width = src->size();
            z3::expr src_expr = src->as_expr();
            VEXA_ASSERT(bit_width % 8 == 0 && "bswap bit width must be a multiple of 8");

            z3::expr result = src_expr.extract(7, 0);
            for (size_t i = 8; i < bit_width; i += 8)
            {
                z3::expr next_byte = src_expr.extract(i + 7, i);
                result = z3::concat(result, next_byte);
            }

            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::cttz:
        {
            vexa::value* src = symex->get(I.getOperand(0));
            uint32_t bit_width = src->size();
            z3::expr src_expr = src->as_expr();

            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width);
            z3::expr result = context->z3_context->bv_val(bit_width, dst_bit_width);
            z3::expr bit_one = context->z3_context->bv_val(1, 1);

            for (int i = bit_width - 1; i >= 0; i--)
            {
                z3::expr current_bit = src_expr.extract(i, i);
                z3::expr current_idx = context->z3_context->bv_val(i, dst_bit_width);
                result = z3::ite(current_bit == bit_one, current_idx, result);
            }

            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::ctlz:
        {
            vexa::value* src = symex->get(I.getOperand(0));
            uint32_t bit_width = src->size();
            z3::expr src_expr = src->as_expr();

            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width);

            z3::expr result = context->z3_context->bv_val(bit_width, dst_bit_width);
            z3::expr bit_one = context->z3_context->bv_val(1, 1);

            for (uint32_t i = 0; i < bit_width; i++)
            {
                z3::expr current_bit = src_expr.extract(i, i);
                uint32_t leading_zeros = bit_width - 1 - i;
                z3::expr current_idx = context->z3_context->bv_val(leading_zeros, dst_bit_width);
                result = z3::ite(current_bit == bit_one, current_idx, result);
            }

            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::umax:
        {
            vexa::value* src0 = symex->get(I.getOperand(0));
            vexa::value* src1 = symex->get(I.getOperand(1));

            uint8_t bit_width = src0->size();
            VEXA_ASSERT(src1->size() == bit_width);

            z3::expr x = src0->as_expr();
            z3::expr y = src1->as_expr();
            z3::expr result = z3::ite(z3::uge(x, y), x, y);

            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width == bit_width);
            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::usub_sat:
        {
            vexa::value* src0 = symex->get(I.getOperand(0));
            vexa::value* src1 = symex->get(I.getOperand(1));

            uint8_t bit_width = src0->size();
            z3::expr x = src0->as_expr();
            z3::expr y = src1->as_expr();

            uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(dst_bit_width == bit_width);

            z3::expr zero = context->z3_context->bv_val(0, dst_bit_width);
            z3::expr sub = x - y;
            z3::expr result = z3::ite(z3::ult(x, y), zero, sub);

            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::abs:
        {
            vexa::value* src = symex->get(I.getOperand(0));

            uint32_t bit_width = src->size();
            uint32_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
            VEXA_ASSERT(bit_width > 0);
            VEXA_ASSERT(dst_bit_width == bit_width);

            z3::expr x = src->as_expr();
            z3::expr zero = context->z3_context->bv_val(0, bit_width);
            z3::expr result = z3::ite(z3::slt(x, zero), zero - x, x);

            VEXA_ASSERT(dst_bit_width == result.get_sort().bv_size());
            return symex->value(result);
        }
        case llvm::Intrinsic::lifetime_start:
        case llvm::Intrinsic::lifetime_end:
            break;
        default:
            THROW("CallInst: unimplemented llvm intrinsic -> {}", callee->getName().str());
        }
    }
    else // means its a remill intrinsic
    {
        // save insert point
        builder->push_ip();

        // memory writes
        if (callee == cpu->intrinsics->write_memory_64)
            write_memory_intrinsic(I, 64);
        else if (callee == cpu->intrinsics->write_memory_32)
            write_memory_intrinsic(I, 32);
        else if (callee == cpu->intrinsics->write_memory_16)
            write_memory_intrinsic(I, 16);
        else if (callee == cpu->intrinsics->write_memory_8)
            write_memory_intrinsic(I, 8);

        // memory reads
        else if (callee == cpu->intrinsics->read_memory_64)
            read_memory_intrinsic(I, 64);
        else if (callee == cpu->intrinsics->read_memory_32)
            read_memory_intrinsic(I, 32);
        else if (callee == cpu->intrinsics->read_memory_16)
            read_memory_intrinsic(I, 16);
        else if (callee == cpu->intrinsics->read_memory_8)
            read_memory_intrinsic(I, 8);

        else if(callee == cpu->intrinsics->atomic_begin || callee == cpu->intrinsics->atomic_end
                || callee == cpu->intrinsics->barrier_load_load || callee == cpu->intrinsics->barrier_load_store
                || callee == cpu->intrinsics->barrier_store_load || callee == cpu->intrinsics->barrier_store_store)
            I.replaceAllUsesWith(I.getArgOperand(0));

        else
            THROW("CallInst: unknown remill intrinsic -> {}", callee->getName().str());

        builder->deleteLater(&I);
        // restore insert point
        builder->pop_ip();
    }

    return symex->concrete(0, 64);
}

vexa::dual_value vexa::cpu::emulator::run(llvm::Instruction* I)
{
    vexa::value* res = visit(I);
    symex->set(I, res);
    return vexa::dual_value(I, res);
}

vexa::value* vexa::cpu::emulator::visitInstruction(llvm::Instruction& I)
{
    THROW("unsupported instruction -> {}", std::string(I.getOpcodeName()));
}

vexa::value* vexa::cpu::emulator::visitFreezeInst(llvm::FreezeInst& I)
{
    return VEXA_SYM_VAL(I.getOperand(0));
}

vexa::value* vexa::cpu::emulator::visitExtractElementInst(llvm::ExtractElementInst& I)
{
    auto* vector_type = llvm::dyn_cast<llvm::FixedVectorType>(I.getVectorOperandType());
    VEXA_ASSERT(vector_type);

    vexa::value* vector = VEXA_SYM_VAL(I.getVectorOperand());
    vexa::value* index = VEXA_SYM_VAL(I.getIndexOperand());
    uint64_t element_bit_width = I.getType()->getPrimitiveSizeInBits();
    uint64_t element_count = vector_type->getNumElements();

    VEXA_ASSERT(element_bit_width);
    VEXA_ASSERT(vector->size() == element_bit_width * element_count);

    z3::expr vector_expr = vector->as_expr();
    if (index->is_concrete())
    {
        uint64_t element_index = index->as_uint64();
        VEXA_ASSERT(element_index < element_count);

        uint64_t low_bit = element_index * element_bit_width;
        return symex->value(vector_expr.extract(low_bit + element_bit_width - 1, low_bit));
    }

    std::string poison_name = "extractelement_poison_" +
                              std::to_string(reinterpret_cast<uintptr_t>(&I));

    z3::expr result = context->z3_context->bv_const(poison_name.c_str(), element_bit_width);
    z3::expr index_expr = index->as_expr();
    
    uint64_t representable_elements =
        index->size() < 64 ? (uint64_t{1} << index->size()) : element_count;

    for (uint64_t element_index = 0;
         element_index < element_count && element_index < representable_elements;
         ++element_index)
    {
        uint64_t low_bit = element_index * element_bit_width;
        z3::expr element = vector_expr.extract(low_bit + element_bit_width - 1, low_bit);
        z3::expr expected_index =
            context->z3_context->bv_val(element_index, index->size());
        result = z3::ite(index_expr == expected_index, element, result);
    }

    return symex->value(result);
}

vexa::value* vexa::cpu::emulator::visitSelectInst(llvm::SelectInst& I)
{
    vexa::value* cond = VEXA_SYM_VAL(I.getCondition());
    vexa::value* lhs = VEXA_SYM_VAL(I.getTrueValue());
    vexa::value* rhs = VEXA_SYM_VAL(I.getFalseValue());

    z3::expr cond_bool = cond->as_expr() == cond->as_expr().ctx().bv_val(1, 1);
    vexa::value* expr = symex->value(z3::ite(cond_bool, lhs->as_expr(), rhs->as_expr()));
    return expr;
}

vexa::value* vexa::cpu::emulator::visitAllocaInst(llvm::AllocaInst& I)
{
    return symex->pointer(symex->concrete(0, 64),
                          memory->allocate(I.getAllocationSize(DL)->getFixedValue()));
}

vexa::value* vexa::cpu::emulator::visitGetElementPtrInst(llvm::GetElementPtrInst &I)
{
    llvm::APInt offset(64, 0);
    if (I.accumulateConstantOffset(DL, offset))
    {
        // calculate pointerOperand value + constant offset
        vexa::value* _offset = symex->concrete(offset.getZExtValue(), 64);
        vexa::pointer* ptr = vexa::dyn_cast<vexa::pointer>(symex->get(I.getPointerOperand()));
        vexa::value* addr = symex->value(ptr->as_expr() + _offset->as_expr());
        return symex->pointer(addr, ptr->get_page());
    }
    THROW("getelementptr, this shouldn't happen... please report");
}

vexa::value* vexa::cpu::emulator::visitStoreInst(llvm::StoreInst& I)
{
    vexa::pointer* ptr = VEXA_SYM_PTR(I.getPointerOperand());
    vexa::value* val = VEXA_SYM_VAL(I.getValueOperand());
    memory->write(ptr, val);
    return symex->concrete(0, 64);
}

vexa::value* vexa::cpu::emulator::visitLoadInst(llvm::LoadInst& I)
{
    vexa::value* ptr_val = symex->get(I.getPointerOperand());
    if (vexa::pointer* ptr = vexa::dyn_cast<vexa::pointer>(ptr_val))
    {
        // read from memory
        vexa::value* read = memory->read(ptr, DL.getTypeStoreSizeInBits(I.getType()));
        return read;
    }

    THROW("LoadInst: pointer operand is not a vexa::pointer");
    return symex->concrete(1, 64);
}

vexa::value* vexa::cpu::emulator::visitBinaryOperator(llvm::BinaryOperator& I)
{
    z3::expr lhs = VEXA_SYM_VAL(I.getOperand(0))->as_expr();
    z3::expr rhs = VEXA_SYM_VAL(I.getOperand(1))->as_expr();

    std::optional<z3::expr> expr;
    switch (I.getOpcode()) {
    case llvm::Instruction::Add:
        expr = lhs + rhs;
        break;
    case llvm::Instruction::Sub:
        expr = lhs - rhs;
        break;
    case llvm::Instruction::Mul:
        expr = lhs * rhs;
        break;
    case llvm::Instruction::UDiv:
        expr = z3::udiv(lhs, rhs);
        break;
    case llvm::Instruction::URem:
        expr = z3::urem(lhs, rhs);
        break;
    case llvm::Instruction::SDiv:
        expr = z3::sdiv(lhs, rhs);
        break;
    case llvm::Instruction::SRem:
        expr = z3::srem(lhs, rhs);
        break;
    case llvm::Instruction::And:
        expr = lhs & rhs;
        break;
    case llvm::Instruction::Or:
        expr = lhs | rhs;
        break;
    case llvm::Instruction::Xor:
        expr = lhs ^ rhs;
        break;
    case llvm::Instruction::Shl:
        expr = z3::shl(lhs, rhs);
        break;
    case llvm::Instruction::LShr:
        expr = z3::lshr(lhs, rhs);
        break;
    case llvm::Instruction::AShr:
        expr = z3::ashr(lhs, rhs);
        break;
    default:
        THROW("unsupported binary operator -> {}", std::string(I.getOpcodeName()));
    }

    return symex->value(*expr);
}

vexa::value* vexa::cpu::emulator::visitICmpInst(llvm::ICmpInst& I)
{
    z3::expr lhs = VEXA_SYM_VAL(I.getOperand(0))->as_expr();
    z3::expr rhs = VEXA_SYM_VAL(I.getOperand(1))->as_expr();

    std::optional<z3::expr> expr;
    switch (I.getCmpPredicate()) {
    case llvm::ICmpInst::ICMP_EQ:
        expr = lhs == rhs;
        break;
    case llvm::ICmpInst::ICMP_NE:
        expr = lhs != rhs;
        break;
    case llvm::ICmpInst::ICMP_UGT:
        expr = z3::ugt(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_UGE:
        expr = z3::uge(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_ULT:
        expr = z3::ult(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_ULE:
        expr = z3::ule(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_SGT:
        expr = z3::sgt(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_SGE:
        expr = z3::sge(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_SLT:
        expr = z3::slt(lhs, rhs);
        break;
    case llvm::ICmpInst::ICMP_SLE:
        expr = z3::sle(lhs, rhs);
        break;
    default:
        THROW("Unsupported icmp predicate -> {}", std::string(I.getPredicateName(I.getPredicate())));
    }

    expr = z3::ite(*expr, context->z3_context->bv_val(1, 1),  context->z3_context->bv_val(0, 1));
    return symex->value(*expr);
}

vexa::value* vexa::cpu::emulator::visitZExtInst(llvm::ZExtInst& I)
{
    vexa::value* src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::value* extended = symex->value(z3::zext(src->as_expr(), I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
    return extended;
}

vexa::value* vexa::cpu::emulator::visitSExtInst(llvm::SExtInst& I)
{
    vexa::value* src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::value* extended = symex->value(z3::sext(src->as_expr(), I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
    return extended;
}

vexa::value* vexa::cpu::emulator::visitTruncInst(llvm::TruncInst& I)
{
    vexa::value* src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::value* truncated = symex->value(src->as_expr().extract(I.getDestTy()->getPrimitiveSizeInBits() - 1, 0));
    return truncated;
}