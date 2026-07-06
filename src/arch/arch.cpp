#include <vexa/vexa.h>

#include <format>

#include <llvm/Transforms/Utils/Cloning.h>
#include <remill/BC/Util.h>

vexa::cpu::cpu(vexa::context* _context)  :
    context(_context), builder(context->builder), symex(context->symex), memory(context->memory), p_manager(this, builder.get())
{ }

vexa::cpu::snapshot vexa::cpu::take_snapshot()
{
    return vexa::cpu::snapshot{memory->take_snapshot(), p_manager.lifted_blocks};
}

void vexa::cpu::restore_snapshot(vexa::cpu::snapshot ss)
{
    memory->restore_snapshot(ss.mem_ss);
    p_manager.lifted_blocks = ss.lifted_blocks;
}

remill::Register* vexa::cpu::get_register(vexa::reg_t r)
{
    TRY()
    VEXA_ASSERT(registers.count(r));
    return registers[r];
    CATCH()
}

vexa::reg_t vexa::cpu::str2reg(std::string r)
{
    TRY()
    VEXA_ASSERT(str_to_reg.count(r));
    return str_to_reg[r];
    CATCH()
}

void vexa::cpu::initialize_arch()
{
    TRY()

    // initialize registers
    auto callback = [&](const remill::Register* r)
    {
        //if (r->offset == 2312) std::cout << r->name << std::endl;
        vexa::reg_t reg = str2reg(r->name);
        registers[reg] = (remill::Register*)r;
    };
    arch->ForEachRegister(callback);

    // load semantics and get llvm module
    std::unique_ptr<llvm::Module> module = remill::LoadArchSemantics(arch.get());
    VEXA_ASSERT(module);

    global_memory = memory->allocate();
    context->llvm_module = std::move(module);
    executor = std::make_shared<symbolic_executor>(context, this);

    // init function and environment
    auto m = context->llvm_module.get();
    vexa_lifted = arch->DeclareLiftedFunction("vexa_lifted", m);
    arch->InitializeEmptyLiftedFunction(vexa_lifted);
    VEXA_ASSERT(vexa_lifted);
    dec_context = arch->CreateInitialContext();

    CATCH()
}

#define EXEC(val) executor->execute(llvm::dyn_cast<llvm::Instruction>(val))
#define SYM_VAL(llvm_ptr) symex->get(llvm_ptr)
#define SYM_PTR(llvm_ptr) vexa::dyn_cast<vexa::pointer>(symex->get(llvm_ptr))

void clearMetadata(llvm::Function* F) {
    for (auto& BB : *F) {
        for (auto& I : BB) {
            llvm::SmallVector<std::pair<unsigned, llvm::MDNode*>, 8> MDs;
            I.getAllMetadataOtherThanDebugLoc(MDs);

            for (auto& MD : MDs) {
                I.setMetadata(MD.first, nullptr);
            }

            I.setDebugLoc(llvm::DebugLoc());
        }
    }
}

