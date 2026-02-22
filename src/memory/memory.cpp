#include <vexa/vexa.h>

vexa::memory::memory() {}
vexa::memory::memory(std::shared_ptr<vexa::context> _context) : context(_context)
{
    TRY()

    // initialize memory as z3::array(64, 8)
    //z3::sort mem_sort = context->z3_context->array_sort(context->z3_context->bv_sort(64), context->z3_context->bv_sort(8));
    //mem = std::make_shared<z3::expr>(context->z3_context->constant("memory", mem_sort));

    CATCH()
}

std::pair<std::shared_ptr<z3::expr>, bool> vexa::memory::get_if_written_before(z3::expr addr)
{
    z3::context& ctx = addr.ctx();
    z3::solver s(ctx);

    for (auto& it : symbolic_memory)
    {
        s.add(addr != *it.first);
        if (s.check() == z3::unsat)
            return {it.first, true};
    }

    return {std::make_shared<z3::expr>(addr), false};
}

void vexa::memory::write(z3::expr addr, z3::expr val) 
{
    TRY()
    int size = val.get_sort().bv_size();
    bool addr_is_concrete = addr.is_numeral();
    bool val_is_concrete = val.is_numeral();

    if (addr_is_concrete)
    {
        for (int i = 0; i < size / 8; i++)
        {
            z3::expr target_byte = val.extract(i * 8 + 7, i * 8);
            uint64_t target_addr = addr.as_uint64() + i;
            concrete_memory[target_addr] = std::make_shared<z3::expr>(target_byte.simplify());
        }
    }
    else
    {
        for (int i = 0; i < size / 8; i++)
        {
            z3::expr target_byte = val.extract(i * 8 + 7, i * 8);
            z3::expr target_addr = addr + context->z3_context->bv_val(i, 64);
            symbolic_memory[get_if_written_before(target_addr).first] = std::make_shared<z3::expr>(target_byte.simplify());
        }
    }

    CATCH()
}

z3::expr vexa::memory::read(z3::expr addr, int size)
{
    TRY()

    if (addr.is_numeral())
    {
        uint64_t const_addr = addr.as_uint64() + (size / 8) - 1;
        //std::cout << "read " << std::hex << const_addr << std::endl;

        z3::expr val_expr = concrete_memory.count(const_addr)
                            ? *concrete_memory[const_addr]
                            : context->z3_context->bv_const(("read_" + std::to_string(const_addr)).c_str(), 8);

        for (int i = (size / 8) - 2; i >= 0; i--)
        {
            z3::expr v = concrete_memory.count(addr.as_uint64() + i)
                            ? *concrete_memory[addr.as_uint64() + i]
                            : context->z3_context->bv_const(("read_" + std::to_string(addr.as_uint64() + i)).c_str(), 8);

            val_expr = z3::concat(val_expr, v);
        }
        return val_expr.simplify();
    }
    else
    {
        z3::expr calculated_address = addr + context->z3_context->bv_val(size / 8 - 1, 64);
        auto [address, flag] = get_if_written_before(calculated_address);

        if (flag)
        {
            std::shared_ptr<z3::expr> val_expr = symbolic_memory[address];
            for (int i = (size / 8) - 2; i >= 0; i--)
            {
                auto [next_address, next_flag] = get_if_written_before(addr + context->z3_context->bv_val(i, 64));
                z3::expr v = next_flag
                                ? *symbolic_memory[next_address]
                                : context->z3_context->bv_const("read_", 8);
                val_expr = std::make_shared<z3::expr>(z3::concat(*val_expr, v));
            }
            return val_expr->simplify();
        }
        else
        {
            z3::expr val_expr = context->z3_context->bv_const("read_", 8);
            for (int i = (size / 8) - 2; i >= 0; i--)
            {
                auto [next_address, next_flag] = get_if_written_before(addr + context->z3_context->bv_val(i, 64));
                z3::expr v = next_flag
                                ? *symbolic_memory[next_address]
                                : context->z3_context->bv_const("read_", 8);
                val_expr = z3::concat(val_expr, v);
            }
            return val_expr.simplify();
        }
    }

    CATCH()
}

vexa::mem_state vexa::memory::take_snapshot()
{
    return mem_state{symbolic_memory, concrete_memory};
}

void vexa::memory::restore_snapshot(vexa::mem_state ss)
{
    symbolic_memory = ss.symbolic_memory;
    concrete_memory = ss.concrete_memory;
}