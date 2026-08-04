#include <vexa/vexa.h>

#include <format>

#include <remill/BC/Util.h>

#define EVENT_HANDLER(event_k) context->event_handler(event_k);

vexa::cpu::cpu(vexa::context* _context)  :
    context(_context), builder(context->builder), symex(context->symex), memory(context->memory)
{ }

vexa::cpu::snapshot vexa::cpu::take_snapshot(uint64_t pc, llvm::BasicBlock* bb)
{
    return vexa::cpu::snapshot{
        memory->take_snapshot(),
        pc,
        VPC,
        bb,
        VJMP,
        PATH
    };
}

void vexa::cpu::restore_snapshot(vexa::cpu::snapshot ss)
{
    memory->restore_snapshot(ss.mem_ss);
    builder->SetInsertPoint(ss.bb);
    PC = ss.pc;
    VPC = ss.vpc;
    VJMP = ss.vjmp;
    PATH = std::move(ss.path);
}

remill::Register* vexa::cpu::get_register(vexa::reg_t r)
{
    VEXA_ASSERT(registers.count(r));
    return registers[r];
}

vexa::reg_t vexa::cpu::str2reg(std::string r)
{
    VEXA_ASSERT(str_to_reg.count(r));
    return str_to_reg[r];
}

void vexa::cpu::initialize_arch()
{
    // initialize registers
    //
    auto callback = [&](const remill::Register* r)
    {
        vexa::reg_t reg = str2reg(r->name);
        registers[reg] = (remill::Register*)r;
    };
    arch->ForEachRegister(callback);

    // load semantics and get llvm module
    //
    std::unique_ptr<llvm::Module> module = remill::LoadArchSemantics(arch.get());
    VEXA_ASSERT(module);

    global_memory = memory->allocate();
    context->llvm_module = std::move(module);
    emulate = std::make_shared<class vexa::cpu::emulator>(this);

    // init function and environment
    //
    auto m = context->llvm_module.get();
    vexa_lifted = arch->DeclareLiftedFunction("vexa_lifted", m);
    arch->InitializeEmptyLiftedFunction(vexa_lifted);
    VEXA_ASSERT(vexa_lifted);
    dec_context = arch->CreateInitialContext();

    // function arguments
    //
    auto args = vexa_lifted->arg_begin();
    state_ptr = &*args++;
    pc_arg = &*args++;
    mem_ptr = &*args++;

    // create and set expressions for arguments
    //
    symex->set(state_ptr, symex->pointer(symex->concrete(0, 64), memory->allocate()));
    symex->set(pc_arg, symex->concrete(0, 64));
    symex->set(mem_ptr, symex->pointer(symex->concrete(0, 64), global_memory));

    // load all registers in entry block for once, so they're not loaded when needed during lifting
    //
    auto init_registers = [&](const remill::Register* r) {
        r->AddressOf(state_ptr, &vexa_lifted->front());
        if (!r->parent)
            memory->write(symex->pointer(symex->concrete(r->offset, 64), VEXA_SYM_PTR(state_ptr)->get_page()),
                          symex->symbolic(r->name, r->size * 8));
    };
    arch->ForEachRegister(init_registers);

    // initialize lifter
    //
    intrinsics.emplace(context->llvm_module.get());
    lifter.emplace(arch.get(), *intrinsics);
    builder->set_function(vexa_lifted);
    builder->SetInsertPoint(&vexa_lifted->front());

    // replace remill intrinsics with llvm instructions
    //
    replace_remill_intrinsics();
}

vexa::value* vexa::cpu::read_register(vexa::reg_t r)
{
    auto ptr = symex->pointer(symex->concrete(registers[r]->offset, 64), VEXA_SYM_PTR(state_ptr)->get_page());
    return memory->read(ptr, registers[r]->size * 8);
}

void vexa::cpu::write_register(vexa::reg_t r, vexa::value* val)
{
    auto ptr = symex->pointer(symex->concrete(registers[r]->offset, 64), VEXA_SYM_PTR(state_ptr)->get_page());
    memory->write(ptr, val);
}