void vexa::cpu::run(uint64_t pc)
{
    TRY()

    // function arguments
    auto args = vexa_lifted->arg_begin();
    state_ptr = &*args++;
    pc_arg = &*args++;
    mem_ptr = &*args++;

    // create and set expressions for arguments
    symex->set(state_ptr, symex->pointer(symex->concrete(0, 64), memory->allocate()));
    symex->set(pc_arg, symex->concrete(pc, 64));
    symex->set(mem_ptr, symex->pointer(symex->concrete(0, 64), memory->allocate()));

    // load all registers in entry block for once, so they're not loaded when needed during lifting
    auto callback = [&](const remill::Register* r) {
        r->AddressOf(state_ptr, &vexa_lifted->front());
        if (!r->parent)
            memory->write(symex->pointer(symex->concrete(r->offset, 64), vexa::to_ptr(symex->get(state_ptr))->get_page()),
                          symex->symbolic(r->name, r->size * 8));
    };
    arch->ForEachRegister(callback);

    // initialize lifter
    intrinsics.emplace(context->llvm_module.get());
    lifter.emplace(arch.get(), *intrinsics);
    builder->set_function(vexa_lifted);
    builder->SetInsertPoint(&vexa_lifted->front());

    // replace remill intrinsics with llvm instructions
    replace_remill_intrinsics();

    // initialize stack
    //      %stack = alloca i8, i64 8192
    std::shared_ptr<mem_page> stack_page = memory->allocate(8192);
    llvm::AllocaInst* stack = builder->CreateAlloca(builder->getInt8Ty(), builder->getIntN(64, 8192), "stack");
    symex->set(stack, symex->pointer(symex->concrete(0, 64), stack_page));

    // stack pointer
    //      %stack_ptr = getelementptr inbounds i8, ptr %stack, i64 4096
    stack_ptr = builder->CreateInBoundsGEP(builder->getInt8Ty(), stack, builder->getIntN(64, 4096), "stack_ptr");
    vexa::shared_pointer _stack_ptr = symex->pointer(symex->concrete(4096, 64), stack_page);
    symex->set(stack_ptr, _stack_ptr);

    // write zero to sp register
    llvm::Value* sp = get_register(vexa::amd64::SP)->AddressOf(state_ptr, *builder);
    builder->CreateStore(builder->getIntN(64, -8), sp);

    // return address init, when function ends with a ret, this value will be read
    auto* s = builder->CreateInBoundsGEP(builder->getInt8Ty(),
                                         stack_ptr,
                                         builder->getIntN(64, -8));
    builder->CreateStore(builder->getIntN(64, 0), s);

    program_counter = pc;

    // execute the entry block, then we are ready for lifting
    executor->execute_block(&vexa_lifted->front());
    next_pc = builder->get_value_by_name("NEXT_PC");
    branch_taken = builder->get_value_by_name("BRANCH_TAKEN");

    uint64_t concrete_gs_base = 0x7fffff000000;
    uint64_t concrete_peb_addr = 0x7fffff800000;
    uint64_t concrete_image_base = 0x140000000;

    vexa::dual_value GS = builder->get_value_by_name("GSBASE");
    VEXA_ASSERT(GS.l);
    memory->write(vexa::to_ptr(symex->get(GS.l)), symex->concrete(concrete_gs_base, 64));
    auto peb_loc = symex->pointer(symex->concrete(concrete_gs_base + 0x60, 64), global_memory);
    memory->write(peb_loc, symex->concrete(concrete_peb_addr, 64));
    auto image_base_loc = symex->pointer(symex->concrete(concrete_peb_addr + 0x10, 64), global_memory);
    memory->write(image_base_loc, symex->concrete(concrete_image_base, 64));

    while (true)
    {
        internal_lifter_status status = process_instruction();
        switch (status)
        {
        case internal_lifter_status::function_ended:
            std::cout << "[engine] reached the end of the function" << std::endl;
            [[fallthrough]];
        case internal_lifter_status::explore_other_paths:
        {
            builder->CreateRet(state_ptr);
            if (p_manager.unexplored_paths.empty())
            {
                std::cout << "[engine] no more paths to explore\n"
                          << std::endl;
                goto ret; // finish execution
            }

            std::cout << "[engine] exploring new path" << std::endl;
            path_state path = p_manager.unexplored_paths.top(); p_manager.unexplored_paths.pop();
            builder->SetInsertPoint(path.bb);
            program_counter = path.pc;
            restore_snapshot(path.ss);
            [[fallthrough]];
        }
        case internal_lifter_status::successful:
        default:
            break;
        }
    }

ret:

    clearMetadata(vexa_lifted);
    return;
    CATCH()
}

