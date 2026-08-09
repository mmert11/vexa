#include <vexa/vexa.h>

#include <llvm/Transforms/Utils/Cloning.h>

#include <remill/Arch/Runtime/HyperCall.h>

void vexa::cpu::emulator::run_block(llvm::BasicBlock *BB)
{
    for (auto &I : llvm::make_early_inc_range(*BB)) {
        if (I.isTerminator())
            continue;

        llvm::Value *I_ptr = &I;
        vexa::value *res = visit(I);
        symex->set(I_ptr, res);

        if (forked_block == BB) {
            forked_block = nullptr;
            return;
        }
    }
}
void vexa::cpu::emulator::write_memory(vexa::pointer *addr, vexa::value *val)
{
    addr->simplify();
    if (addr->is_concrete()) {
        memory->write(addr, val);
        return;
    }

    // symbolic write
    std::vector<bw::Term> possible_values = addr->possible_values(cpu->path_constraints);
    auto resolve = [&](vexa::value *address) {
        vexa::pointer *resolved = vexa::to_ptr(cpu->calculate_pointer(address));
        VEXA_ASSERT(resolved);
        return resolved;
    };

    // address has resolved to a single concrete value
    //
    if (possible_values.size() == 1) {
        memory->write(resolve(symex->value(possible_values.front())), val);
        return;
    }

    // address has multiple possible values
    // create ite for each possible address and write on it
    //
    int byte_size = val->size() / 8;
    for (const bw::Term &address : possible_values) {
        vexa::value *solved = symex->value(address);
        vexa::pointer *resolved = resolve(solved);
        auto page = resolved->get_page();
        auto &contents = page->concrete_memory;
        uint64_t base = resolved->as_uint64();
        bw::Term condition = *addr == *solved;

        for (int i = 0; i < byte_size; i++) {
            uint64_t byte_addr = base + i;
            bw::Term new_byte = val->extract(i * 8 + 7, i * 8);
            bw::Term old_byte = context->term_manager.mk_const(
                context->term_manager.mk_bv_sort(8),
                "noinit_" + std::to_string(reinterpret_cast<uintptr_t>(page.get())) + "_"
                    + std::to_string(byte_addr));
            auto it = contents.find(byte_addr);
            if (it != contents.end() && it->second)
                old_byte = *it->second;

            contents[byte_addr] = context->bitwuzla->simplify(
                context->term_manager.mk_term(bw::Kind::ITE, {condition, new_byte, old_byte}));
        }
    }
}

vexa::value *vexa::cpu::emulator::read_memory(vexa::pointer *addr, int size)
{
    addr->simplify();
    if (addr->is_concrete())
        return memory->read(addr, size);

    // symbolic address read
    std::vector<bw::Term> possible_values = addr->possible_values(cpu->path_constraints);
    auto resolve = [&](vexa::value *address) {
        vexa::pointer *resolved = vexa::to_ptr(cpu->calculate_pointer(address));
        VEXA_ASSERT(resolved);
        return resolved;
    };

    // address has resolved to a single concrete value
    //
    if (possible_values.size() == 1)
        return memory->read(resolve(symex->value(possible_values.front())), size);

    // address has multiple possible values
    // combine every possibility in an array of ite
    //
    bw::Term value = symex->concrete(0, size)->as_expr();
    for (const bw::Term &address : possible_values) {
        vexa::value *solved = symex->value(address);
        vexa::value *read = memory->read(resolve(solved), size);
        value = context->term_manager.mk_term(
            bw::Kind::ITE, {*addr == *solved, read->as_expr(), value});
    }

    return symex->value(value);
}

