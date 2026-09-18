#include "vexa/passes/predicate_solver.hpp"
#include "bitwuzla/cpp/sat_solver.h"
#include <llvm/IR/InstrTypes.h>
#include <llvm/Support/Casting.h>
#include <vexa/vexa.h>

#include <llvm/IR/CFG.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>

/*
struct PathConstraint
{
    llvm::Value *condition;
    bool expected_value;
};

std::vector<PathConstraint>
    collect_path_constraints(llvm::Instruction *target_inst, llvm::DominatorTree &DT)
{
    std::vector<PathConstraint> constraints;
    llvm::BasicBlock *target_bb = target_inst->getParent();

    llvm::DomTreeNode *curr_node = DT.getNode(target_bb);
    while (curr_node) {
        llvm::DomTreeNode *idom_node = curr_node->getIDom();
        if (!idom_node) {
            break;
        }

        llvm::BasicBlock *idom_bb = idom_node->getBlock();
        llvm::Instruction *term = idom_bb->getTerminator();

        if (auto *br = llvm::dyn_cast<llvm::BranchInst>(term)) {
            if (br->isConditional()) {
                llvm::Value *cond = br->getCondition();
                llvm::BasicBlock *true_bb = br->getSuccessor(0);
                llvm::BasicBlock *false_bb = br->getSuccessor(1);

                if (DT.dominates(llvm::BasicBlockEdge(idom_bb, true_bb), target_bb)) {
                    constraints.push_back({cond, true});
                }
                else if (DT.dominates(llvm::BasicBlockEdge(idom_bb, false_bb), target_bb)) {
                    constraints.push_back({cond, false});
                }
            }
        }
        else if (auto *sw = llvm::dyn_cast<llvm::SwitchInst>(term)) {
            llvm::Value *cond = sw->getCondition();
            for (auto &Case : sw->cases()) {
                llvm::BasicBlock *case_bb = Case.getCaseSuccessor();
                if (DT.dominates(llvm::BasicBlockEdge(idom_bb, case_bb), target_bb)) {
                }
            }
        }

        curr_node = idom_node;
    }

    return constraints;
}
*/

bool is_leaf(llvm::Instruction *I)
{
    if (llvm::isa<llvm::LoadInst>(I) || llvm::isa<llvm::PHINode>(I))
        return true;
    return false;
}

std::vector<llvm::Instruction *> slice_backward(llvm::Instruction *I)
{
    std::vector<llvm::Instruction *> slice;
    std::vector<llvm::Instruction *> worklist;

    worklist.push_back(llvm::dyn_cast<llvm::Instruction>(I));
    slice.push_back(I);

    while (!worklist.empty()) {
        llvm::Instruction *current = worklist.back();
        worklist.pop_back();

        if (is_leaf(current))
            continue;

        std::vector<llvm::Value *> operands;
        for (auto &op : current->operands()) {
            llvm::Value *operand = op.get();
            if (auto *op_inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
                if (std::find(slice.begin(), slice.end(), op_inst) == slice.end()) {
                    slice.push_back(op_inst);
                    worklist.push_back(op_inst);
                }
            }
        }
    }

    return slice;
}

llvm::Instruction *find_common_ancestor(llvm::Instruction *I)
{
    if (is_leaf(I))
        return I;

    llvm::Instruction *common_ancestor = nullptr;

    std::vector<llvm::Instruction *> op_insts;
    for (llvm::Use &op : I->operands()) {
        if (auto *inst = llvm::dyn_cast<llvm::Instruction>(op.get())) {
            op_insts.push_back(inst);
        }
    }

    auto first_slice = slice_backward(llvm::dyn_cast<llvm::Instruction>(op_insts.front()));

    for (auto it = std::next(op_insts.begin()); it != op_insts.end(); ++it) {
        llvm::Instruction *op = llvm::dyn_cast<llvm::Instruction>(*it);
        auto slice = slice_backward(op);

        if (common_ancestor == nullptr) {
            for (auto *inst : first_slice) {
                if (std::find(slice.begin(), slice.end(), inst) != slice.end()) {
                    common_ancestor = inst;
                    break;
                }
            }
        }
        else {
            if (std::find(slice.begin(), slice.end(), common_ancestor) == slice.end()) {
                common_ancestor = nullptr;
                break;
            }
        }
    }
    return common_ancestor;
}