void vexa::cpu::run(uint64_t pc)
{
    // initialize stack
    //      %stack = alloca i8, i64 8192
    //
    std::shared_ptr<mem_page> stack_page = memory->allocate();
    llvm::AllocaInst* stack = builder->CreateAlloca(builder->getInt8Ty(), builder->getIntN(64, 8192), "stack");
    symex->set(stack, symex->pointer(symex->concrete(0, 64), stack_page));

    // stack pointer
    //      %stack_ptr = getelementptr inbounds i8, ptr %stack, i64 4096
    //
    llvm::Value* stack_ptr_l = builder->CreateInBoundsGEP(builder->getInt8Ty(), stack, builder->getIntN(64, 4096), "stack_ptr");
    vexa::pointer* stack_ptr_v = symex->pointer(4096, stack_page);
    symex->set(stack_ptr_l, stack_ptr_v);
    stack_ptr = {stack_ptr_l, stack_ptr_v};

    // concretize stack pointer
    //
    llvm::Value* sp = get_register(vexa::amd64::SP)->AddressOf(state_ptr, *builder);
    builder->CreateStore(builder->getIntN(64, -8), sp);

    // init return address as zero, when function ends with an indirect jump, this value will be read
    //
    auto* s = builder->CreateInBoundsGEP(builder->getInt8Ty(),
                                         stack_ptr.l,
                                         builder->getIntN(64, -8));
    builder->CreateStore(builder->getIntN(64, 0), s);

    // execute the entry block, then we are ready for lifting
    //
    emulate->run_block(&vexa_lifted->front());
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

    PC = pc;
    mode = (mode_t)context->get_option(vexa::option::MODE);

    while (true)
    {
        internal_lifter_status status = process_instruction();
        switch (status)
        {
        case internal_lifter_status::function_ended:
            LOG_INFO(logger, "Reached the end of the function");
            builder->CreateRet(state_ptr);
            [[fallthrough]];
        case internal_lifter_status::explore_other_paths:
        {
            if (unexplored_paths.empty())
            {
                LOG_INFO(logger, "No more paths to explore");
                goto ret; // finish execution
            }

            LOG_INFO(logger, "Exploring new path");

            snapshot path = unexplored_paths.top();
            unexplored_paths.pop();
            restore_snapshot(path);

            [[fallthrough]];
        }
        case internal_lifter_status::successful:
        default:
            break;
        }
    }

ret:

    // clear all metadata
    //
    for (auto& BB : *vexa_lifted)
    {
        for (auto& I : BB) {
            llvm::SmallVector<std::pair<unsigned, llvm::MDNode*>, 8> MDs;
            I.getAllMetadataOtherThanDebugLoc(MDs);

            for (auto& MD : MDs) {
                I.setMetadata(MD.first, nullptr);
            }

            I.setDebugLoc(llvm::DebugLoc());
        }
    }

    builder->eraseDeletedInstructions();
    return;
}

bool vexa::cpu::CFG_recovery(vexa::mode_t mode)
{
    // CFG RECOVERY
    //
    if (mode == mode_t::CFG_RECOVERY)
    {
        if (context->get_option(option::CFG_JOIN_POLICY) == cfg_join_policy_t::SPECIALIZE_BY_PATH)
        {
            auto lifted = PATH.find(VPC);
            if (lifted != PATH.end())
            {
                builder->CreateBr(lifted->second);
                VJMP = false;
                return true;
            }
        }
        else if (CFG.contains(PC))
        {
            builder->CreateBr(CFG[PC]);
            return true;
        }
    }
    // VCFG RECOVERY
    //
    else if (mode == mode_t::VCFG_RECOVERY)
    {
        if (!VJMP)
            return false;

        if (context->get_option(option::CFG_JOIN_POLICY) == cfg_join_policy_t::SPECIALIZE_BY_PATH)
        {
            auto lifted = PATH.find(VPC);
            if (lifted != PATH.end())
            {
                builder->CreateBr(lifted->second);
                VJMP = false;
                return true;
            }
        }
        else if (VCFG.contains(VPC))
        {
            builder->CreateBr(VCFG[VPC]);
            VJMP = false;
            return true;
        }

        return VJMP = false;
    }

    return false;
}

