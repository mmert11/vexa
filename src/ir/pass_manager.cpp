#include <vexa/ir/pass_manager.hpp>
#include <vexa/vexa.h>

#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>

bool vexa::ir::simplify_cfg_and_dce::run(llvm::Function *func)
{
    llvm::FunctionAnalysisManager FAM;
    llvm::PassBuilder PB;
    PB.registerFunctionAnalyses(FAM);

    llvm::FunctionPassManager FPM;
    FPM.addPass(llvm::SimplifyCFGPass());
    FPM.addPass(llvm::ADCEPass());
    FPM.run(*func, FAM);

    return false;
}

bool vexa::ir::mem2reg_and_sroa::run(llvm::Function *func)
{
    llvm::FunctionAnalysisManager FAM;
    llvm::PassBuilder PB;
    PB.registerFunctionAnalyses(FAM);

    llvm::FunctionPassManager FPM;
    FPM.addPass(llvm::SROAPass(llvm::SROAOptions::PreserveCFG));
    FPM.addPass(llvm::PromotePass());
    FPM.run(*func, FAM);

    return false;
}

bool vexa::ir::Oz::run(llvm::Function *func)
{
    // optimize with llvm's optimization pipeline
    auto function = func;
    if (!function)
        THROW("function is null!");

    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM; //
    llvm::PassBuilder PB;

    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    llvm::FunctionPassManager FPM = PB.buildFunctionSimplificationPipeline(
        llvm::OptimizationLevel::Oz, llvm::ThinOrFullLTOPhase::None);

    FPM.run(*function, FAM);

    return false;
}

void vexa::ir::pass_manager::run(llvm::Function *func)
{
    if (llvm::verifyFunction(*func, &llvm::errs()))
        THROW("invalid IR before optimization!");

    // run the passes in order, if any pass changes something in the ir, start over the process
    bool repeat;
    do {
        repeat = false;
        for (auto &pass : pipeline) {
            if (!pass->is_recursive() && pass->did_run)
                continue;

            repeat = pass->run(func);
            pass->did_run = true;
            if (repeat)
                break;
        }
    } while (repeat);

    // reorder the blocks
    llvm::ReversePostOrderTraversal<llvm::Function *> RPOT(context->builder->get_function());
    std::vector<llvm::BasicBlock *> OrderedBlocks;
    for (auto *BB : RPOT)
        OrderedBlocks.push_back(BB);
    for (size_t i = 1; i < OrderedBlocks.size(); ++i)
        OrderedBlocks[i]->moveAfter(OrderedBlocks[i - 1]);

    // strip ssa value names
    for (auto &BB : *func) {
        for (auto &I : BB)
        {
            if (!I.getType()->isPointerTy())
                I.setName("");
        }
    }
}