llvm::BasicBlock *vexa::cpu::emulator::fork_memory_access(
    llvm::CallInst &call,
    vexa::value *address,
    size_t size,
    bool write,
    const std::vector<bw::Term> &values)
{
    VEXA_ASSERT(values.size() > 1);
    llvm::BasicBlock *source = call.getParent();
    llvm::Instruction *continuation = call.getNextNode();
    VEXA_ASSERT(source && continuation);

    // insert temporary terminator at the end of the basic block
    //
    bool temporary_terminator = source->getTerminator() == nullptr;
    if (temporary_terminator) {
        builder->SetInsertPoint(source);
        builder->CreateUnreachable();
    }

    // split the basic block right after remill's intrinsic call
    //
    llvm::BasicBlock *suffix =
        source->splitBasicBlock(continuation, source->getName() + (write ? ".write" : ".read"));
    // erase the terminators
    //
    source->getTerminator()->eraseFromParent();
    if (temporary_terminator)
        suffix->getTerminator()->eraseFromParent();
    //

    // clone the split basic block for every possible address
    //
    std::vector<llvm::BasicBlock *> possible_blocks(values.size());
    possible_blocks.front() = suffix;
    for (size_t i = 1; i < values.size(); i++) {
        llvm::ValueToValueMapTy map;
        possible_blocks[i] = llvm::CloneBasicBlock(
            suffix,
            map,
            std::format(".{}_{}", write ? "write" : "read", vexa::value::as_uint64(values[i])),
            cpu->vexa_lifted);
        // remap operands
        //
        for (llvm::Instruction &instruction : *possible_blocks[i])
            llvm::RemapInstruction(
                &instruction, map, llvm::RF_NoModuleLevelChanges | llvm::RF_IgnoreMissingLocals);
    }
    if (write)
        call.replaceAllUsesWith(call.getArgOperand(0));

    // create switch case
    //
    auto *address_type = llvm::dyn_cast<llvm::IntegerType>(call.getArgOperand(1)->getType());
    VEXA_ASSERT(address_type);
    llvm::BasicBlock *default_block = builder->basic_block();
    builder->SetInsertPoint(default_block);
    builder->CreateUnreachable();
    builder->SetInsertPoint(source);
    llvm::SwitchInst *dispatch =
        builder->CreateSwitch(call.getArgOperand(1), default_block, values.size());

    //mem_state base_memory = memory->take_snapshot();
    //symex_state base_symex = symex->take_snapshot();

    auto base_snapshot = cpu->take_snapshot(cpu->instruction.next_pc, cpu->block);
    std::vector<bw::Term> base_constraints = cpu->path_constraints;

    auto execute = [&](size_t index) {
        //memory->restore_snapshot(base_memory);
        //symex->restore_snapshot(base_symex);

        cpu->restore_snapshot(base_snapshot);
        cpu->path_constraints = base_constraints;

        const bw::Term &candidate = values[index];
        uint64_t concrete_address = vexa::value::as_uint64(candidate);
        llvm::BasicBlock *case_block = builder->basic_block(
            std::format("{}_{:x}", write ? "write" : "read", concrete_address));
        dispatch->addCase(llvm::ConstantInt::get(address_type, concrete_address), case_block);

        cpu->path_constraints.push_back(
            context->term_manager.mk_term(bw::Kind::EQUAL, {address->as_expr(), candidate}));
        symex->specialize(address->as_expr(), candidate);

        vexa::value *solved = symex->value(candidate);
        vexa::dual_pointer page = cpu->get_page(solved);
        builder->SetInsertPoint(case_block);
        vexa::dual_pointer resolved =
            builder
                ->inbounds_gep(
                    builder->getInt8Ty(),
                    page.l,
                    llvm::ConstantInt::get(address_type, concrete_address))
                .to_ptr();
        
        // memory write
        if (write)
            run(builder->CreateStore(call.getOperand(2), resolved.l));
        // memory read
        else {
            auto *load = builder->CreateLoad(builder->getIntNTy(size), resolved.l);
            symex->set(load, memory->read(resolved.v, size));
            for (llvm::Instruction &instruction : *possible_blocks[index])
                instruction.replaceUsesOfWith(&call, load);
        }

        builder->CreateBr(possible_blocks[index]);
        builder->SetInsertPoint(possible_blocks[index]);
        run_block(possible_blocks[index]);
        builder->SetInsertPoint(possible_blocks[index]);
    };

    // execute the every address case except for the first one
    //
    for (size_t i = values.size(); i-- > 1;) {
        execute(i);
        cpu->unexplored_paths.push(
            cpu->take_snapshot(cpu->instruction.next_pc, builder->GetInsertBlock()));
    }

    execute(0);
    forked_block = source;
    LOG_INFO(logger, "Path fork by symbolic memory {}", write ? "write" : "read");
    context->event_handler(vexa::event_kind::PATH_FORKING);
    return builder->GetInsertBlock();
}

