#include <vexa/vexa.h>

#include <llvm/IR/IntrinsicsX86.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <remill/Arch/Runtime/HyperCall.h>

void vexa::cpu::emulator::run_block(llvm::BasicBlock *BB)
{
    for (auto &I : llvm::make_early_inc_range(*BB)) {
        if (I.isTerminator())
            continue;

        llvm::Instruction *I_ptr = &I;
        run(I_ptr);

        if (forked_block == BB) {
            forked_block = nullptr;
            return;
        }
    }
}

void vexa::cpu::emulator::write_memory(vexa::pointer *addr, vexa::value *val)
{
    addr->simplify();

    if (addr->is_symbolic()) {
        vexa::result result;
        std::vector<bwr::Expr> addresses = addr->possible_values(cpu->path_constraints, &result);

        if (addresses.size() == 1) {
            vexa::pointer *ptr = cpu->calculate_pointer(symex->value(addresses.front()));
            memory->write(ptr, val);
            return;
        }

        if (addresses.size() > 1 && result == vexa::result::unsat) {
            int size = val->size();

            for (size_t i = 0; i < addresses.size(); ++i) {
                vexa::value *curr_addr = symex->value(addresses[i]);
                vexa::pointer *curr_ptr = cpu->calculate_pointer(curr_addr);

                vexa::value *old_val = memory->read(curr_ptr, size);
                vexa::value *cond = (*addr == *curr_addr);
                vexa::value *updated_val = cond->ite(*val, *old_val);

                memory->write(curr_ptr, updated_val);
            }
            return;
        }
    }

    memory->write(addr, val);
}

vexa::value *vexa::cpu::emulator::read_memory(vexa::pointer *addr, int size)
{
    addr->simplify();

    if (addr->is_symbolic()) {
        vexa::result result;
        std::vector<bwr::Expr> addresses = addr->possible_values(cpu->path_constraints, &result);

        if (addresses.size() == 1) {
            vexa::pointer *ptr = cpu->calculate_pointer(symex->value(addresses.front()));
            return memory->read(ptr, size);
        }

        if (addresses.size() > 1 && result == vexa::result::unsat) {
            vexa::value *first_addr = symex->value(addresses.front());
            vexa::pointer *first_ptr = cpu->calculate_pointer(first_addr);
            vexa::value *final_sym = memory->read(first_ptr, size);

            for (size_t i = 1; i < addresses.size(); ++i) {
                vexa::value *curr_addr = symex->value(addresses[i]);
                vexa::pointer *curr_ptr = cpu->calculate_pointer(curr_addr);

                vexa::value *curr_sym = memory->read(curr_ptr, size);
                vexa::value *cond = (*addr == *curr_addr);
                final_sym = cond->ite(*curr_sym, *final_sym);
            }

            return final_sym;
        }
    }

    return memory->read(addr, size);
}

