#pragma once
#include "../context.hpp"
#include "../value/value.hpp"

#include <deque>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/IR/ValueMap.h>

namespace vexa
{
class symex
{
  public:
    symex(vexa::context *_context);
    vexa::value *symbolic(std::string name, int size);
    vexa::value *concrete(uint64_t val, int size);
    vexa::pointer *pointer(vexa::value *value, std::shared_ptr<vexa::mem_page> page);
    vexa::pointer *pointer(uint64_t value, std::shared_ptr<vexa::mem_page> page);
    vexa::value *value(bw::Term e);
    vexa::value *get(llvm::Value *v);
    void set(llvm::Value *v, vexa::value *e);
    void clear();
    bool is_sync(llvm::Value *v);
    
  private:
    bw::TermManager *term_manager;
    bw::Bitwuzla *solver;
    vexa::context *context;
    std::deque<vexa::value> values;
    std::deque<vexa::pointer> pointers;
    llvm::ValueMap<llvm::Value *, vexa::value *> vars;
};
} // namespace vexa