vexa::cpu::internal_lifter_status vexa::cpu::process_instruction()
{
    TRY()
    if (program_counter == 0)
        return internal_lifter_status::function_ended;

    // create loop
#if VCFG_RECOVERAGE
    if (lifted_blocks.count(current_vip) && vbranching)
    {
        builder->jump(lifted_blocks[current_vip]);
        return internal_lifter_status::explore_other_paths;
    }
#else
    if (p_manager.lifted_blocks.count(program_counter) && false)
    {
        std::cout << "created a loop" << std::endl;
        builder->CreateBr(p_manager.lifted_blocks[program_counter]);
        return internal_lifter_status::explore_other_paths;
    }
#endif

    // read 15 bytes at program counter
    std::string bytes_str;
    bytes_str.reserve(15);
    // custom memory access for performance
    auto& cm = global_memory->concrete_memory;
    for (int i = 0; i < 15; i++)
    {
        auto it = cm.find(program_counter + i);
        if (it == cm.end() || !it->second.has_value()) break;
        const z3::expr& byte_expr = (*it->second);
        if (!byte_expr.is_numeral()) break;
        bytes_str.push_back(static_cast<char>(byte_expr.get_numeral_uint64() & 0xFF));
    }

    // disassemble instruction
    remill::Instruction inst;
    if (!arch->DecodeInstruction(program_counter, bytes_str, inst, dec_context)) {
        std::cout << "[error] disassemble fail" << std::endl;
        return internal_lifter_status::explore_other_paths;
    }

    // lift instruction
    std::cout << std::hex << program_counter << " " << inst.disassembly << "\n";
    program_counter = lift_instruction(inst);
    p_manager.lifted_count++;
    return internal_lifter_status::successful;

    CATCH()
}

uint64_t vexa::cpu::lift_instruction(remill::Instruction inst)
{
    // create basic block for every instruction
    llvm::BasicBlock* block = builder->basic_block(inst.function + "_");
    builder->CreateBr(block);
    builder->SetInsertPoint(block);

    EXEC(builder->CreateStore(builder->getIntN(64, program_counter), next_pc.l));
    if (lifter->LiftIntoBlock(inst, block, state_ptr) != remill::kLiftedInstruction)
    {
        std::cout << "[error] lifting failed for instruction -> " << inst.function << std::endl;
        return -1;
    }

    // simulate the lifted block using z3
    executor->execute_block(block);
    p_manager.lifted_blocks[program_counter] = block;

    // control flow instructions
    if (inst.IsControlFlow())
    {
        if (inst.IsConditionalBranch())
        {
            vexa::dual_value cond = get_condition(block);
            return p_manager.branching(cond, inst.branch_taken_pc, inst.branch_not_taken_pc);
        }

        if (inst.IsDirectControlFlow())
        {
            return p_manager.direct_branch(inst.branch_taken_pc);
        }

        vexa::dual_value next = get_next_pc(block);
        next.v->simplify();

        if (next.v->is_concrete())
        {
            return p_manager.direct_branch(next.v->as_uint64());
        }

        std::cout << next.v->as_expr() << std::endl;
        THROW("unresolved jump");
    }
    // custom handling of cmov semantics
    else if (inst.function.starts_with("CMOV"))
    {
        handle_conditional_moves(inst, block);
    }

    return inst.next_pc;
}

void vexa::cpu::handle_conditional_moves(remill::Instruction inst, llvm::BasicBlock* block)
{
    /*
    CMOVB_GPRv_GPRv_64_:
        %48 = load i64, ptr %NEXT_PC, align 8
        store i64 %48, ptr %PC, align 8
        %49 = add i64 %48, 4
        store i64 %49, ptr %NEXT_PC, align 8
        %50 = load i64, ptr %R14, align 8
        %51 = load ptr, ptr %MEMORY, align 8
        %52 = getelementptr inbounds nuw i8, ptr %state, i64 2065
        %53 = load i8, ptr %52, align 1, !tbaa !132
        %54 = icmp ne i8 %53, 0
        %55 = load i64, ptr %RAX, align 8, !tbaa !130
        %56 = select i1 %54, i64 %50, i64 %55
        store i64 %56, ptr %RAX, align 8, !tbaa !130
    */

    llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&block->back());
    VEXA_ASSERT(store);
    llvm::SelectInst* select = llvm::dyn_cast<llvm::SelectInst>(store->getValueOperand());
    VEXA_ASSERT(select);

    llvm::Value* store_ptr = store->getPointerOperand();
    vexa::shared_value cond_val = SYM_VAL(select->getCondition());
    cond_val->simplify();
    if (cond_val->is_concrete())
        return;

    vexa::cpu::snapshot base_snapshot = take_snapshot();
    // path forking
    llvm::BasicBlock* true_block = builder->basic_block();
    llvm::BasicBlock* false_block = builder->basic_block();
    builder->CreateCondBr(select->getCondition(), true_block, false_block);

    // false path
    builder->SetInsertPoint(false_block);
    EXEC(builder->CreateStore(select->getFalseValue(), store_ptr));
    vexa::cpu::path_state false_path = {take_snapshot(), inst.next_pc, false_block, p_manager.current_vip};
    p_manager.unexplored_paths.push(false_path);

    // true path
    restore_snapshot(base_snapshot);
    builder->SetInsertPoint(true_block);
    EXEC(builder->CreateStore(select->getTrueValue(), store_ptr));

    store->eraseFromParent();
}