vexa::cpu::internal_lifter_status vexa::cpu::process_instruction()
{
    if (PC == 0)
        return internal_lifter_status::function_ended;

    // build the CFG
    //
    if (CFG_recovery(mode))
    {
        LOG_INFO(logger, "Control flow edge rebuilt");
        return internal_lifter_status::explore_other_paths;
    }

    std::string bytes_str;
    bytes_str.reserve(15);

    // read 15 bytes at program counter
    // custom memory access for performance
    //
    auto& cm = global_memory->concrete_memory;
    for (int i = 0; i < 15; i++)
    {
        auto it = cm.find(PC + i);
        if (it == cm.end() || !it->second.has_value()) break;
        const z3::expr& byte_expr = (*it->second);
        if (!byte_expr.is_numeral()) break;
        bytes_str.push_back(static_cast<char>(byte_expr.get_numeral_uint64() & 0xFF));
    }

    // disassemble instruction
    //
    remill::Instruction inst;
    if (!arch->DecodeInstruction(PC, bytes_str, inst, dec_context)) {
        LOG_WARNING(logger, "Disassemble fail");
        return internal_lifter_status::explore_other_paths;
    }
    instruction = inst;

    // lift instruction
    //
    LOG_DEBUG(logger, "{:x} {}", PC, inst.disassembly);
    vexa::cpu::internal_lifter_status status = lift_instruction(inst);
    lifted_count++;

    return status;
}

vexa::cpu::internal_lifter_status vexa::cpu::lift_instruction(remill::Instruction inst)
{
    // create basic block for every instruction
    //
    block = builder->basic_block(inst.function + "_");
    builder->CreateBr(block);
    builder->SetInsertPoint(block);

    EVENT_HANDLER(event_kind::INSTRUCTION_LIFT);

    // lift instruction using remill, inline the semantics
    //
    VEXA_EXEC(builder->CreateStore(builder->getIntN(64, PC), next_pc.l));
    const auto status = lifter->LiftIntoBlock(inst, block, state_ptr);
    if (status != remill::kLiftedInstruction)
    {
        LOG_ERROR(
            logger,
            "Lifting failed for instruction -> {} : {}",
            inst.disassembly,
            static_cast<int>(status)
        );

        builder->CreateUnreachable();
        PC = -1;
        return internal_lifter_status::explore_other_paths;
    }

    // emulate the lifted block in z3
    //
    emulate->run_block(block);

    // save the lifted block
    //
    if (mode == mode_t::CFG_RECOVERY)
    {
        if (context->get_option(option::CFG_JOIN_POLICY) == cfg_join_policy_t::SPECIALIZE_BY_PATH)
            PATH[PC] = block;
        else
            CFG[PC] = block;
    }
    else if (mode == mode_t::VCFG_RECOVERY)
    {
        if (context->get_option(option::CFG_JOIN_POLICY) == cfg_join_policy_t::SPECIALIZE_BY_PATH)
            PATH.try_emplace(VPC, block);
        else
            VCFG.try_emplace(VPC, block);
    }

    // control flow
    //
    if (inst.IsControlFlow())
    {
        // CONDITIONAL
        if (inst.IsConditionalBranch())
        {
            vexa::dual_value cond = get_condition(block);
            branching(cond, inst.branch_taken_pc, inst.branch_not_taken_pc);
            return internal_lifter_status::successful;
        }

        // DIRECT JUMP
        if (inst.IsDirectControlFlow())
        {
            PC = inst.branch_taken_pc;
            EVENT_HANDLER(event_kind::DIRECT_JUMP);
            return internal_lifter_status::successful;
        }

        // INDIRECT JUMP
        vexa::value* next = get_next_pc(block).v->simplify();
        if (next->is_concrete())
        {
            PC = next->as_uint64();
            EVENT_HANDLER(event_kind::INDIRECT_JUMP);
            return internal_lifter_status::successful;
        }

        LOG_WARNING(logger, "{}", next->as_expr().to_string());
        LOG_ERROR(logger, "Unresolved indirect jump");
        return internal_lifter_status::explore_other_paths;

        //THROW("Unresolved jump");
    }
    // custom handling of cmov semantics
    //
    else if (inst.function.starts_with("CMOV"))
        handle_conditional_moves(inst, block);

    PC = inst.next_pc;
    return internal_lifter_status::successful;
}