llvm::BasicBlock *vexa::cpu::emulator::fork_memory_access(
    llvm::CallInst &call,
    vexa::value *address,
    size_t size,
    bool write,
    const std::vector<bwr::Expr> &values)
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
    auto base_snapshot = cpu->take_snapshot(cpu->instruction.next_pc, cpu->block);

    auto execute = [&](size_t index) {
        // reset state to the beginning
        //
        cpu->restore_snapshot(base_snapshot);

        // initialize for the execution
        //
        const bwr::Expr &candidate = values[index];
        uint64_t concrete_address = vexa::value::as_uint64(candidate);
        llvm::BasicBlock *case_block = builder->basic_block(
            std::format("{}_{:x}", write ? "write" : "read", concrete_address));
        dispatch->addCase(llvm::ConstantInt::get(address_type, concrete_address), case_block);

        // add to path constraints
        cpu->path_constraints.push_back(
            address->as_expr().eq(candidate));
        cpu->solve_cpu_context();

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
    address_sym.v = address_sym.v->simplify(cpu->path_constraints);

    if (address_sym.v->is_symbolic() && !cpu->instruction.IsControlFlow()) {
        vexa::result result;
        std::vector<bwr::Expr> values =
            address_sym.v->possible_values(cpu->path_constraints, &result);
        if (values.size() > 1 && result == vexa::result::unsat)
            return fork_memory_access(call, address_sym.v, size, true, values);
    }

    if (address_sym.v->is_symbolic()) {
        vexa::result result;
        std::vector<bwr::Expr> values =
            address_sym.v->possible_values(cpu->path_constraints, &result);

        if (values.size() > 1 && result == vexa::result::unsat) {
            builder->SetInsertPoint(call.getNextNode());
            auto *address_type = llvm::dyn_cast<llvm::IntegerType>(address->getType());
            VEXA_ASSERT(address_type);

            llvm::Value *val = call.getOperand(2);
            vexa::dual_value val_sym = VEXA_VAL(val);
            VEXA_ASSERT(val_sym.v->size() == val->getType()->getPrimitiveSizeInBits());

            for (size_t i = 0; i < values.size(); ++i) {
                vexa::value *curr_addr = symex->value(values[i]);
                vexa::pointer *curr_ptr = cpu->calculate_pointer(curr_addr);
                vexa::dual_pointer curr_page = cpu->get_page(curr_addr);

                llvm::Value *curr_gep =
                    builder
                        ->inbounds_gep(
                            builder->getInt8Ty(),
                            curr_page.l,
                            llvm::ConstantInt::get(address_type, vexa::value::as_uint64(values[i])))
                        .l;

                llvm::Value *cond_l = builder->CreateICmpEQ(
                    address,
                    llvm::ConstantInt::get(address_type, vexa::value::as_uint64(values[i])));

                llvm::Value *curr_old_val = builder->CreateLoad(builder->getIntNTy(size), curr_gep);
                llvm::Value *curr_new_val = builder->CreateSelect(cond_l, val, curr_old_val);
                builder->CreateStore(curr_new_val, curr_gep);

                vexa::value *curr_old_sym = memory->read(curr_ptr, size);
                vexa::value *curr_cond_v = (*address_sym.v == *curr_addr);
                vexa::value *curr_new_sym = curr_cond_v->ite(*val_sym.v, *curr_old_sym)->simplify();
                memory->write(curr_ptr, curr_new_sym);
            }

            call.replaceAllUsesWith(call.getArgOperand(0));
            return nullptr;
        }

        if (values.size() == 1) {
            symex->set(address, symex->value(values.front()));
        }
    }

    builder->SetInsertPoint(call.getNextNode());
    vexa::dual_value ptr = cpu->value_to_pointer(address);
    vexa::pointer *ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    llvm::Value *val = call.getOperand(2);
    vexa::dual_value val_sym = VEXA_VAL(val);
    VEXA_ASSERT(val_sym.v->size() == val->getType()->getPrimitiveSizeInBits());

    run(builder->CreateStore(val, ptr.l));
    call.replaceAllUsesWith(call.getArgOperand(0));
    return nullptr;
}