vexa::dual_value vexa::cpu::get_condition(llvm::BasicBlock* BB)
{
    for (auto it = BB->rbegin(), end = BB->rend(); it != end; it++)
    {
        llvm::Instruction* I = &*it;
        if (auto *store = llvm::dyn_cast<llvm::StoreInst>(I))
        {
            // does it store to the BRANCH_TAKEN alloca?
            if (store->getPointerOperand() == branch_taken.l)
            {
                // if yes, we return the value operand
                vexa::dual_value val = EXEC(builder->CreateTrunc(store->getValueOperand(), builder->getInt1Ty()));
                return val;
            }
        }
    }

    THROW("couldnt get condition");
}

vexa::dual_value vexa::cpu::get_next_pc(llvm::BasicBlock* BB)
{
    for (auto it = BB->rbegin(), end = BB->rend(); it != end; it++)
    {
        llvm::Instruction* I = &*it;
        if (auto *store = llvm::dyn_cast<llvm::StoreInst>(I))
        {
            // does it store to the NEXT_PC alloca?
            if (store->getPointerOperand() == next_pc.l)
            {
                // if yes, we return the value operand
                auto *val = store->getValueOperand();
                return vexa::dual_value(val, SYM_VAL(val));
            }
        }
    }

    THROW("couldnt get next pc");
}

void vexa::cpu::symbolic_executor::execute_block(llvm::BasicBlock* BB)
{
    TRY()
    for (auto &I : llvm::make_early_inc_range(*BB))
    {
        if (I.isTerminator() /*|| symex->is_sync(&I)*/)
            continue;

        llvm::Value* I_ptr = &I;
        vexa::shared_value res = visit(I);

        // we dont check if instruction is already executed before
        // because erased intrinsic call's addresses might be used for new instructions
        // see visitCallInst

        //if (!symex->is_sync(I_ptr))
        symex->set(I_ptr, res);
    }
    CATCH()
}

vexa::dual_value vexa::cpu::value_to_pointer(llvm::Value* addr)
{
    // save insert point
    builder->push_ip();

    // get value and simplify
    vexa::shared_value v = SYM_VAL(addr);
    v->simplify();

    vexa::dual_value ptr;
    if (v->is_concrete())
    {
        // if its lower than 0, we take it as a stack access
        // TODO: implement a better stack access detection method
        if (static_cast<int64_t>(v->as_uint64()) <= 0)
        {
            // stack access
            ptr = builder->inbounds_gep(builder->getInt8Ty(), stack_ptr, addr);
            goto ret;
        }
    }

    // global memory access
    // TODO: implement inttoptr
    ptr = builder->inttoptr(addr, builder->getPtrTy(), global_memory);
ret:
    // restore insert point
    builder->pop_ip();
    return ptr;
}

void vexa::cpu::symbolic_executor::write_memory_intrinsic(llvm::CallInst& call, size_t size)
{
    builder->SetInsertPoint(&call);
    // get pointer operand expression
    vexa::dual_value ptr = cpu->value_to_pointer(call.getOperand(1));
    vexa::shared_pointer ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // get value operand expression
    llvm::Value* val = call.getOperand(2);
    vexa::shared_value val_sym = SYM_VAL(val);
    VEXA_ASSERT(val_sym->size() == val->getType()->getPrimitiveSizeInBits());

    // create actual store
    execute(builder->CreateStore(val, ptr.l));
}

void vexa::cpu::symbolic_executor::read_memory_intrinsic(llvm::CallInst& call, size_t size)
{
    builder->SetInsertPoint(&call);
    // get pointer operand expression
    vexa::dual_value ptr = cpu->value_to_pointer(call.getOperand(1));
    vexa::shared_pointer ptr_sym = vexa::to_ptr(ptr.v);
    VEXA_ASSERT(ptr_sym);

    // create actual load
    llvm::Value* load = execute(builder->CreateLoad(builder->getIntNTy(size), ptr.l)).l;
    call.replaceAllUsesWith(load);
}