bool vexa::passes::predicate_solver::run(llvm::Function *func)
{
    for (auto &BB : *func) {
        for (auto it = BB.rbegin(), end = BB.rend(); it != end; ++it) {
            llvm::Instruction *I = &*it;
            llvm::Value *cond = nullptr;

            if (auto *select = llvm::dyn_cast<llvm::SelectInst>(I))
                cond = select->getCondition();
            else if (auto *br = llvm::dyn_cast<llvm::BranchInst>(I))
                if (br->isConditional())
                    cond = br->getCondition();

            if (!cond)
                continue;

            if (!blacklist.insert(cond).second)
                continue;

            llvm::Instruction *current_node = llvm::dyn_cast<llvm::Instruction>(cond);
            while (current_node) {
                if (is_leaf(current_node))
                    break;

                // collect non-constant instructions from operands
                //
                std::vector<llvm::Instruction *> operands;
                for (auto &op : current_node->operands()) {
                    llvm::Value *operand = op.get();
                    if (auto *inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
                        operands.push_back(inst);
                    }
                }

                if (operands.size() == 1) {
                    current_node = operands.front();
                    continue;
                }

                llvm::Instruction *common_ancestor = find_common_ancestor(current_node);
                if (!common_ancestor)
                    break;

                current_node = common_ancestor;
            }

            if (!current_node || current_node == cond)
                continue;

            std::vector<llvm::Instruction *> slice =
                slice_backward(llvm::dyn_cast<llvm::Instruction>(cond));

            bool has_phi = false;
            for (llvm::Instruction *inst : slice) {
                if (llvm::isa<llvm::PHINode>(inst)) {
                    has_phi = true;
                    break;
                }
            }

            if (has_phi)
                continue;
            
            // clear stale cache entries in the slice to force re-evaluation
            //
            for (llvm::Instruction *inst : slice) {
                if (inst != current_node && !is_leaf(inst)) {
                    symex->erase(inst);
                }
            }

            bw::Term term;
            vexa::value *cond_sym = nullptr;
            try {
                term = symex->get(current_node)->as_expr();
                cond_sym = symex->get(cond);
            } catch (const std::exception &e) {
                continue;
            }

            LOG_DEBUG(logger, "Term: {}", term.str(16));

            std::vector<uint8_t> check_list{0, 1};

            for (auto check_cond : check_list) {
                bw::Term cond_assumption =
                    (*cond_sym == *symex->concrete(check_cond, 1))->as_expr();
                std::vector<bw::Term> assumptions = {cond_assumption};

                std::vector<bw::Term> solved_values;
                bw::Result res;
                while (solved_values.size() < 4) {
                    res = context->bitwuzla->check_sat(assumptions);

                    if (res != bw::Result::SAT) {
                        break;
                    }

                    bw::Term evaluated = context->bitwuzla->get_value(term);
                    bw::Term eq = context->term_manager.mk_term(bw::Kind::EQUAL, {term, evaluated});
                    bw::Term block = context->term_manager.mk_term(bw::Kind::NOT, {eq});

                    assumptions.push_back(block);
                    solved_values.push_back(evaluated);
                }

                if (res != bw::Result::UNSAT)
                    continue;

                for (auto &solved : solved_values) {
                    LOG_DEBUG(logger, "Solved: {}", solved.str(16));
                }

                // recreate the condition with new solved values
                //
                builder->SetInsertPoint(llvm::dyn_cast<llvm::Instruction>(cond));
                uint8_t size = current_node->getType()->getPrimitiveSizeInBits();
                uint64_t uint64 =
                    llvm::APInt(size, solved_values.front().value<std::string>(10), 10)
                        .getZExtValue();

                llvm::CmpInst::Predicate predicate = (check_cond == 1)
                                                         ? llvm::CmpInst::Predicate::ICMP_EQ
                                                         : llvm::CmpInst::Predicate::ICMP_NE;
                llvm::Value *new_cond =
                    builder->CreateICmp(predicate, current_node, builder->getIntN(size, uint64));
                blacklist.insert(new_cond);

                for (size_t i = 1; i < solved_values.size(); i++) {
                    bw::Term solved_value = solved_values[i];
                    uint64 =
                        llvm::APInt(size, solved_value.value<std::string>(10), 10).getZExtValue();

                    llvm::CmpInst::Predicate predicate = check_cond == 1
                                                             ? llvm::CmpInst::Predicate::ICMP_EQ
                                                             : llvm::CmpInst::Predicate::ICMP_NE;
                    llvm::Value *_cond = builder->CreateICmp(
                        predicate, current_node, builder->getIntN(size, uint64));

                    if (check_cond == 1) {
                        new_cond = builder->CreateOr(new_cond, _cond);
                    }
                    else {
                        new_cond = builder->CreateAnd(new_cond, _cond);
                    }

                    blacklist.insert(new_cond);
                }

                cond->replaceAllUsesWith(new_cond);
                return true;
            }
        }
    }

    return false;
}