llvm::BasicBlock *vexa::cpu::emulator::write_memory_intrinsic(llvm::CallInst &call, size_t size)
{
    // get pointer operand expression
    llvm::Value *address = call.getOperand(1);
    vexa::dual_value address_sym = VEXA_VAL(address);
    address_sym.v->simplify();

    if (address_sym.v->is_symbolic() && !cpu->instruction.IsControlFlow()) {
        bw::Result result;
        std::vector<bw::Term> values =
            address_sym.v->possible_values(cpu->path_constraints, &result);
        if (values.size() > 1 && result == bw::Result::UNSAT)
            return fork_memory_access(call, address_sym.v, size, true, values);
    }

    builder->SetInsertPoint(call.getNextNode());
    vexa::dual_value ptr = cpu->value_to_pointer(address);
    vexa::pointer *ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // get value operand expression
    llvm::Value *val = call.getOperand(2);
    vexa::dual_value val_sym = VEXA_VAL(val);
    VEXA_ASSERT(val_sym.v->size() == val->getType()->getPrimitiveSizeInBits());

    // create actual store
    run(builder->CreateStore(val, ptr.l));
    call.replaceAllUsesWith(call.getArgOperand(0));
    return nullptr;
}

llvm::BasicBlock *vexa::cpu::emulator::read_memory_intrinsic(llvm::CallInst &call, size_t size)
{
    // get pointer operand expression
    llvm::Value *val = call.getOperand(1);
    vexa::dual_value val_sym = VEXA_VAL(val);
    val_sym.v->simplify();

    if (val_sym.v->is_symbolic() && !cpu->instruction.IsControlFlow()) {
        bw::Result result;
        std::vector<bw::Term> values = val_sym.v->possible_values(cpu->path_constraints, &result);
        if (values.size() > 1 && result == bw::Result::UNSAT)
            return fork_memory_access(call, val_sym.v, size, false, values);
    }

    builder->SetInsertPoint(call.getNextNode());
    vexa::dual_value ptr = cpu->value_to_pointer(val);
    vexa::pointer *ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // create actual load
    auto *load = builder->CreateLoad(builder->getIntNTy(size), ptr.l);
    symex->set(load, read_memory(ptr_sym, size));
    call.replaceAllUsesWith(load);
    return nullptr;
}

vexa::value *vexa::cpu::emulator::visitCallInst(llvm::CallInst &I)
{
    llvm::Function *callee = I.getCalledFunction();
    VEXA_ASSERT(callee);
    if (callee->isIntrinsic()) // if the callee is an llvm intrinsic
    {
        auto id = callee->getIntrinsicID();
        return handle_llvm_intrinsics(I, callee, id);
    }
    else // means its a remill intrinsic
    {
        // save insert point
        builder->push_ip();
        llvm::BasicBlock *resume = nullptr;

        // memory writes
        if (callee == cpu->intrinsics->write_memory_64)
            resume = write_memory_intrinsic(I, 64);
        else if (callee == cpu->intrinsics->write_memory_32)
            resume = write_memory_intrinsic(I, 32);
        else if (callee == cpu->intrinsics->write_memory_16)
            resume = write_memory_intrinsic(I, 16);
        else if (callee == cpu->intrinsics->write_memory_8)
            resume = write_memory_intrinsic(I, 8);

        // memory reads
        else if (callee == cpu->intrinsics->read_memory_64)
            resume = read_memory_intrinsic(I, 64);
        else if (callee == cpu->intrinsics->read_memory_32)
            resume = read_memory_intrinsic(I, 32);
        else if (callee == cpu->intrinsics->read_memory_16)
            resume = read_memory_intrinsic(I, 16);
        else if (callee == cpu->intrinsics->read_memory_8)
            resume = read_memory_intrinsic(I, 8);

        else if (callee == cpu->intrinsics->sync_hyper_call) {
            if (auto ID = llvm::dyn_cast<llvm::ConstantInt>(I.getArgOperand(2))) {
                switch (ID->getZExtValue()) {
                case SyncHyperCall::kX86CPUID: {
                    cpu->write_register(amd64::RAX, symex->concrete(0x00A50F00, 64));
                    cpu->write_register(amd64::RBX, symex->concrete(0x000C0800, 64));
                    cpu->write_register(amd64::RCX, symex->concrete(0x7EF8320B, 64));
                    cpu->write_register(amd64::RDX, symex->concrete(0x178BFBFF, 64));
                    break;
                }
                default:
                    THROW("Unknown SyncHyperCall");
                }
            }
            else {
                THROW("SyncHyperCall ID error");
            }
        }

        else if (
            callee == cpu->intrinsics->atomic_begin || callee == cpu->intrinsics->atomic_end
            || callee == cpu->intrinsics->barrier_load_load
            || callee == cpu->intrinsics->barrier_load_store
            || callee == cpu->intrinsics->barrier_store_load
            || callee == cpu->intrinsics->barrier_store_store)
            I.replaceAllUsesWith(I.getArgOperand(0));

        else
            THROW("CallInst: unknown remill intrinsic -> {}", callee->getName().str());

        builder->deleteLater(&I);
        // restore insert point
        builder->pop_ip();
        if (resume)
            builder->SetInsertPoint(resume);
    }

    return symex->concrete(0, 64);
}