llvm::BasicBlock *vexa::cpu::emulator::read_memory_intrinsic(llvm::CallInst &call, size_t size)
{
    // get pointer operand expression
    llvm::Value *addr = call.getOperand(1);
    vexa::dual_value addr_sym = VEXA_VAL(addr);
    addr_sym.v->simplify();

    
    if (addr_sym.v->is_symbolic() && !cpu->instruction.IsControlFlow()) {
        vexa::result result;
        std::vector<bwr::Expr> values = addr_sym.v->possible_values(cpu->path_constraints, &result);
        if (values.size() > 1 && result == vexa::result::unsat)
            return fork_memory_access(call, addr_sym.v, size, false, values);
    }

    if (addr_sym.v->is_symbolic()) {
        vexa::result result;
        std::vector<bwr::Expr> values = addr_sym.v->possible_values(cpu->path_constraints, &result);
        if (values.size() > 1 && result == vexa::result::unsat) {
            builder->SetInsertPoint(call.getNextNode());
            auto *address_type = llvm::dyn_cast<llvm::IntegerType>(addr->getType());
            VEXA_ASSERT(address_type);

            vexa::value *first_addr = symex->value(values.front());
            vexa::pointer *first_ptr = cpu->calculate_pointer(first_addr);
            vexa::dual_pointer first_page = cpu->get_page(first_addr);
            llvm::Value *first_gep =
                builder
                    ->inbounds_gep(
                        builder->getInt8Ty(),
                        first_page.l,
                        llvm::ConstantInt::get(
                            address_type, vexa::value::as_uint64(values.front())))
                    .l;
            llvm::Value *first_load = builder->CreateLoad(builder->getIntNTy(size), first_gep);
            vexa::value *first_sym = memory->read(first_ptr, size);
            symex->set(first_load, first_sym);
            llvm::Value *final_load = first_load;
            vexa::value *final_sym = first_sym;

            for (size_t i = 1; i < values.size(); ++i) {
                vexa::value *curr_addr = symex->value(values[i]);
                vexa::pointer *curr_ptr = cpu->calculate_pointer(curr_addr);
                vexa::dual_pointer curr_page = cpu->get_page(curr_addr);
                llvm::Value *curr_gep =
                    builder
                        ->inbounds_gep(
                            builder->getInt8Ty(),
                            curr_page.l,
                            llvm::ConstantInt::get(address_type, vexa::value::as_uint64(values[i])))
                        .l;
                llvm::Value *curr_load = builder->CreateLoad(builder->getIntNTy(size), curr_gep);
                vexa::value *curr_sym = memory->read(curr_ptr, size);
                symex->set(curr_load, curr_sym);
                llvm::Value *cond_l = builder->CreateICmpEQ(
                    addr, llvm::ConstantInt::get(address_type, vexa::value::as_uint64(values[i])));
                final_load = builder->CreateSelect(cond_l, curr_load, final_load);

                vexa::value *cond_v = (*addr_sym.v == *curr_addr);
                final_sym = cond_v->ite(*curr_sym, *final_sym);
            }

            final_sym = final_sym->simplify();
            symex->set(final_load, final_sym);
            call.replaceAllUsesWith(final_load);
            return nullptr;
        }
        if (values.size() == 1) {
            bwr::Expr& concretized = values.front();
            symex->set(addr, symex->value(concretized));
        }
    }

    builder->SetInsertPoint(call.getNextNode());
    vexa::dual_value ptr = cpu->value_to_pointer(addr);
    vexa::pointer *ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // create actual load
    auto *load = run(builder->CreateLoad(builder->getIntNTy(size), ptr.l)).l;
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
                switch (auto id = ID->getZExtValue()) {
                case SyncHyperCall::kX86CPUID: {
                    cpu->write_register(amd64::RAX, symex->concrete(0x00A50F00, 64));
                    cpu->write_register(amd64::RBX, symex->concrete(0x000C0800, 64));
                    cpu->write_register(amd64::RCX, symex->concrete(0x7EF8320B, 64));
                    cpu->write_register(amd64::RDX, symex->concrete(0x178BFBFF, 64));
                    break;
                }
                case SyncHyperCall::kX86ReadTSC: {
                    vexa::value *tsc = context->get_option(option::CONCRETIZE_RDTSC)
                                           ? tsc = symex->concrete(0x0, 64)
                                           : tsc = symex->symbolic("tsc", 64);
                    // rax
                    auto rax_low = tsc->extract(31, 0);
                    auto rax_high = symex->concrete(0, 32);
                    auto rax = rax_high->concat(*rax_low);
                    cpu->write_register(amd64::RAX, rax);
                    //rdx
                    auto rdx_low = tsc->extract(63, 32);
                    auto rdx_high = symex->concrete(0, 32);
                    auto rdx = rdx_high->concat(*rdx_low);
                    cpu->write_register(amd64::RDX, rdx);
                    break;
                }
                default: {
                    logger->flush_log();
                    I.print(llvm::outs());
                    llvm::outs() << "\n";
                    THROW("unimplemented SyncHyperCall id -> {}", id);
                }
                }

                for (auto &use : I.uses())
                    builder->deleteLater(llvm::dyn_cast<llvm::Instruction>(use.getUser()));
            }
            else {
                THROW("SyncHyperCall ID is not a constant");
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
        return vector->extract(low_bit + element_bit_width - 1, low_bit);
    }

    std::string poison_name =
        "extractelement_poison_" + std::to_string(reinterpret_cast<uintptr_t>(&I));

    bwr::Expr result = context->bitwright_ctx.symbol(poison_name, element_bit_width);

    uint64_t representable_elements =
        index->size() < 64 ? (uint64_t{1} << index->size()) : element_count;

    for (uint64_t element_index = 0;
         element_index < element_count && element_index < representable_elements;
         ++element_index)
    {
        uint64_t low_bit = element_index * element_bit_width;
        bwr::Expr element = vector->extract(low_bit + element_bit_width - 1, low_bit)->as_expr();
        vexa::value *expected_index = symex->concrete(element_index, index->size());
        result = (*index == *expected_index)->as_expr().select(element, result);
    }

    return symex->value(result);
}

