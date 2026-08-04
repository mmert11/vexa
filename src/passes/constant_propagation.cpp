#include <vexa/vexa.h>
#include <vexa/passes/constant_propagation.hpp>
#include <deque>
#include <unordered_set>
#include <vector>

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Dominators.h>

// maintaining this pass was too difficult and draining for me
// so i let the ai handle it all, bugs and edge-cases are expected
// you can find the legacy version here
// https://github.com/mmert11/vexa/blob/1.2/src/passes/constant_propagation.cpp

bool vexa::passes::constant_propagation::
has_only_defined_constant_operands(llvm::Instruction *instruction)
{
    for (llvm::Use &operand : instruction->operands())
    {
        llvm::Value *value = operand.get();
        if (!llvm::isa<llvm::Constant>(value) ||
            llvm::isa<llvm::UndefValue>(value) ||
            llvm::isa<llvm::PoisonValue>(value))
        {
            return false;
        }
    }
    return true;
}

bool vexa::passes::constant_propagation::is_ssa_fold_candidate(
    llvm::Instruction *instruction,
    llvm::LoopInfo &LI,
    const std::unordered_set<llvm::BasicBlock *> &joined_blocks)
{
    if (!instruction->getParent() ||
        llvm::isa<llvm::LoadInst>(instruction) ||
        llvm::isa<llvm::StoreInst>(instruction) ||
        llvm::isa<llvm::PHINode>(instruction) ||
        instruction->isTerminator() ||
        instruction->mayHaveSideEffects() ||
        !instruction->getType()->isIntegerTy() ||
        LI.getLoopFor(instruction->getParent()) ||
        joined_blocks.count(instruction->getParent()) ||
        !has_only_defined_constant_operands(instruction))
    {
        return false;
    }

    auto *operation = llvm::dyn_cast<llvm::Operator>(instruction);
    return operation &&
           !llvm::canCreateUndefOrPoison(operation) &&
           llvm::isSafeToSpeculativelyExecute(instruction) &&
           symex->is_sync(instruction);
}

std::vector<llvm::Instruction *>
vexa::passes::constant_propagation::collect_instruction_users(
    llvm::Instruction *instruction)
{
    std::vector<llvm::Instruction *> users;
    users.reserve(instruction->getNumUses());
    for (llvm::User *user : instruction->users())
    {
        if (auto *user_instruction =
                llvm::dyn_cast<llvm::Instruction>(user))
        {
            users.push_back(user_instruction);
        }
    }
    return users;
}

std::vector<llvm::Instruction *>
vexa::passes::constant_propagation::propagate_ssa_values(
    llvm::Function *function,
    llvm::LoopInfo &LI,
    value_simplifier &simplify_once,
    const std::vector<llvm::Instruction *> &seeds)
{

    std::unordered_set<llvm::BasicBlock *> joined_blocks;
    for (llvm::BasicBlock &block : *function)
    {
        if (llvm::pred_size(&block) > 1)
            joined_blocks.insert(&block);
    }

    std::vector<llvm::Instruction *> dead_values;
    std::deque<llvm::Instruction *> worklist;
    std::unordered_set<llvm::Instruction *> queued;
    std::unordered_set<llvm::Instruction *> folded;

    auto enqueue = [&](llvm::Instruction *instruction)
    {
        if (!folded.count(instruction) && queued.insert(instruction).second)
            worklist.push_back(instruction);
    };

    for (llvm::Instruction *seed : seeds)
        enqueue(seed);

    while (!worklist.empty())
    {
        llvm::Instruction *instruction = worklist.front();
        worklist.pop_front();
        queued.erase(instruction);

        if (folded.count(instruction) ||
            !is_ssa_fold_candidate(instruction, LI, joined_blocks))
        {
            continue;
        }

        vexa::value *value = symex->get(instruction);
        if (vexa::dyn_cast<vexa::pointer>(value))
            continue;

        value = simplify_once(value);
        if (!value->is_concrete() ||
            value->size() != instruction->getType()->getIntegerBitWidth())
        {
            continue;
        }

        llvm::Constant *constant =
            builder->getIntN(value->size(), value->as_uint64());
        std::vector<llvm::Instruction *> users =
            collect_instruction_users(instruction);

        folded.insert(instruction);
        instruction->replaceAllUsesWith(constant);
        dead_values.push_back(instruction);
        for (llvm::Instruction *user : users)
            enqueue(user);
    }

    return dead_values;
}

void vexa::passes::constant_propagation::erase_dead_instructions(
    const std::vector<llvm::Instruction *> &instructions)
{
    for (auto it = instructions.rbegin(); it != instructions.rend(); ++it)
    {
        if ((*it)->use_empty())
            (*it)->eraseFromParent();
    }
}


bool vexa::passes::constant_propagation::run(llvm::Function *function)
{
    llvm::DominatorTree DT(*function);
    llvm::LoopInfo LI;
    LI.analyze(DT);

    value_simplifier simplify_once;
    memory_propagation_result memory =
        concretize_memory_loads(function, DT, LI, simplify_once);

    std::vector<llvm::Instruction *> dead_ssa_values =
        propagate_ssa_values(
            function, LI, simplify_once, memory.fold_seeds);

    erase_dead_instructions(dead_ssa_values);
    erase_dead_instructions(memory.dead_loads);

    LOG_INFO(logger, "Concretized memory loads -> {}",
             memory.dead_loads.size());
    LOG_INFO(logger, "Concretized SSA values -> {}",
             dead_ssa_values.size());
    builder->eraseDeletedInstructions();
    return true;
}