vexa::shared_value vexa::cpu::symbolic_executor::visitCallInst(llvm::CallInst& I)
{
    llvm::Function* callee = I.getCalledFunction();
    VEXA_ASSERT(callee);
    if (callee->isIntrinsic()) // if the callee is an llvm intrinsic
    {
        auto id = callee->getIntrinsicID();
        switch (id) {
        case llvm::Intrinsic::ctpop:
        {
            vexa::shared_value src = symex->get(I.getOperand(0));
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
            vexa::shared_value src_x = symex->get(I.getOperand(0));
            vexa::shared_value src_y = symex->get(I.getOperand(1));
            vexa::shared_value shift = symex->get(I.getOperand(2));

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
            vexa::shared_value src_x = symex->get(I.getOperand(0));
            vexa::shared_value src_y = symex->get(I.getOperand(1));
            vexa::shared_value shift = symex->get(I.getOperand(2));

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
            vexa::shared_value src = symex->get(I.getOperand(0));
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
            vexa::shared_value src = symex->get(I.getOperand(0));
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
            vexa::shared_value src = symex->get(I.getOperand(0));
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
        default:
            THROW("CallInst: unimplemented llvm intrinsic -> " + callee->getName().str());
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

        else
            THROW("CallInst: unknown remill intrinsic -> " + callee->getName().str());

        I.eraseFromParent();

        // restore insert point
        builder->pop_ip();
        return symex->concrete(0, 64);
    }
}

vexa::dual_value vexa::cpu::symbolic_executor::execute(llvm::Instruction* I)
{
    TRY()

    vexa::shared_value res = visit(I);
    symex->set(I, res);
    return vexa::dual_value(I, res);

    CATCH()
}

vexa::shared_value vexa::cpu::symbolic_executor::visitInstruction(llvm::Instruction& I)
{
    THROW("unsupported instruction -> " + std::string(I.getOpcodeName()));
}

vexa::shared_value vexa::cpu::symbolic_executor::visitFreezeInst(llvm::FreezeInst& I)
{
    return SYM_VAL(I.getOperand(0));
}

vexa::shared_value vexa::cpu::symbolic_executor::visitSelectInst(llvm::SelectInst& I)
{
    vexa::shared_value cond = SYM_VAL(I.getCondition());
    vexa::shared_value lhs = SYM_VAL(I.getTrueValue());
    vexa::shared_value rhs = SYM_VAL(I.getFalseValue());

    z3::expr cond_bool = cond->as_expr() == cond->as_expr().ctx().bv_val(1, 1);
    vexa::shared_value expr = symex->value(z3::ite(cond_bool, lhs->as_expr(), rhs->as_expr()));
    return expr;
}

vexa::shared_value vexa::cpu::symbolic_executor::visitAllocaInst(llvm::AllocaInst& I)
{
    return symex->pointer(symex->concrete(0, 64),
                          memory->allocate(I.getAllocationSize(DL)->getFixedValue()));
}

vexa::shared_value vexa::cpu::symbolic_executor::visitGetElementPtrInst(llvm::GetElementPtrInst &I)
{
    llvm::APInt offset(64, 0);
    if (I.accumulateConstantOffset(DL, offset))
    {
        // calculate pointerOperand value + constant offset
        vexa::shared_value _offset = symex->concrete(offset.getZExtValue(), 64);
        vexa::shared_pointer ptr = vexa::dyn_cast<vexa::pointer>(symex->get(I.getPointerOperand()));
        vexa::shared_value addr = symex->value(ptr->as_expr() + _offset->as_expr());
        return symex->pointer(addr, ptr->get_page());
    }
    THROW("getelementptr");
}

vexa::shared_value vexa::cpu::symbolic_executor::visitStoreInst(llvm::StoreInst& I)
{
    vexa::shared_pointer ptr = SYM_PTR(I.getPointerOperand());
    vexa::shared_value val = SYM_VAL(I.getValueOperand());
    memory->write(ptr, val);
    return symex->concrete(0, 64);
}

vexa::shared_value vexa::cpu::symbolic_executor::visitLoadInst(llvm::LoadInst& I)
{
    vexa::shared_value ptr_val = symex->get(I.getPointerOperand());
    if (vexa::shared_pointer ptr = vexa::dyn_cast<vexa::pointer>(ptr_val))
    {
        // read from memory
        vexa::shared_value read = memory->read(ptr, DL.getTypeStoreSizeInBits(I.getType()));
        return read;
    }

    THROW("LoadInst: pointer operand is not a vexa::pointer");
    return symex->concrete(1, 64);
}

vexa::shared_value vexa::cpu::symbolic_executor::visitBinaryOperator(llvm::BinaryOperator& I)
{
    z3::expr lhs = SYM_VAL(I.getOperand(0))->as_expr();
    z3::expr rhs = SYM_VAL(I.getOperand(1))->as_expr();

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
        THROW("unsupported binary operator -> " + std::string(I.getOpcodeName()));
    }

    return symex->value(*expr);
}

vexa::shared_value vexa::cpu::symbolic_executor::visitICmpInst(llvm::ICmpInst& I)
{
    TRY()
    z3::expr lhs = SYM_VAL(I.getOperand(0))->as_expr();
    z3::expr rhs = SYM_VAL(I.getOperand(1))->as_expr();

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
        THROW("unsupported icmp predicate -> " + std::string(I.getPredicateName(I.getPredicate())));
    }

    expr = z3::ite(*expr, context->z3_context->bv_val(1, 1),  context->z3_context->bv_val(0, 1));
    return symex->value(*expr);
    CATCH()
}

