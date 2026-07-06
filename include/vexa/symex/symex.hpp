#pragma once
#include "../context.hpp"
#include "../value/value.hpp"

#include <llvm/IR/ValueMap.h>
#include <map>

namespace vexa {
class symex
{
public:
    symex(vexa::context* _context);
    vexa::shared_value symbolic(std::string name, int size);
    vexa::shared_value concrete(uint64_t val, int size);
    vexa::shared_pointer pointer(vexa::shared_value value, std::shared_ptr<vexa::mem_page> page);
    vexa::shared_value value(z3::expr e);
    vexa::shared_value get(llvm::Value* v);
    void set(llvm::Value* v, vexa::shared_value e);
    bool is_sync(llvm::Value* v);
private:
    vexa::context* context;
    std::unordered_map<llvm::Value*, vexa::shared_value> vars;
    //llvm::ValueMap<llvm::Value*, vexa::shared_value> vars;
};
}
