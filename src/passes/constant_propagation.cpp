#include <vexa/vexa.h>
#include <vexa/passes/constant_propagation.hpp>

#include <unordered_set>
#include <iostream>

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>

bool vexa::passes::constant_propagation::run(llvm::Function* func)
{
    llvm::Function* function = func;

    // run llvm's LoopInfo analysis
    llvm::DominatorTree DT(*function);
    llvm::LoopInfo LI;
    LI.analyze(DT);

    // Memory
    // we look for every concrete store instruction in loops
    // and mark them as in-loop writes
    std::unordered_set<uint64_t> loop_written_addrs;
    for (llvm::Loop *L : LI)                                        // iterate every loop
        for (llvm::BasicBlock *BB : L->getBlocks())                 // every block
            for (auto &I : *BB)                                     // every instruction
                if (auto *SI = llvm::dyn_cast<llvm::StoreInst>(&I)) // if instruction is a store
                {
                    // get the target address
                    llvm::Value *ptr = SI->getPointerOperand();
                    if (!symex->is_sync(ptr)) // do we have a symbolic expresion for it? if not, skip
                        continue;

                    if (L->isLoopInvariant(SI->getValueOperand()))
                        continue;

                    // get the expr
                    vexa::shared_value addr = symex->get(ptr);
                    addr->simplify();
                    if (addr->is_concrete()) // is it concrete?
                        // if yes, mark the address as in-loop variant
                        loop_written_addrs.insert(addr->as_uint64());
                }
    
    // we iterate every load instruction in function
    // and check if there are any loads that can be over-concretized
    // except for the ones that marked as in-loop writes
    // so we dont break the loops dataflow
    std::vector<llvm::Instruction *> dead_loads;
    for (auto &BB : *function)
    {
        for (auto &I : BB)
        {
            auto *LI_inst = llvm::dyn_cast<llvm::LoadInst>(&I);
            // if its not a load instruction, skip
            if (!LI_inst)
                continue;

            // if its not sync with symex, skip
            if (!symex->is_sync(LI_inst))
                continue;
            
            // get the target address of load instruction
            llvm::Value *ptr = LI_inst->getPointerOperand();
            if (symex->is_sync(ptr))
            {
                vexa::shared_value ptr_addr = symex->get(ptr);
                ptr_addr->simplify();
                if (!ptr_addr->is_concrete()) // if its not concrete, skip
                    continue;

                // if its written in loops before, skip
                if (loop_written_addrs.count(ptr_addr->as_uint64()))
                    continue;

                // if its an external memory access (global memory), skip
                if (ptr_addr->as_uint64() > 0)
                    continue;
            }

            // get the symbolic value of load and check if its concrete
            vexa::shared_value val = symex->get(LI_inst);
            val->simplify();
            if (!val->is_concrete())
                continue;

            // create it as a new constant in ir
            llvm::Value *C = builder->getIntN(val->size(), val->as_uint64());
            if (C->getType() != LI_inst->getType())
                continue;

            // and replace the load
            LI_inst->replaceAllUsesWith(C);
            dead_loads.push_back(LI_inst);
        }
    }

    // remove all optimized loads
    for (auto *I : dead_loads)
        if (I->use_empty())
            I->eraseFromParent();

    std::cout << "[analysis] over-concretized memory loads: " << std::dec << dead_loads.size() << std::endl;

    // SSA variables
    // mark all variables in loops as tainted
    // except for loop invariants
    std::unordered_set<llvm::Value *> loopTaintedVariants;
    for (llvm::Loop *L : LI)                            // every loop
        for (llvm::BasicBlock *loopBB : L->getBlocks()) // every block
            for (auto &I : *loopBB)                     // every instruction
            {
                if (L->isLoopInvariant(&I))
                    continue; // if its marked as loop-invariant by llvm, skip

                // mark it as tainted loop variable
                loopTaintedVariants.insert(&I);
            }

    // taint the load variables that access to tainted addresses
    // that detected by memory analysis up there
    for (auto &BB : *function)
        for (auto &I : BB)
            if (auto *LD = llvm::dyn_cast<llvm::LoadInst>(&I))
            {
                llvm::Value *ptr = LD->getPointerOperand();
                if (!symex->is_sync(ptr))
                    continue;

                vexa::shared_value addr = symex->get(ptr);
                addr->simplify();
                if (addr->is_concrete() && loop_written_addrs.count(addr->as_uint64()))
                    loopTaintedVariants.insert(LD);
            }

    // iterate all instructions in function
    // and check if any of the operands is tainted
    // if so, taint that instruction as well
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto &BB : *function)
            for (auto &I : BB)
            {
                if (loopTaintedVariants.count(&I)) // if its already tainted, skip
                    continue;
                    
                for (auto &op : I.operands()) // iterate operands
                    if (loopTaintedVariants.count(op.get()))
                    {
                        // if one of the operands is tainted, taint the instruction
                        // and repeat the process until there is no more instruction to be marked
                        if (loopTaintedVariants.insert(&I).second)
                            changed = true;
                        break;
                    }
            }
    }

    // iterate all instructions and eliminate the tainted ones
    // then check if we have a concrete expression for it in our symex module
    // if so, replace the instruction with new concretized constant
    std::vector<llvm::Instruction *> toDelete;
    for (llvm::BasicBlock &BB : *function)
    {
        for (auto &I : BB)
        {
            llvm::Value *lvalue = &I;
            if (llvm::isa<llvm::Constant>(lvalue) ||
                    llvm::isa<llvm::StoreInst>(lvalue) ||
                    lvalue->getType()->isPointerTy() ||
                    I.isTerminator())
                continue;

            if (loopTaintedVariants.count(lvalue))
                continue;

            // eliminate llvm.assume because it caused some type mismatch problems
            bool feeds_assume = false;
            for (auto *user : lvalue->users()) {
                if (auto *CI = llvm::dyn_cast<llvm::CallInst>(user)) {
                    auto *F = CI->getCalledFunction();
                    if (F && F->getName().str().starts_with("llvm.assume")) {
                        feeds_assume = true;
                        break;
                    }
                }
            }
            if (feeds_assume) continue;

            if (!symex->is_sync(lvalue))
                continue;

            vexa::shared_value expression = symex->get(lvalue);
            expression->simplify();
            if (!expression->is_concrete())
                continue;

            llvm::Value *concretized = builder->getIntN(expression->size(), expression->as_uint64());
            lvalue->replaceAllUsesWith(concretized);
            toDelete.push_back(&I);
        }
    }

    // remove the old instructions
    for (auto *inst : toDelete)
        if (inst->use_empty())
            inst->eraseFromParent();

    std::cout << "[analysis] over-concretized ssa variables: " << toDelete.size() << std::endl;
    return true;
}