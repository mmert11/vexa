#pragma once
#include "../ir/pass_manager.hpp"

#include <unordered_set>
#include <vector>

namespace llvm
{
class DominatorTree;
class BasicBlock;
class LoopInfo;
} // namespace llvm

namespace vexa
{
namespace passes
{

// Replacing the values which can be conretized with constants using Bitwuzla
class constant_propagation : public vexa::ir::pass
{
    class value_simplifier
    {
      public:
        vexa::value *operator()(vexa::value *value)
        {
            if (simplified.insert(value).second)
                value->simplify();
            return value;
        }

      private:
        std::unordered_set<vexa::value *> simplified;
    };

    struct memory_propagation_result
    {
        std::vector<llvm::Instruction *> dead_loads;
        std::vector<llvm::Instruction *> fold_seeds;
    };

    static bool has_only_defined_constant_operands(llvm::Instruction *instruction);
    bool is_ssa_fold_candidate(
        llvm::Instruction *instruction,
        llvm::LoopInfo &LI,
        const std::unordered_set<llvm::BasicBlock *> &joined_blocks);
    static std::vector<llvm::Instruction *>
        collect_instruction_users(llvm::Instruction *instruction);
    std::vector<llvm::Instruction *> propagate_ssa_values(
        llvm::Function *function,
        llvm::LoopInfo &LI,
        value_simplifier &simplify_once,
        const std::vector<llvm::Instruction *> &seeds);
    memory_propagation_result concretize_memory_loads(
        llvm::Function *function,
        llvm::DominatorTree &DT,
        llvm::LoopInfo &LI,
        value_simplifier &simplify_once);
    static void erase_dead_instructions(const std::vector<llvm::Instruction *> &instructions);

  public:
    constant_propagation(vexa::context *ctx) : pass(ctx) {}
    bool run(llvm::Function *func) override;
    bool is_recursive() override { return false; }
};

} // namespace passes
} // namespace vexa