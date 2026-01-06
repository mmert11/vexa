#pragma once
#include "../value/value.hpp"
#include <z3++.h>

#include <map>

namespace vexa
{
    class symex
    {
    public:
        symex(std::shared_ptr<vexa::context> _context);
        z3::expr symbolic(std::string name, int size);
        z3::expr concrete(uint64_t val, int size);
        z3::expr get(llvm::Value* v);
        void set(llvm::Value* v, z3::expr e); 
        bool is_sync(llvm::Value* v);
    private:
        std::shared_ptr<vexa::context> context;
        std::map<llvm::Value*, std::shared_ptr<z3::expr>> vars;
    };
}
