#pragma once

#include "../context.hpp"
#include "../value/value.hpp"

#include <map>

namespace vexa {
struct mem_page
{
    mem_page(uint64_t size)
    {
        if (size > 0)
            concrete_memory.reserve(size);
    }
    std::unordered_map<uint64_t, std::optional<z3::expr>> concrete_memory;
};

struct mem_state
{
    std::vector<mem_page> pages;
};

class memory
{
public:
    memory();
    memory(vexa::context* _context);

    std::shared_ptr<mem_page> allocate(uint64_t size = 0);
    void write(vexa::shared_pointer addr, vexa::shared_value val);
    vexa::shared_value read(vexa::shared_pointer addr, int size);

    mem_state take_snapshot();
    void restore_snapshot(mem_state ss);

private:
    vexa::context* context;
    std::vector<std::shared_ptr<mem_page>> pages;

    void _write(vexa::shared_pointer addr, vexa::shared_value val);
    vexa::shared_value _read(vexa::shared_pointer addr, int size);
    std::pair<std::shared_ptr<z3::expr>, bool> get_if_written_before(z3::expr addr);
};
}