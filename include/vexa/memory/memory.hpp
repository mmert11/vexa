#pragma once

#include "../context.hpp"
#include "../value/value.hpp"

#include <map>

namespace vexa
{
    struct mem_state
    {
    public:
        std::unordered_map<std::shared_ptr<z3::expr>, std::shared_ptr<z3::expr>> symbolic_memory;
        std::unordered_map<uint64_t, std::shared_ptr<z3::expr>> concrete_memory;
    };

    class memory
    {
    public:
        memory();
        memory(std::shared_ptr<vexa::context> _context);
        std::pair<std::shared_ptr<z3::expr>, bool> get_if_written_before(z3::expr addr);
        void write(z3::expr addr, z3::expr val);
        z3::expr read(z3::expr addr, int size);

        mem_state take_snapshot();
        void restore_snapshot(mem_state ss);

    private:
        std::shared_ptr<vexa::context> context;
        std::unordered_map<std::shared_ptr<z3::expr>, std::shared_ptr<z3::expr>> symbolic_memory;
        std::unordered_map<uint64_t, std::shared_ptr<z3::expr>> concrete_memory;
    };
}