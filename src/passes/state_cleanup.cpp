#include <vexa/vexa.h>
#include <vexa/passes/state_cleanup.hpp>

#include <queue>

bool vexa::passes::state_cleanup::run(llvm::Function* func)
{
    llvm::DataLayout DL = context->llvm_module->getDataLayout();
    bool changed_smth = false;

    for (auto &BB : llvm::make_early_inc_range(*func))
    {
        // collection of detected dead stores
        std::vector<llvm::StoreInst*> dead_stores;
        for (auto it = BB.rbegin(), end = BB.rend(); it != end; ++it)
        {
            // if its a store instruction
            if (auto *store_inst = llvm::dyn_cast<llvm::StoreInst>(&*it))
            {
                if (std::find(dead_stores.begin(), dead_stores.end(), store_inst) != dead_stores.end())
                    continue;

                llvm::GetElementPtrInst* store_PTR = llvm::dyn_cast<llvm::GetElementPtrInst>(store_inst->getPointerOperand());
                if (!store_PTR) continue;

                llvm::APInt store_offset(64, 0);
                if (!store_PTR->accumulateConstantOffset(DL, store_offset)) continue;

                // we check if the gep's first operand is our state pointer
                if (store_PTR->getOperand(0) == context->cpu->state_ptr.l &&
                        store_offset != context->cpu->get_return_register()->offset) // every register except for return register
                {
                    // now check if there is any load to same address after this store
                    bool is_alive = false;

                    std::queue<llvm::BasicBlock::iterator> queue;
                    std::unordered_set<llvm::BasicBlock*> visited;
                    queue.push(++store_inst->getIterator()); // add rest of the current block to queue too

                    // iterate all possible blocks that we can fall through
                    // and look for loads to the same address
                    while (!queue.empty())
                    {
                        llvm::BasicBlock::iterator iterator = queue.front();
                        queue.pop();
                        llvm::BasicBlock* block = iterator->getParent();
                        if (!visited.insert(block).second) continue; // if already visited

                        // iterate all intructions in the block, starting from the iterator
                        for (auto _it = iterator, _end = block->end(); _it != _end; _it++)
                        {
                            if (auto* over_store = llvm::dyn_cast<llvm::StoreInst>(_it))
                            {
                                llvm::GetElementPtrInst* overstore_PTR = llvm::dyn_cast<llvm::GetElementPtrInst>(over_store->getPointerOperand());
                                llvm::APInt overstore_offset(64, 0);

                                if (!overstore_PTR || !overstore_PTR->accumulateConstantOffset(DL, overstore_offset))
                                    continue;
                                
                                if (overstore_PTR->isSameOperationAs(store_PTR) && store_offset == overstore_offset)
                                    // this store is overwritten by another store
                                    break;
                            }
                            else if (auto* load_inst = llvm::dyn_cast<llvm::LoadInst>(_it))
                            {
                                llvm::GetElementPtrInst* load_PTR = llvm::dyn_cast<llvm::GetElementPtrInst>(load_inst->getPointerOperand());
                                llvm::APInt load_offset(64, 0);

                                if (!load_PTR || !load_PTR->accumulateConstantOffset(DL, load_offset)) continue;
                                if (load_PTR->isSameOperationAs(store_PTR) && store_offset == load_offset)
                                {
                                    // this store is not dead
                                    is_alive = true;
                                    break;
                                }
                            }
                        }

                        // add this block's successors to the queue
                        for (auto *succ : llvm::successors(block))
                            queue.push(succ->begin());
                    }

                    if (!is_alive)
                    {
                        dead_stores.push_back(store_inst);
                    }
                }
            }
        }

        // erase all dead stores
        for (auto *store : dead_stores)
        {
            store->eraseFromParent();
            changed_smth = true;
        }
    }

    return changed_smth;
}