vexa::dual_value vexa::cpu::emulator::run(llvm::Instruction *I)
{
    vexa::value *res = visit(I);
    symex->set(I, res);
    return vexa::dual_value(I, res);
}

vexa::value *vexa::cpu::emulator::visitInstruction(llvm::Instruction &I)
{
    THROW("unsupported instruction -> {}", std::string(I.getOpcodeName()));
}

vexa::value *vexa::cpu::emulator::visitFreezeInst(llvm::FreezeInst &I)
{
    return VEXA_VAL(I.getOperand(0)).v;
}

vexa::value *vexa::cpu::emulator::visitExtractElementInst(llvm::ExtractElementInst &I)
{
    auto *vector_type = llvm::dyn_cast<llvm::FixedVectorType>(I.getVectorOperandType());
    VEXA_ASSERT(vector_type);

    vexa::value *vector = VEXA_VAL(I.getVectorOperand()).v;
    vexa::value *index = VEXA_VAL(I.getIndexOperand()).v;
    uint64_t element_bit_width = I.getType()->getPrimitiveSizeInBits();
    uint64_t element_count = vector_type->getNumElements();

    VEXA_ASSERT(element_bit_width);
    VEXA_ASSERT(vector->size() == element_bit_width * element_count);

    if (index->is_concrete()) {
        uint64_t element_index = index->as_uint64();
        VEXA_ASSERT(element_index < element_count);

        uint64_t low_bit = element_index * element_bit_width;
        return symex->value(vector->extract(low_bit + element_bit_width - 1, low_bit));
    }

    std::string poison_name =
        "extractelement_poison_" + std::to_string(reinterpret_cast<uintptr_t>(&I));

    bw::Term result = context->term_manager.mk_const(
        context->term_manager.mk_bv_sort(element_bit_width), poison_name);

    uint64_t representable_elements =
        index->size() < 64 ? (uint64_t{1} << index->size()) : element_count;

    for (uint64_t element_index = 0;
         element_index < element_count && element_index < representable_elements;
         ++element_index)
    {
        uint64_t low_bit = element_index * element_bit_width;
        bw::Term element = vector->extract(low_bit + element_bit_width - 1, low_bit);
        vexa::value *expected_index = symex->concrete(element_index, index->size());
        result = context->term_manager.mk_term(
            bw::Kind::ITE, {*index == *expected_index, element, result});
    }

    return symex->value(result);
}

vexa::value *vexa::cpu::emulator::visitSelectInst(llvm::SelectInst &I)
{
    vexa::value *cond = VEXA_VAL(I.getCondition()).v;
    vexa::value *lhs = VEXA_VAL(I.getTrueValue()).v;
    vexa::value *rhs = VEXA_VAL(I.getFalseValue()).v;

    return symex->value(cond->ite(*lhs, *rhs));
}

vexa::value *vexa::cpu::emulator::visitAllocaInst(llvm::AllocaInst &I)
{
    return symex->pointer(
        symex->concrete(0, 64), memory->allocate(I.getAllocationSize(DL)->getFixedValue()));
}