vexa::value *vexa::cpu::emulator::visitSelectInst(llvm::SelectInst &I)
{
    vexa::value *cond = VEXA_VAL(I.getCondition()).v;
    vexa::value *lhs = VEXA_VAL(I.getTrueValue()).v;
    vexa::value *rhs = VEXA_VAL(I.getFalseValue()).v;

    if (cond->is_concrete()) {
        return cond->as_uint64() ? lhs : rhs;
    }
    return cond->ite(*lhs, *rhs);
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
        vexa::value *addr = *ptr + *_offset;
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

    vexa::value *expr;
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

    return expr;
}

vexa::value *vexa::cpu::emulator::visitICmpInst(llvm::ICmpInst &I)
{
    vexa::value *lhs = VEXA_VAL(I.getOperand(0)).v;
    vexa::value *rhs = VEXA_VAL(I.getOperand(1)).v;

    vexa::value *expr;
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

    return expr;
}

vexa::value *vexa::cpu::emulator::visitZExtInst(llvm::ZExtInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return src->zext(I.getDestTy()->getPrimitiveSizeInBits() - src->size());
}

vexa::value *vexa::cpu::emulator::visitSExtInst(llvm::SExtInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return src->sext(I.getDestTy()->getPrimitiveSizeInBits() - src->size());
}

vexa::value *vexa::cpu::emulator::visitTruncInst(llvm::TruncInst &I)
{
    vexa::value *src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    return src->extract(I.getDestTy()->getPrimitiveSizeInBits() - 1, 0);
}