vexa::shared_value vexa::cpu::symbolic_executor::visitZExtInst(llvm::ZExtInst& I)
{
    TRY()
    vexa::shared_value src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::shared_value extended = symex->value(z3::zext(src->as_expr(), I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
    return extended;
    CATCH()
}

vexa::shared_value vexa::cpu::symbolic_executor::visitSExtInst(llvm::SExtInst& I)
{
    TRY()
    vexa::shared_value src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::shared_value extended = symex->value(z3::sext(src->as_expr(), I.getDestTy()->getPrimitiveSizeInBits() - src->size()));
    return extended;
    CATCH()
}

vexa::shared_value vexa::cpu::symbolic_executor::visitTruncInst(llvm::TruncInst& I)
{
    TRY()
    vexa::shared_value src = symex->get(I.getOperand(0));
    // check if the operand size in llvm matches with it's size in symex
    VEXA_ASSERT(src->size() == I.getSrcTy()->getPrimitiveSizeInBits());
    vexa::shared_value truncated = symex->value(src->as_expr().extract(I.getDestTy()->getPrimitiveSizeInBits() - 1, 0));
    return truncated;
    CATCH()
}

void vexa::cpu::replace_remill_intrinsics()
{
    auto replace_with_first_arg = [&](llvm::Function* F)
    {
        if (!F) return;

        for (auto *U : llvm::make_early_inc_range(F->users()))
        {
            auto *call = llvm::dyn_cast<llvm::CallInst>(U);
            if (!call) continue;

            llvm::Value *first_arg = call->getArgOperand(0);
            call->replaceAllUsesWith(first_arg);
            call->eraseFromParent();
        }
    };

    replace_with_first_arg(intrinsics->compare_eq);
    replace_with_first_arg(intrinsics->compare_neq);
    replace_with_first_arg(intrinsics->compare_sgt);
    replace_with_first_arg(intrinsics->compare_sle);
    std::vector<std::string> extra_compares = {
        "__remill_compare_slt",
        "__remill_compare_sge",
        "__remill_compare_ugt",
        "__remill_compare_uge",
        "__remill_compare_ult",
        "__remill_compare_ule"
    };

    for (const auto& func_name : extra_compares) {
        replace_with_first_arg(context->llvm_module->getFunction(func_name));
    }

    replace_with_first_arg(intrinsics->flag_computation_carry);
    replace_with_first_arg(intrinsics->flag_computation_zero);
    replace_with_first_arg(intrinsics->flag_computation_sign);
    replace_with_first_arg(intrinsics->flag_computation_overflow);

    auto replace_with_undefined = [&](llvm::Function* F, size_t size)
    {
        if (!F) return;

        for (auto *U : llvm::make_early_inc_range(F->users()))
        {
            auto *call = llvm::dyn_cast<llvm::CallInst>(U);
            if (!call) continue;

            llvm::Value* undef = llvm::UndefValue::get(builder->getIntNTy(size));
            symex->set(undef, symex->symbolic("undefined_" + std::to_string(size), size));

            call->replaceAllUsesWith(undef);
            call->eraseFromParent();
        }
    };

    replace_with_undefined(intrinsics->undefined_64, 64);
    replace_with_undefined(intrinsics->undefined_32, 32);
    replace_with_undefined(intrinsics->undefined_16, 16);
    replace_with_undefined(intrinsics->undefined_8, 8);
}


void vexa::cpu::path_manager::handle_loops(uint64_t addr)
{
    if (lifted_blocks.count(addr))
    {
        // means this is a loop backedge
        // erase the trace that from the target addr to current rip
        // so we can execute the same path again and unroll the loop
        lifted_blocks.erase_range(addr, cpu->program_counter);
        //builder->CreateCall((llvm::Function*)cpu->context->MarkerFunc.getCallee());
    }
}

uint64_t vexa::cpu::path_manager::branching(vexa::dual_value condition, vexa::cpu::resolved_path_t path, uint64_t fallthrough_pc)
{
    // we take an extra uint64_t parameter as fallthrough_pc
    // because z3 might switch the ite expression nodes
    // that means resolved true and false paths can be opposite placed in ast
    // this way, we verify which one is jump address and which one is fallthrough address

    uint64_t jump_target_pc = (path.true_ip == fallthrough_pc) ? path.false_ip : path.true_ip;
    return -1;
}

uint64_t vexa::cpu::path_manager::branching(vexa::dual_value condition, uint64_t jump_pc, uint64_t fallthrough_pc)
{
    condition.v->simplify();
    // basic opaque predicate detection
    if (condition.v->is_concrete())
    {
        uint64_t is_taken = condition.v->as_uint64();
        if (is_taken)
            return direct_branch(jump_pc);
        return direct_branch(fallthrough_pc);
    }

    // opaque solving with z3
    if (true)
    {
        z3::context &c = *cpu->context->z3_context;
        z3::solver solver(c);
        z3::expr cond = condition.v->as_expr();

        // check if cond can be false
        solver.add(cond != 1);
        if (solver.check() == z3::unsat) // means it cant be false
            return direct_branch(jump_pc); // we solved this branch is always taken

        // check if cond can be true
        solver.reset();
        solver.add(cond != 0);
        if (solver.check() == z3::unsat) // means it cant be true
            return direct_branch(fallthrough_pc); // we solved this branch is never taken
    }

    llvm::BasicBlock *jump_bb = builder->basic_block(utils::addr_to_str(jump_pc));
    llvm::BasicBlock *fallthrough_bb = builder->basic_block(utils::addr_to_str(fallthrough_pc));

    path_state path_s{cpu->take_snapshot(), fallthrough_pc, fallthrough_bb, current_vip};
    unexplored_paths.push(path_s);

    builder->CreateCondBr(condition.l, jump_bb, fallthrough_bb);
    builder->SetInsertPoint(jump_bb);
    return jump_pc;
}

uint64_t vexa::cpu::path_manager::direct_branch(uint64_t addr)
{
    handle_loops(addr);
    return addr;
}

vexa::cpu::resolved_path_t vexa::cpu::path_manager::resolve_path(vexa::shared_value v)
{
    v->simplify();

    z3::expr v_expr = v->as_expr();
    if (v_expr.is_app() && v_expr.decl().decl_kind() == Z3_OP_ITE)
    {
        z3::expr cond = v_expr.arg(0);
        z3::expr then_expr = v_expr.arg(1);
        z3::expr else_expr = v_expr.arg(2);

        std::cout << "CONDITION: " << cond << std::endl;
        std::cout << "THEN EXPR: " << then_expr << std::endl;
        std::cout << "ELSE EXPR: " << else_expr << std::endl;

        if (then_expr.is_numeral() && else_expr.is_numeral())
            return vexa::cpu::resolved_path_t(
                       then_expr.as_uint64(),
                       else_expr.as_uint64());
    }

    std::cout << v_expr << std::endl;
    THROW("failed to resolve indirect jump");
}
