#pragma once
#include "../context.hpp"
#include "../value/value.hpp"

#include <llvm/IR/ValueMap.h>
#include <map>
#include <deque>

namespace vexa {
class symex
{
public:
    symex(vexa::context* _context);
    vexa::value* symbolic(std::string name, int size);
    vexa::value* concrete(uint64_t val, int size);
    vexa::pointer* pointer(vexa::value* value, std::shared_ptr<vexa::mem_page> page);
    vexa::pointer* pointer(uint64_t value, std::shared_ptr<vexa::mem_page> page);
    vexa::value* value(z3::expr e);
    vexa::value* get(llvm::Value* v);
    void set(llvm::Value* v, vexa::value* e);
    void clear();
    bool is_sync(llvm::Value* v);
private:
    // Keeps the Z3 context alive until every arena object has been destroyed.
    std::shared_ptr<z3::context> z3_context;
    vexa::context* context;
    std::deque<vexa::value> values;
    std::deque<vexa::pointer> pointers;
    std::unordered_map<llvm::Value*, vexa::value*> vars;
};
}
