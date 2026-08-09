#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/Transforms/Utils/ValueMapper.h>
#include <vexa/passes/loop_reroll.hpp>
#include <vexa/vexa.h>

// UNDER MAINTENANCE, DONT USE UNTIL ITS REWORKED !!

// basic loop reroll pass to simplify the unrolled loops
// it searches for vexa.unrolled.marker function calls in code to identify locations loop begins
// and starts pattern matching to detect the edges of the loop and iteration count
// then recreates the unrolled loop in a real loop structure
// supports just lineer loop bodies, not with branches
// its just a PoC i wanted to work on, so im not too confident about this pass
// it may fail on real life samples and edge cases

// instruction
struct instruction_t
{
    llvm::Instruction *value;
    size_t hash;

    bool operator==(const instruction_t &other)
    {
        return (value == other.value && hash == other.hash);
    }
};

instruction_t hash_instruction(llvm::Instruction *I)
{
    size_t h = 0;

    h = llvm::hash_combine(h, I->getOpcode());
    h = llvm::hash_combine(h, I->getType());

    for (unsigned i = 0; i < I->getNumOperands(); ++i) {
        llvm::Value *op = I->getOperand(i);

        if (auto *CI = llvm::dyn_cast<llvm::ConstantInt>(op)) {
            h = llvm::hash_combine(h, CI->getZExtValue());
        }
        else {
            h = llvm::hash_combine(h, 0xDEADC0DE);
        }
    }

    return {I, h};
}

size_t hash_instructions(std::vector<instruction_t> instructions)
{
    size_t hash = 0;
    for (unsigned int i = 0; i < instructions.size(); i++) {
        hash = llvm::hash_combine(hash, instructions[i].hash);
        hash = llvm::hash_combine(hash, i);
    }

    return hash;
}

// dataflow
struct dependency_t
{
    llvm::Value *dependency, *access_instr;
    unsigned int accessed_by_index;
    bool operator==(const dependency_t &other) const;
};

struct dataflow_t
{
    std::vector<dependency_t> dependencies;
    bool operator==(const dataflow_t &other) const { return dependencies == other.dependencies; }
};

struct block_t
{
    std::vector<instruction_t> instructions;
    dataflow_t dataflow;
};

struct unrolled_loop_t
{
    int iteration_count = 1;
    std::vector<instruction_t> body, entry;
    std::vector<block_t> blocks;
};

size_t hash_dependency(dependency_t dependency)
{
    size_t hash = 0;

    if (auto *I = llvm::dyn_cast_or_null<llvm::Instruction>(dependency.access_instr))
        hash = llvm::hash_combine(hash, hash_instruction(I).hash);

    hash = llvm::hash_combine(hash, dependency.accessed_by_index);
    return hash;
}

bool dependency_t::operator==(const dependency_t &other) const
{
    return hash_dependency(*this) == hash_dependency(other);
}

dataflow_t dataflow_analysis(std::vector<instruction_t> block_1)
{
    std::vector<dependency_t> dependencies;

    for (auto it = block_1.begin(); it != block_1.end(); ++it) {
        for (auto &op : it->value->operands()) {
            // only non-constants
            if (llvm::isa<llvm::Constant>(op))
                continue;

            bool found = false;
            for (auto &it_ : block_1) {
                if (it_.value == op) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                // !!!
                bool already_added = false;
                for (const auto &existing_dep : dependencies) {
                    if (existing_dep.dependency == op) {
                        already_added = true;
                        break;
                    }
                }

                if (!already_added) {
                    unsigned int index = std::distance(block_1.begin(), it);
                    dependencies.push_back({op, it->value, index});
                }
            }
        }
    }

    return {dependencies};
}

dataflow_t dependency_analysis(dataflow_t dataflow, std::vector<instruction_t> block)
{
    auto dependencies = dataflow.dependencies;
    std::erase_if(dependencies, [&](const dependency_t &dep) {
        bool found_in_prev_block = false;
        for (auto &I : block) {
            if (I.value == dep.dependency) {
                found_in_prev_block = true;
                break;
            }
        }
        return !found_in_prev_block;
    });

    return {dependencies};
}

dataflow_t phi_analysis(dataflow_t flow_1, dataflow_t flow_2)
{
    auto dependencies = flow_1.dependencies;
    std::erase_if(dependencies, [&](const dependency_t &dep) {
        bool found_in_prev_block = false;
        for (auto &D : flow_2.dependencies) {
            if (D.accessed_by_index == dep.accessed_by_index) {
                found_in_prev_block = true;
                break;
            }
        }
        return !found_in_prev_block;
    });

    return {dependencies};
}