void vexa::cpu::branching(vexa::dual_value condition, uint64_t jump_pc, uint64_t fallthrough_pc)
{
    if (context->get_option(vexa::option::OPAQUE_SOLVING))
    {
        bool taken;
        if (opaque_solver(condition.v, taken))
        {
            // opaque predicate solved
            // treat this as a direct branch

            if (taken)
            {
                // OPAQUE TAKEN
                PC = jump_pc;
                EVENT_HANDLER(event_kind::CONDITIONAL_TAKEN);
            }
            else
            {
                // OPAQUE NOT TAKEN
                PC = fallthrough_pc;
                EVENT_HANDLER(event_kind::CONDITIONAL_FALLTHROUGH);
            }

            return;
        }
    }

    // PATH FORKING
    //
    llvm::BasicBlock *jump_bb = builder->basic_block(utils::addr_to_str(jump_pc));
    llvm::BasicBlock *fallthrough_bb = builder->basic_block(utils::addr_to_str(fallthrough_pc));

    snapshot path_s = take_snapshot(fallthrough_pc, fallthrough_bb);
    unexplored_paths.push(path_s);

    builder->CreateCondBr(condition.l, jump_bb, fallthrough_bb);
    builder->SetInsertPoint(jump_bb);

    LOG_INFO(logger, "Path forking");
    PC = jump_pc;

    EVENT_HANDLER(vexa::event_kind::PATH_FORKING);
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

    llvm::StoreInst* store = nullptr;
    llvm::Instruction* and_inst = nullptr;
    llvm::SelectInst* select = nullptr;

    // search for store and select instructions in basic block
    //
    for (auto it = block->rbegin(), end = block->rend(); it != end; ++it)
    {
        auto* candidate = llvm::dyn_cast<llvm::StoreInst>(&*it);
        if (!candidate)
            continue;

        llvm::Value* stored_value = candidate->getValueOperand();
        if (auto* direct_select = llvm::dyn_cast<llvm::SelectInst>(stored_value))
        {
            store = candidate;
            select = direct_select;
            break;
        }

        auto* binary = llvm::dyn_cast<llvm::BinaryOperator>(stored_value);
        if (!binary || binary->getOpcode() != llvm::Instruction::And)
            continue;

        auto* masked_select =
            llvm::dyn_cast<llvm::SelectInst>(binary->getOperand(0));
        if (!masked_select)
            continue;

        store = candidate;
        and_inst = binary;
        select = masked_select;
        break;
    }

    VEXA_ASSERT(store);
    VEXA_ASSERT(select);

    /*
    CMOVO_GPRv_GPRv_32_:                              ; preds = %PUSH_GPRv_50_64_3
        store i64 5369515651, ptr %NEXT_PC, align 8
        %66 = load i64, ptr %NEXT_PC, align 8
        store i64 %66, ptr %PC, align 8
        %67 = add i64 %66, 4
        store i64 %67, ptr %NEXT_PC, align 8
        %68 = load i32, ptr %R9D, align 4
        %69 = zext i32 %68 to i64
        %70 = load ptr, ptr %MEMORY, align 8
        %71 = getelementptr inbounds nuw i8, ptr %state, i64 2077
        %72 = load i8, ptr %71, align 1, !tbaa !133
        %73 = icmp eq i8 %72, 0
        %74 = load i64, ptr %R8, align 8, !tbaa !130
        %75 = select i1 %73, i64 %74, i64 %69
        %76 = and i64 %75, 4294967295    <-----
        store i64 %76, ptr %R8, align 8, !tbaa !130
    */

    llvm::Value* store_ptr = store->getPointerOperand();
    vexa::value* cond_val = VEXA_SYM_VAL(select->getCondition());

    llvm::Value* true_val = select->getTrueValue();
    llvm::Value* false_val = select->getFalseValue();
    if (and_inst)
    {
        llvm::Value* true_and = builder->CreateAnd(true_val, and_inst->getOperand(1));
        true_val = VEXA_EXEC(true_and).l;

        llvm::Value* false_and = builder->CreateAnd(false_val, and_inst->getOperand(1));
        false_val = VEXA_EXEC(false_and).l;
    }

    // opaque predicate solving in conditional moves
    // disabled in vcfg recovery mode (EXPERIMENTAL!)
    //
    if (context->get_option(vexa::option::OPAQUE_SOLVING) && mode != mode_t::VCFG_RECOVERY)
    {
        bool result;
        if (opaque_solver(cond_val, result))
        {
            builder->deleteLater(store);

            if (result)
                VEXA_EXEC(builder->CreateStore(true_val, store_ptr));
            else
                VEXA_EXEC(builder->CreateStore(false_val, store_ptr));

            return;
        }
    }

    // path forking
    vexa::cpu::snapshot base_snapshot = take_snapshot(PC, block);
    llvm::BasicBlock* true_block = builder->basic_block();
    llvm::BasicBlock* false_block = builder->basic_block();
    builder->CreateCondBr(select->getCondition(), true_block, false_block);

    // false path
    builder->SetInsertPoint(false_block);
    VEXA_EXEC(builder->CreateStore(false_val, store_ptr));
    vexa::cpu::snapshot false_path = take_snapshot(inst.next_pc, false_block);
    unexplored_paths.push(false_path);

    // true path
    restore_snapshot(base_snapshot);
    builder->SetInsertPoint(true_block);
    VEXA_EXEC(builder->CreateStore(true_val, store_ptr));

    builder->deleteLater(store);
    LOG_INFO(logger, "Path fork by CMOVxx");
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
                vexa::dual_value val = VEXA_EXEC(builder->CreateTrunc(store->getValueOperand(), builder->getInt1Ty()));
                return val;
            }
        }
    }

    THROW("Couldn't get condition");
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
                return vexa::dual_value(val, VEXA_SYM_VAL(val));
            }
        }
    }

    THROW("Couldn't get next pc");
}


