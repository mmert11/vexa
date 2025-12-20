#pragma once

#include "../context.hpp"
#include "../value/value.hpp"

#include <map>

namespace vexa
{
    struct mem_state
    {
    public: z3::expr mem;
    };

    class memory
    {
    public:
        memory();
        memory(std::shared_ptr<vexa::context> _context);
        void write(z3::expr addr, z3::expr val);
        z3::expr read(z3::expr addr, int size);

        mem_state take_snapshot();
        void restore_snapshot(mem_state ss);

    private:
        std::shared_ptr<vexa::context> context;
        std::shared_ptr<z3::expr> mem;

        //std::map<llvm::Value*, z3::expr> vars;
    };
}
