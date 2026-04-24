#include <vexa/vexa.h>

vexa::snapshot vexa::cpu::take_snapshot()
{
    return vexa::snapshot{cpu_state{registers}, memory->take_snapshot(), p_manager.lifted_blocks};
}

void vexa::cpu::restore_snapshot(vexa::snapshot ss)
{
    registers = ss.cpu_ss.registers;
    memory->restore_snapshot(ss.mem_ss);
    p_manager.lifted_blocks = ss.lifted_blocks;
}

z3::expr vexa::cpu::bv_bool(bool v)
{
    return context->z3_context->bool_val(v);
}

void vexa::path_manager::handle_loops(uint64_t addr)
{
    if (lifted_blocks.count(addr))
    {
        uint64_t ip = cpu->_read_register(x64::RIP).as_uint64();
        /*
        auto start_it = lifted_blocks.find_iterator(addr);
        auto end_it = lifted_blocks.find_iterator(ip);
        if (start_it != lifted_blocks.end() && end_it != lifted_blocks.end())
            for (auto it = start_it; it != lifted_blocks.end(); ++it)
            {
                uloop_backedges.insert(it->second);
                if (it == end_it)
                    break;
            }
        */

        // means this is a loop backedge
        // erase the trace that from the target addr to current rip
        // so we can execute the same path again and unroll the loop
        lifted_blocks.erase_range(addr, ip);
        builder->call((llvm::Function*)cpu->context->MarkerFunc.getCallee());
    }
}

vexa::value vexa::path_manager::branching(vexa::value condition, vexa::value true_addr, vexa::value false_addr)
{
    if (condition.as_expr().is_numeral()) // opaque predicates solving
    {
        if (condition.as_uint64() == true) // check if we will branch or not
        {
            handle_loops(true_addr.as_uint64());
            // return the true address to create a direct branching to it
            return true_addr;
        }
        else
        {
            handle_loops(false_addr.as_uint64());
            // return the false address to create a direct branching to it
            return false_addr;
        }
    }

    uint64_t ip = cpu->_read_register(x64::RIP).as_uint64();
    std::cout << "[cpu] forking path" << std::endl;

    // create basic blocks for both paths
    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(ip));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(false_addr.as_uint64()));

    // take snapshot and save the path state
    path_state path_s{cpu->take_snapshot(), false_addr, else_bb, current_vip};
    unexplored_paths.push(path_s);

    // create branching
    builder->jump_if(condition, then_bb, else_bb);
    builder->set_ip(then_bb);
    return true_addr;
}

vexa::value vexa::path_manager::branching(vexa::value addr)
{
    handle_loops(addr.as_uint64());
    return addr;
}

vexa::resolved_path_t vexa::path_manager::resolve_path(vexa::value v)
{
    z3::expr v_expr = v.as_expr();
    if (v_expr.is_app() && v_expr.decl().decl_kind() == Z3_OP_ITE)
    {
        z3::expr cond = v_expr.arg(0);
        z3::expr then_expr = v_expr.arg(1);
        z3::expr else_expr = v_expr.arg(2);

        if (then_expr.is_numeral() && else_expr.is_numeral())
            return vexa::resolved_path_t(
                       builder->get_const_int(then_expr.as_uint64(), 64),
                       builder->get_const_int(else_expr.as_uint64(), 64));
    }

    std::cout << v_expr << std::endl;
    THROW("failed to resolve indirect jump");
}