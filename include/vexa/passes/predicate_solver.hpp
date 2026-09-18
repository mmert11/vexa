#pragma once
#include "../ir/pass_manager.hpp"
#include <unordered_set>

namespace vexa
{
namespace passes
{

// Solving and simplifying obfuscated conditions and predicates.

class predicate_solver : public vexa::ir::pass
{
  public:
    predicate_solver(vexa::context *ctx) : pass(ctx) {}
    bool run(llvm::Function *func) override;
    bool is_recursive() override { return true; }
  private:
    std::unordered_set<llvm::Value*> blacklist;
};

} // namespace passes
} // namespace vexa