bool vexa::passes::loop_reroll::run(llvm::Function *func)
{
    llvm::FunctionCallee MarkerFunc = context->MarkerFunc;

    for (auto &BB : *func) {
        for (auto it = BB.rbegin(), end = BB.rend(); it != end; ++it) {
            llvm::Instruction &I = *it;
            if (I.isTerminator() || it == BB.rbegin() || it == BB.rend())
                continue;

            llvm::CallInst *call_inst = llvm::dyn_cast<llvm::CallInst>(&I);
            // try to find the unrolled loop marker function call, this saves us from pattern
            // scanning the whole code
            if (!call_inst || call_inst->getCalledFunction() != MarkerFunc.getCallee())
                continue;

            if (llvm::CallInst *C = llvm::dyn_cast<llvm::CallInst>(call_inst->getPrevNode()))
                if (C->getCalledFunction() == MarkerFunc.getCallee())
                    continue;

            // we have found a marker, means there is an unrolled loop here, lets search for it's
            // pattern
            unrolled_loop_t loop;
            dataflow_t dataflow;

            std::vector<instruction_t> &search_for = loop.body;
            search_for.push_back(hash_instruction(call_inst));

            // now look for similar patterns recursively in next instructions in the basic block
            for (auto it_ = std::next(it); it_ != end;) {
                size_t search_hash = hash_instructions(search_for);
                std::vector<instruction_t> instructions;

                // intruction limit
                if (search_for.size() > 512) {
                    break;
                }

                // gather the next sequence of instructions
                auto it__ = it_;
                for (unsigned int i = 0; i < search_for.size(); i++) {
                    if (it__ == end)
                        break;

                    auto instruction = &*it__;
                    instructions.push_back(hash_instruction(instruction));
                    it__++;
                }

                size_t hash = hash_instructions(instructions);
                dataflow = dataflow_analysis(search_for);
                dataflow_t inst_dataflow = dataflow_analysis(instructions);

                // does it match with the pattern we look for? (heuristic detection improved with
                // dataflow analysis)
                if (search_hash == hash && dataflow == inst_dataflow) {
                    if (loop.blocks.empty())
                        loop.blocks.push_back({search_for, dataflow});

                    loop.iteration_count++;
                    loop.body = instructions;
                    std::advance(it_, instructions.size()); // !!!
                    loop.blocks.push_back({instructions, inst_dataflow});
                }
                else {
                    if (loop.iteration_count > 3) {
                        // we already found a loop, but pattern doesnt match anymore
                        // means we came to the entry of the unrolled loop (first iteration)
                        // first iteration is structurly different from loop body usually
                        // so we extract the entry at where it starts to unmatch with our pattern

                        for (uint32_t i = 0; i < instructions.size(); i++)
                            if (instructions[i].hash != search_for[i].hash)
                                instructions.erase(
                                    std::next(instructions.begin(), i), instructions.end());

                        loop.entry = instructions;

                        // increase the iterator by the loop size
                        size_t skip_amount =
                            (loop.body.size() * (loop.iteration_count - 1)) + loop.entry.size();
                        std::advance(it, skip_amount - 1);
                        break;
                    }
                    else {
                        // couldnt find a loop, extend the pattern
                        search_for.push_back(hash_instruction(&*it_));
                        it_++;
                    }
                }
            }

            // we have found a loop that can be re-rolled
            if (loop.iteration_count > 3) {
                dataflow_t dependency_flow = dependency_analysis(
                    loop.blocks[loop.blocks.size() - 2].dataflow, loop.blocks.back().instructions);
                dataflow_t dataflow_of_last_block = loop.blocks.back().dataflow;
                dataflow_t phi_s = phi_analysis(dataflow_of_last_block, dependency_flow);

                llvm::Instruction *first_inst_of_body =
                    loop.blocks.back().instructions.back().value;
                llvm::BasicBlock *pre_header = first_inst_of_body->getParent();
                llvm::BasicBlock *unrolled_loop_body =
                    pre_header->splitBasicBlock(first_inst_of_body->getIterator());
                llvm::BasicBlock *loop_exit = unrolled_loop_body->splitBasicBlock(
                    loop.blocks.front().instructions.front().value->getIterator(), "loop.exit");

                // insert new loop header
                llvm::BasicBlock *loop_header = builder->basic_block("loop.header");
                pre_header->getTerminator()->eraseFromParent();
                builder->SetInsertPoint(pre_header);
                builder->CreateBr(loop_header);

                // create the loop body, will fill it later
                llvm::BasicBlock *loop_body = builder->basic_block("loop.body");
                llvm::ValueToValueMapTy VMap;

                // move loop_exit to after loop_body
                loop_exit->moveAfter(loop_body);

                // fill the new loop header
                std::map<llvm::Value *, llvm::PHINode *> backedge_map;
                builder->SetInsertPoint(loop_header);
                for (auto &D : phi_s.dependencies) {
                    llvm::PHINode *phi = builder->CreatePHI(
                        D.dependency->getType(), 2, "phi_" + D.dependency->getName().str());
                    phi->addIncoming(D.dependency, pre_header);
                    VMap[D.dependency] = phi;
                    backedge_map[D.access_instr] = phi;
                }
                llvm::PHINode *counter =
                    builder->CreatePHI(builder->getInt64Ty(), 2, "loop.counter");
                builder->CreateBr(loop_body);

                std::vector<instruction_t> back_original = loop.blocks.back().instructions;

                // reverse the instructions vector
                auto &body_instrs = loop.blocks.back().instructions;
                std::reverse(body_instrs.begin(), body_instrs.end());

                // create the loop body, replace the dependencies with new phi nodes, using VMap
                builder->SetInsertPoint(loop_body);
                // copy the old body (first iteration in the loop)
                for (auto &I : loop.blocks.back().instructions) {
                    if (!I.value)
                        continue;
                    // dont copy the marker function
                    if (llvm::CallInst *C = llvm::dyn_cast<llvm::CallInst>(I.value))
                        if (C->getCalledFunction() == MarkerFunc.getCallee())
                            continue;

                    llvm::Instruction *copy_I = I.value->clone();
                    if (backedge_map.count(I.value))
                        backedge_map[I.value]->addIncoming(copy_I, loop_body);

                    llvm::ValueMapper VM(VMap);
                    llvm::RemapInstruction(copy_I, VMap, llvm::RF_NoModuleLevelChanges);
                    builder->Insert(copy_I);
                    VMap[I.value] = copy_I;
                }

                // create the counter
                llvm::Value *next_counter =
                    builder->CreateAdd(counter, builder->getInt64(1), "next.counter");
                counter->addIncoming(builder->getInt64(0), pre_header);
                counter->addIncoming(next_counter, loop_body);

                // create the loop condition
                llvm::Value *cond =
                    builder->CreateICmpSLT(next_counter, builder->getInt64(loop.iteration_count));
                builder->CreateCondBr(cond, loop_header, loop_exit);

                // thanks to claude for below
                //   all_loop_values  — fast membership test for "is this user inside the loop?"
                //   structural_exit_map — maps every loop instruction at position j to
                //                         VMap[back_original[j]], its re-rolled equivalent.
                //                         Used to fix up post-loop uses before erasing.
                llvm::SmallPtrSet<llvm::Value *, 32> all_loop_values;
                std::map<llvm::Value *, llvm::Value *> structural_exit_map;

                for (auto &B : loop.blocks) {
                    for (size_t j = 0; j < B.instructions.size(); j++) {
                        llvm::Value *val = B.instructions[j].value;
                        if (!val)
                            continue;

                        all_loop_values.insert(val);

                        if (j < back_original.size()) {
                            llvm::Value *canonical = back_original[j].value;
                            if (canonical && VMap.count(canonical))
                                structural_exit_map[val] = VMap[canonical];
                        }
                    }
                }

                // Erase old unrolled loop instructions.
                // For each instruction, before killing it:
                //   redirect any post-loop uses to the re-rolled equivalent via
                //   structural_exit_map, then wipe remaining (intra-loop) uses with undef and
                //   erase.
                for (auto &B : loop.blocks) {
                    for (auto &LI : B.instructions) {
                        if (!LI.value || !LI.value->getParent())
                            continue;

                        auto it_map = structural_exit_map.find(LI.value);
                        if (it_map != structural_exit_map.end()) {
                            llvm::Value *rerolled = it_map->second;
                            for (auto UI = LI.value->use_begin(), UE = LI.value->use_end();
                                 UI != UE;)
                            {
                                llvm::Use &use = *UI++;
                                auto *user_inst = llvm::dyn_cast<llvm::Instruction>(use.getUser());
                                if (user_inst && !all_loop_values.count(user_inst))
                                    use.set(rerolled);
                            }
                        }

                        LI.value->replaceAllUsesWith(llvm::UndefValue::get(LI.value->getType()));
                        LI.value->eraseFromParent();
                    }
                }

                LOG_INFO(
                    logger, "Loop detected and re-rolled, iteration {}", loop.iteration_count + 1);
                // we have re-rolled a loop
                return true;
            }
        }
    }

    // remove the any markers left in the function
    for (auto &BB : *builder->get_function())
        for (auto &I : llvm::make_early_inc_range(BB))
            if (llvm::CallInst *C = llvm::dyn_cast<llvm::CallInst>(&I))
                if (C->getCalledFunction() == MarkerFunc.getCallee())
                    C->eraseFromParent();

    return false;
}