vexa::value *vexa::cpu::emulator::visitGetElementPtrInst(llvm::GetElementPtrInst &I)
{
    llvm::APInt offset(64, 0);
    if (I.accumulateConstantOffset(DL, offset)) {
        // calculate pointerOperand value + constant offset
        vexa::value *_offset = symex->concrete(offset.getZExtValue(), 64);
        vexa::pointer *ptr = vexa::dyn_cast<vexa::pointer>(symex->get(I.getPointerOperand()));
        vexa::value *addr = symex->value(*ptr + *_offset);
        return symex->pointer(addr, ptr->get_page());
    }
    THROW("getelementptr, this shouldn't happen...");
}

vexa::value *vexa::cpu::emulator::visitStoreInst(llvm::StoreInst &I)
{
    vexa::pointer *ptr = VEXA_SYM_PTR(I.getPointerOperand());
    vexa::value *val = VEXA_VAL(I.getValueOperand()).v;
    write_memory(ptr, val);
    return symex->concrete(0, 64);
}

vexa::value *vexa::cpu::emulator::visitLoadInst(llvm::LoadInst &I)
{
    vexa::value *ptr_val = VEXA_VAL(I.getPointerOperand()).v;
    if (vexa::pointer *ptr = vexa::dyn_cast<vexa::pointer>(ptr_val->simplify())) {
        llvm::TypeSize read_size = DL.getTypeStoreSizeInBits(I.getType());
        vexa::value *read = read_memory(ptr, read_size);
        return read;
    }

    THROW("LoadInst: pointer operand is not a vexa::pointer");
    return symex->concrete(1, 64);
}

vexa::value *vexa::cpu::emulator::visitBinaryOperator(llvm::BinaryOperator &I)
{
    vexa::value *lhs = VEXA_VAL(I.getOperand(0)).v;
    vexa::value *rhs = VEXA_VAL(I.getOperand(1)).v;

    std::optional<bw::Term> expr;
    switch (I.getOpcode()) {
    case llvm::Instruction::Add:
        expr = *lhs + *rhs;
        break;
    case llvm::Instruction::Sub:
        expr = *lhs - *rhs;
        break;
    case llvm::Instruction::Mul:
        expr = *lhs * *rhs;
        break;
    case llvm::Instruction::UDiv:
        expr = lhs->udiv(*rhs);
        break;
    case llvm::Instruction::URem:
        expr = lhs->urem(*rhs);
        break;
    case llvm::Instruction::SDiv:
        expr = *lhs / *rhs;
        break;
    case llvm::Instruction::SRem:
        expr = *lhs % *rhs;
        break;
    case llvm::Instruction::And:
        expr = *lhs & *rhs;
        break;
    case llvm::Instruction::Or:
        expr = *lhs | *rhs;
        break;
    case llvm::Instruction::Xor:
        expr = *lhs ^ *rhs;
        break;
    case llvm::Instruction::Shl:
        expr = *lhs << *rhs;
        break;
    case llvm::Instruction::LShr:
        expr = lhs->lshr(*rhs);
        break;
    case llvm::Instruction::AShr:
        expr = *lhs >> *rhs;
        break;
    default:
        THROW("unsupported binary operator -> {}", std::string(I.getOpcodeName()));
    }

    return symex->value(std::move(*expr));
}

vexa::value *vexa::cpu::emulator::visitICmpInst(llvm::ICmpInst &I)
{
    vexa::value *lhs = VEXA_VAL(I.getOperand(0)).v;
    vexa::value *rhs = VEXA_VAL(I.getOperand(1)).v;

    std::optional<bw::Term> expr;
    switch (I.getCmpPredicate()) {
    case llvm::ICmpInst::ICMP_EQ:
        expr = *lhs == *rhs;
        break;
    case llvm::ICmpInst::ICMP_NE:
        expr = *lhs != *rhs;
        break;
    case llvm::ICmpInst::ICMP_UGT:
        expr = lhs->ugt(*rhs);
        break;
    case llvm::ICmpInst::ICMP_UGE:
        expr = lhs->uge(*rhs);
        break;
    case llvm::ICmpInst::ICMP_ULT:
        expr = lhs->ult(*rhs);
        break;
    case llvm::ICmpInst::ICMP_ULE:
        expr = lhs->ule(*rhs);
        break;
    case llvm::ICmpInst::ICMP_SGT:
        expr = *lhs > *rhs;
        break;
    case llvm::ICmpInst::ICMP_SGE:
        expr = *lhs >= *rhs;
        break;
    case llvm::ICmpInst::ICMP_SLT:
        expr = *lhs < *rhs;
        break;
    case llvm::ICmpInst::ICMP_SLE:
        expr = *lhs <= *rhs;
        break;
    default:
        THROW(
            "Unsupported icmp predicate -> {}", std::string(I.getPredicateName(I.getPredicate())));
    }

    bw::Sort bool_sort = context->term_manager.mk_bv_sort(1);
    return symex->value(context->term_manager.mk_term(
        bw::Kind::ITE,
        {*expr,
         context->term_manager.mk_bv_one(bool_sort),
         context->term_manager.mk_bv_zero(bool_sort)}));
}

