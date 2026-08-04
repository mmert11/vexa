#include <vexa/vexa.h>

vexa::memory::memory() {}
vexa::memory::memory(vexa::context* _context) : context(_context)
{
}

std::shared_ptr<vexa::mem_page> vexa::memory::allocate(uint64_t size)
{
    std::shared_ptr<mem_page> new_page = std::make_shared<mem_page>(size);
    pages.push_back(new_page);
    return new_page;
}

void vexa::memory::write(vexa::pointer* addr, vexa::value* val)
{
    addr->simplify();
    return _write(addr, val);
}

vexa::value* vexa::memory::read(vexa::pointer* addr, int size)
{
    addr->simplify();
    return _read(addr, size);
}

// internal write function
void vexa::memory::_write(vexa::pointer* addr, vexa::value* val)
{
    z3::expr addr_expr = addr->as_expr();
    z3::expr val_expr = val->as_expr();
    int size = val_expr.get_sort().bv_size();

    auto& page = addr->get_page()->concrete_memory;

    if (addr_expr.is_numeral())
    {
        uint64_t base = addr->as_uint64();
        for (int i = 0; i < size / 8; i++)
        {
            page[base + i] = val_expr.extract(i * 8 + 7, i * 8).simplify();
        }
    }
    else
    {
        // TODO: handle symbolic writes
    }
}

// internal read function
vexa::value* vexa::memory::_read(vexa::pointer* addr, int size)
{
    z3::expr addr_expr = addr->as_expr();
    auto& concrete_memory = addr->get_page()->concrete_memory;

    if (addr_expr.is_numeral())
    {
        uint64_t const_addr = addr_expr.as_uint64() + (size / 8) - 1;
        z3::expr val_expr = concrete_memory.count(const_addr)
                            ? *concrete_memory[const_addr]
                            : context->z3_context->bv_const(("read_" + std::to_string(const_addr)).c_str(), 8);

        uint64_t base = addr->as_uint64();
        for (int i = (size / 8) - 2; i >= 0; i--)
        {
            auto it = concrete_memory.find(base + i);
            z3::expr v = (it != concrete_memory.end() && it->second.has_value())
                         ? *it->second
                         : context->z3_context->bv_const(("read_" + std::to_string(base + i)).c_str(), 8);
            val_expr = z3::concat(val_expr, v);
        }
        return context->symex->value(val_expr.simplify());
    }
    else
    {
        return context->symex->symbolic("read_" + std::to_string(Z3_get_ast_hash(*context->z3_context, addr_expr)), size);
    }
}

vexa::mem_state vexa::memory::take_snapshot()
{
    vexa::mem_state state;
    for (auto& page : pages)
        state.pages.push_back(*page);

    return state;
}

void vexa::memory::restore_snapshot(vexa::mem_state ss)
{
    unsigned int i;
    for (i = 0; i < ss.pages.size(); i++)
        *pages[i] = ss.pages[i];

    if (pages.size() > ss.pages.size())
        pages.erase(pages.begin() + ss.pages.size(), pages.end());
}