vexa::value *vexa::cpu::emulator::handle_llvm_intrinsics(
    llvm::CallInst &I,
    llvm::Function *callee,
    llvm::Intrinsic::ID id)
{
    switch (id) {
    case llvm::Intrinsic::ctpop: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);
        bwr::Expr res = src->as_expr().popcnt();
        if (res.width() < dst_bit_width)
            res = res.zext(dst_bit_width);
        else if (res.width() > dst_bit_width)
            res = res.trunc(dst_bit_width);
        return symex->value(res);
    }
    case llvm::Intrinsic::fshl: {
        vexa::value *src_x = symex->get(I.getOperand(0));
        vexa::value *src_y = symex->get(I.getOperand(1));
        vexa::value *shift = symex->get(I.getOperand(2));

        uint8_t bit_width = src_x->size();
        bwr::Expr x = src_x->as_expr();
        bwr::Expr y = src_y->as_expr();
        bwr::Expr s = shift->as_expr();
        if (x == y) {
            return symex->value(x.rotl(s));
        }
        bwr::Expr s_mod = s.urem(context->bitwright_ctx.constant(s.width(), bit_width));
        bwr::Expr wide = x.concat(y);
        bwr::Expr shifted = wide << s_mod.zext(bit_width * 2);
        bwr::Expr result = shifted.extract(bit_width, bit_width);
        return symex->value(result);
    }
    case llvm::Intrinsic::fshr: {
        vexa::value *src_x = symex->get(I.getOperand(0));
        vexa::value *src_y = symex->get(I.getOperand(1));
        vexa::value *shift = symex->get(I.getOperand(2));

        uint8_t bit_width = src_x->size();
        bwr::Expr x = src_x->as_expr();
        bwr::Expr y = src_y->as_expr();
        bwr::Expr s = shift->as_expr();
        if (x == y) {
            return symex->value(x.rotr(s));
        }
        bwr::Expr s_mod = s.urem(context->bitwright_ctx.constant(s.width(), bit_width));
        bwr::Expr wide = x.concat(y);
        bwr::Expr shifted = wide >> s_mod.zext(bit_width * 2);
        bwr::Expr result = shifted.extract(0, bit_width);
        return symex->value(result);
    }
    case llvm::Intrinsic::bswap: {
        vexa::value *src = symex->get(I.getOperand(0));
        return symex->value(src->as_expr().bswap());
    }
    case llvm::Intrinsic::bitreverse: {
        vexa::value *src = symex->get(I.getOperand(0));
        return symex->value(src->as_expr().bitrev());
    }
    case llvm::Intrinsic::cttz: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);
        bwr::Expr res = src->as_expr().ctz();
        if (res.width() < dst_bit_width)
            res = res.zext(dst_bit_width);
        else if (res.width() > dst_bit_width)
            res = res.trunc(dst_bit_width);
        return symex->value(res);
    }
    case llvm::Intrinsic::ctlz: {
        vexa::value *src = symex->get(I.getOperand(0));
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        VEXA_ASSERT(dst_bit_width);
        bwr::Expr res = src->as_expr().clz();
        if (res.width() < dst_bit_width)
            res = res.zext(dst_bit_width);
        else if (res.width() > dst_bit_width)
            res = res.trunc(dst_bit_width);
        return symex->value(res);
    }
    case llvm::Intrinsic::umax: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        bwr::Expr res = src0->uge(*src1)->as_expr().select(src0->as_expr(), src1->as_expr());
        return symex->value(res);
    }
    case llvm::Intrinsic::umin: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        bwr::Expr res = src0->ule(*src1)->as_expr().select(src0->as_expr(), src1->as_expr());
        return symex->value(res);
    }
    case llvm::Intrinsic::smax: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        return symex->value(a.sge(b).select(a, b));
    }
    case llvm::Intrinsic::smin: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        return symex->value(a.sle(b).select(a, b));
    }
    case llvm::Intrinsic::scmp: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint16_t dst_w = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        bwr::Expr neg1 = ~context->bitwright_ctx.constant(dst_w, 0);
        bwr::Expr zero = context->bitwright_ctx.constant(dst_w, 0);
        bwr::Expr pos1 = context->bitwright_ctx.constant(dst_w, 1);
        bwr::Expr res = a.slt(b).select(neg1, a.sgt(b).select(pos1, zero));
        return symex->value(res);
    }
    case llvm::Intrinsic::ucmp: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint16_t dst_w = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        bwr::Expr neg1 = ~context->bitwright_ctx.constant(dst_w, 0);
        bwr::Expr zero = context->bitwright_ctx.constant(dst_w, 0);
        bwr::Expr pos1 = context->bitwright_ctx.constant(dst_w, 1);
        bwr::Expr res = a.ult(b).select(neg1, a.ugt(b).select(pos1, zero));
        return symex->value(res);
    }
    case llvm::Intrinsic::uadd_sat: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint16_t w = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        bwr::Expr sum = a + b;
        bwr::Expr ones = ~context->bitwright_ctx.constant(w, 0);
        return symex->value(sum.ult(a).select(ones, sum));
    }
    case llvm::Intrinsic::usub_sat: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint8_t dst_bit_width = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr zero = context->bitwright_ctx.constant(dst_bit_width, 0);
        bwr::Expr sub = (*src0 - *src1)->as_expr();
        bwr::Expr res = src0->ult(*src1)->as_expr().select(zero, sub);
        return symex->value(res);
    }
    case llvm::Intrinsic::sadd_sat: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint16_t w = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        bwr::Expr sum = a + b;
        bwr::Expr zero = context->bitwright_ctx.constant(w, 0);
        bwr::Expr smin = context->bitwright_ctx.constant(w, 1) << context->bitwright_ctx.constant(w, w - 1);
        bwr::Expr smax = ~smin;
        bwr::Expr ovf = (a.slt(zero).eq(b.slt(zero))) & (sum.slt(zero).ne(a.slt(zero)));
        bwr::Expr sat = a.slt(zero).select(smin, smax);
        return symex->value(ovf.select(sat, sum));
    }
    case llvm::Intrinsic::ssub_sat: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        uint16_t w = I.getType()->getPrimitiveSizeInBits();
        bwr::Expr a = src0->as_expr();
        bwr::Expr b = src1->as_expr();
        bwr::Expr diff = a - b;
        bwr::Expr zero = context->bitwright_ctx.constant(w, 0);
        bwr::Expr smin = context->bitwright_ctx.constant(w, 1) << context->bitwright_ctx.constant(w, w - 1);
        bwr::Expr smax = ~smin;
        bwr::Expr ovf = (a.slt(zero).ne(b.slt(zero))) & (diff.slt(zero).ne(a.slt(zero)));
        bwr::Expr sat = a.slt(zero).select(smin, smax);
        return symex->value(ovf.select(sat, diff));
    }
    case llvm::Intrinsic::abs: {
        vexa::value *src = symex->get(I.getOperand(0));
        bwr::Expr x = src->as_expr();
        bwr::Expr zero = context->bitwright_ctx.constant(x.width(), 0);
        bwr::Expr res = x.slt(zero).select(-x, x);
        return symex->value(res);
    }
    case llvm::Intrinsic::x86_bmi_pdep_32:
    case llvm::Intrinsic::x86_bmi_pdep_64: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        return symex->value(src0->as_expr().pdep(src1->as_expr()));
    }
    case llvm::Intrinsic::x86_bmi_pext_32:
    case llvm::Intrinsic::x86_bmi_pext_64: {
        vexa::value *src0 = symex->get(I.getOperand(0));
        vexa::value *src1 = symex->get(I.getOperand(1));
        return symex->value(src0->as_expr().pext(src1->as_expr()));
    }
    case llvm::Intrinsic::expect:
    case llvm::Intrinsic::expect_with_probability: {
        return symex->get(I.getOperand(0));
    }
    case llvm::Intrinsic::is_constant: {
        vexa::value *src = symex->get(I.getOperand(0));
        return symex->concrete(src->is_concrete() ? 1 : 0, 1);
    }
    case llvm::Intrinsic::assume:
    case llvm::Intrinsic::sideeffect:
    case llvm::Intrinsic::pseudoprobe:
    case llvm::Intrinsic::dbg_declare:
    case llvm::Intrinsic::dbg_value:
    case llvm::Intrinsic::dbg_label:
    case llvm::Intrinsic::invariant_start:
    case llvm::Intrinsic::invariant_end:
    case llvm::Intrinsic::lifetime_start:
    case llvm::Intrinsic::lifetime_end:
        break;
    default:
        THROW("CallInst: unimplemented llvm intrinsic -> {}", callee->getName().str());
    }
    return symex->concrete(0, 64);
}