vexa::value *vexa::cpu::emulator::visitZExtInst(llvm::ZExtInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return symex->value(src->zext(I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
}

vexa::value *vexa::cpu::emulator::visitSExtInst(llvm::SExtInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return symex->value(src->sext(I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
}

vexa::value *vexa::cpu::emulator::visitTruncInst(llvm::TruncInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return symex->value(src->extract(I.getDestTy()->getPrimitiveSizeInBits() - 1, 0));
}

vexa::value *vexa::cpu::emulator::handle_llvm_intrinsics(
    llvm::CallInst &I,
    llvm::Function *callee,
    llvm::Intrinsic::ID id)
{
    bw::TermManager &tm = context->term_manager;
    switch (id) {
    case llvm::Intrinsic::ctpop: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint8_t src_bit_width = src->size();
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);

        // extract the first bit and zero extend to target size
        bw::Term total = tm.mk_term(
            bw::Kind::BV_ZERO_EXTEND,
            {src->extract(0, 0)},
            {static_cast<uint64_t>(dst_bit_width - 1)});
        // then extract other bits and add to the total
        for (size_t i = 1; i < src_bit_width; i++) {
            bw::Term bit = tm.mk_term(
                bw::Kind::BV_ZERO_EXTEND,
                {src->extract(i, i)},
                {static_cast<uint64_t>(dst_bit_width - 1)});
            total = tm.mk_term(bw::Kind::BV_ADD, {total, bit});
        }
        return symex->value(std::move(total));
    }
    case llvm::Intrinsic::fshl: {
        vexa::value *src_x = symex->get(I.getOperand(0));
        vexa::value *src_y = symex->get(I.getOperand(1));
        vexa::value *shift = symex->get(I.getOperand(2));

        uint8_t bit_width = src_x->size();
        bw::Term x = src_x->as_expr();
        bw::Term y = src_y->as_expr();
        bw::Term s = shift->as_expr();

        bw::Term s_mod =
            tm.mk_term(bw::Kind::BV_UREM, {s, tm.mk_bv_value_uint64(s.sort(), bit_width)});
        bw::Term s_wide = tm.mk_term(bw::Kind::BV_ZERO_EXTEND, {s_mod}, {bit_width});
        bw::Term wide = tm.mk_term(bw::Kind::BV_CONCAT, {x, y});
        bw::Term shifted = tm.mk_term(bw::Kind::BV_SHL, {wide, s_wide});

        // extract the bits from left
        bw::Term result = tm.mk_term(
            bw::Kind::BV_EXTRACT, {shifted}, {static_cast<uint64_t>(2 * bit_width - 1), bit_width});
        // check if the result value's size matches with the expected in llvm
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());

        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::fshr: {
        vexa::value *src_x = symex->get(I.getOperand(0));
        vexa::value *src_y = symex->get(I.getOperand(1));
        vexa::value *shift = symex->get(I.getOperand(2));

        uint8_t bit_width = src_x->size();
        bw::Term x = src_x->as_expr();
        bw::Term y = src_y->as_expr();
        bw::Term s = shift->as_expr();

        bw::Term s_mod =
            tm.mk_term(bw::Kind::BV_UREM, {s, tm.mk_bv_value_uint64(s.sort(), bit_width)});
        bw::Term s_wide = tm.mk_term(bw::Kind::BV_ZERO_EXTEND, {s_mod}, {bit_width});
        bw::Term wide = tm.mk_term(bw::Kind::BV_CONCAT, {x, y});
        bw::Term shifted = tm.mk_term(bw::Kind::BV_SHR, {wide, s_wide});

        // extract the bits from right
        bw::Term result =
            tm.mk_term(bw::Kind::BV_EXTRACT, {shifted}, {static_cast<uint64_t>(bit_width - 1), 0});
        // check if the result value's size matches with the expected in llvm
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());

        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::bswap: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint32_t bit_width = src->size();
        VEXA_ASSERT(bit_width % 8 == 0 && "bswap bit width must be a multiple of 8");

        bw::Term result = src->extract(7, 0);
        for (size_t i = 8; i < bit_width; i += 8) {
            bw::Term next_byte = src->extract(i + 7, i);
            result = tm.mk_term(bw::Kind::BV_CONCAT, {result, next_byte});
        }

        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::cttz: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint32_t bit_width = src->size();
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);
        bw::Sort dst_sort = tm.mk_bv_sort(dst_bit_width);
        bw::Term result = tm.mk_bv_value_uint64(dst_sort, bit_width);
        bw::Term bit_one = tm.mk_bv_one(tm.mk_bv_sort(1));

        for (int i = bit_width - 1; i >= 0; i--) {
            bw::Term current_bit = src->extract(i, i);
            bw::Term current_idx = tm.mk_bv_value_uint64(dst_sort, i);
            bw::Term is_one = tm.mk_term(bw::Kind::EQUAL, {current_bit, bit_one});
            result = tm.mk_term(bw::Kind::ITE, {is_one, current_idx, result});
        }

        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::ctlz: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint32_t bit_width = src->size();
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);
        bw::Sort dst_sort = tm.mk_bv_sort(dst_bit_width);
        bw::Term result = tm.mk_bv_value_uint64(dst_sort, bit_width);
        bw::Term bit_one = tm.mk_bv_one(tm.mk_bv_sort(1));

        for (uint32_t i = 0; i < bit_width; i++) {
            bw::Term current_bit = src->extract(i, i);
            uint32_t leading_zeros = bit_width - 1 - i;
            bw::Term current_idx = tm.mk_bv_value_uint64(dst_sort, leading_zeros);
            bw::Term is_one = tm.mk_term(bw::Kind::EQUAL, {current_bit, bit_one});
            result = tm.mk_term(bw::Kind::ITE, {is_one, current_idx, result});
        }

        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::umax: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));

        uint8_t bit_width = src0->size();
        VEXA_ASSERT(src1->size() == bit_width);

        bw::Term result =
            tm.mk_term(bw::Kind::ITE, {src0->uge(*src1), src0->as_expr(), src1->as_expr()});

        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width == bit_width);
        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::usub_sat: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));

        uint8_t bit_width = src0->size();
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width == bit_width);

        bw::Term zero = tm.mk_bv_zero(tm.mk_bv_sort(dst_bit_width));
        bw::Term sub = *src0 - *src1;
        bw::Term result = tm.mk_term(bw::Kind::ITE, {src0->ult(*src1), zero, sub});

        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::abs: {
        vexa::value *src = symex->get(I.getOperand(0));

        uint32_t bit_width = src->size();
        uint32_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(bit_width > 0);
        VEXA_ASSERT(dst_bit_width == bit_width);

        bw::Term x = src->as_expr();
        bw::Term zero = tm.mk_bv_zero(tm.mk_bv_sort(bit_width));
        bw::Term is_negative = tm.mk_term(bw::Kind::BV_SLT, {x, zero});
        bw::Term negated = tm.mk_term(bw::Kind::BV_SUB, {zero, x});
        bw::Term result = tm.mk_term(bw::Kind::ITE, {is_negative, negated, x});

        VEXA_ASSERT(dst_bit_width == result.sort().bv_size());
        return symex->value(std::move(result));
    }
    case llvm::Intrinsic::lifetime_start:
    case llvm::Intrinsic::lifetime_end:
        break;
    default:
        THROW("CallInst: unimplemented llvm intrinsic -> {}", callee->getName().str());
    }

    return symex->concrete(0, 64);
}