vexa::dual_value vexa::cpu::value_to_pointer(llvm::Value* addr)
{
    // save insert point
    builder->push_ip();

    // get value and simplify
    vexa::value* v = VEXA_SYM_VAL(addr);
    v->simplify();

    vexa::dual_value ptr;
    if (v->is_concrete())
    {
        // if its lower than 0, we take it as a stack access
        // TODO: implement a better stack access detection method
        int64_t signed_addr = static_cast<int64_t>(v->as_uint64());
        if ((signed_addr <= 0 && signed_addr >= -8192)) // || (signed_addr > 5000 && signed_addr < 6500)
        {
            // stack access
            ptr = builder->inbounds_gep(builder->getInt8Ty(), stack_ptr.l, addr);
        }
        else if ((signed_addr > 0 && signed_addr < 100000) && false)
        {
            // experimental
            //
            ptr = builder->inbounds_gep(builder->getInt8Ty(), mem_ptr, addr);
            ptr.l = builder->CreateInBoundsGEP(builder->getInt8Ty(), stack_ptr.l, addr);
            symex->set(ptr.l, ptr.v);
        }
        else
        {
            ptr = builder->inbounds_gep(builder->getInt8Ty(), mem_ptr, addr);
        }
    }
    else
    {
        ptr = builder->inttoptr(addr, builder->getInt8Ty(), global_memory);
    }

ret:
    // restore insert point
    builder->pop_ip();
    return ptr;
}

bool vexa::cpu::opaque_solver(vexa::value* condition, bool &result)
{
    // basic opaque predicate detection
    condition->simplify();
    if (condition->is_concrete())
    {
        uint64_t is_taken = condition->as_uint64();
        if (is_taken)
        {
            result = true;
            return true;
        }
        else
        {
            result = false;
            return true;
        }
    }

    // init solver
    z3::context &c = *context->z3_context;
    z3::solver solver(c);
    z3::expr cond = condition->as_expr();

    // check if cond can be false
    solver.add(cond != 1);
    if (solver.check() == z3::unsat)
    {
        // means it cant be false
        // we solved this branch is always taken
        result = true;
        return true;
    }

    // check if cond can be true
    solver.reset();
    solver.add(cond != 0);
    if (solver.check() == z3::unsat)
    {
        // means it cant be true
        // we solved this branch is never taken
        result = false;
        return true;
    }

    // condition is not an opaque predicate
    return false;
}

vexa::value* vexa::cpu::stack_access(uint64_t offset)
{
    vexa::value* sp = stack_ptr.v;
    int64_t addr = static_cast<int64_t>(sp->as_uint64() + offset);
    auto page = vexa::to_ptr(stack_ptr.v)->get_page();
    return symex->pointer(addr, page);
}

vexa::cpu::resolved_path_t vexa::cpu::resolve_path(vexa::value* v)
{
    v->simplify();

    z3::expr v_expr = v->as_expr();
    if (v_expr.is_app() && v_expr.decl().decl_kind() == Z3_OP_ITE)
    {
        z3::expr cond = v_expr.arg(0);
        z3::expr then_expr = v_expr.arg(1);
        z3::expr else_expr = v_expr.arg(2);

        if (then_expr.is_numeral() && else_expr.is_numeral())
            return vexa::cpu::resolved_path_t(
                       then_expr.as_uint64(),
                       else_expr.as_uint64());
    }

    LOG_DEBUG(logger,"{}", v_expr.to_string());
    THROW("failed to resolve indirect jump");
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