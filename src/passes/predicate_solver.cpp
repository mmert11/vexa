#include "vexa/passes/predicate_solver.hpp"

#include <llvm/IR/Instruction.h>
#include <unordered_set>
#include <vexa/vexa.h>

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/IntrinsicsX86.h>

using SymMap = std::unordered_map<std::string, llvm::Value *>;
using ExprCache = std::unordered_map<bw_expr, llvm::Value *>;

// ── bwr::Expr → llvm::Value* ────────────────────────────────────────────────

static llvm::Value *bwr_to_llvm(
    bwr::Expr expr,
    llvm::IRBuilder<> &B,

    ExprCache &cache,
    const SymMap &sym)

{
    bw_expr handle = expr.raw();
    auto it = cache.find(handle);

    if (it != cache.end())
        return it->second;

    bwr::Node n = expr.node();
    llvm::LLVMContext &ctx = B.getContext();
    llvm::Type *ty = llvm::IntegerType::get(ctx, n.width);
    llvm::Value *result = nullptr;

    switch (n.kind) {
    case bwr::Kind::Const: {
        auto val = expr.value();
        if (!val)
            return nullptr;

        auto u64 = val->to_u64();
        if (!u64)
            return nullptr;
        result = llvm::ConstantInt::get(ty, *u64);
        break;
    }
    case bwr::Kind::Symbol: {
        auto name = expr.name();

        if (!name)
            return nullptr;

        auto sit = sym.find(*name);
        if (sit == sym.end())
            return nullptr;
        llvm::Value *v = sit->second;
        if (v->getType() == ty)
            result = v;
        else if (v->getType()->getIntegerBitWidth() < n.width)
            result = B.CreateZExt(v, ty);
        else
            result = B.CreateTrunc(v, ty);
        break;
    }
    case bwr::Kind::Unary: {
        if (n.children.empty())
            return nullptr;

        llvm::Value *x = bwr_to_llvm(n.children[0], B, cache, sym);
        if (!x)
            return nullptr;

        switch (static_cast<bwr::UnOp>(n.op)) {
        case bwr::UnOp::Not:
            result = B.CreateNot(x);
            break;

        case bwr::UnOp::Neg:
            result = B.CreateNeg(x);
            break;
        case bwr::UnOp::Popcnt:
            result = B.CreateUnaryIntrinsic(llvm::Intrinsic::ctpop, x);
            break;
        case bwr::UnOp::Clz:
            result =
                B.CreateBinaryIntrinsic(llvm::Intrinsic::ctlz, x, llvm::ConstantInt::getFalse(ctx));
            break;
        case bwr::UnOp::Ctz:
            result =
                B.CreateBinaryIntrinsic(llvm::Intrinsic::cttz, x, llvm::ConstantInt::getFalse(ctx));
            break;
        case bwr::UnOp::Bswap:
            result = B.CreateUnaryIntrinsic(llvm::Intrinsic::bswap, x);
            break;
        case bwr::UnOp::BitRev:
            result = B.CreateUnaryIntrinsic(llvm::Intrinsic::bitreverse, x);
            break;
        default:
            return nullptr;
        }

        break;
    }
    case bwr::Kind::Binary: {
        if (n.children.size() < 2)
            return nullptr;

        llvm::Value *a = bwr_to_llvm(n.children[0], B, cache, sym);
        llvm::Value *b = bwr_to_llvm(n.children[1], B, cache, sym);

        if (!a || !b)
            return nullptr;

        switch (static_cast<bwr::BinOp>(n.op)) {
        case bwr::BinOp::Add:
            result = B.CreateAdd(a, b);
            break;
        case bwr::BinOp::Sub:
            result = B.CreateSub(a, b);
            break;
        case bwr::BinOp::Mul:
            result = B.CreateMul(a, b);
            break;
        case bwr::BinOp::UDiv:
            result = B.CreateUDiv(a, b);
            break;
        case bwr::BinOp::URem:
            result = B.CreateURem(a, b);
            break;
        case bwr::BinOp::SDiv:
            result = B.CreateSDiv(a, b);
            break;
        case bwr::BinOp::SRem:
            result = B.CreateSRem(a, b);
            break;
        case bwr::BinOp::And:
            result = B.CreateAnd(a, b);
            break;
        case bwr::BinOp::Or:
            result = B.CreateOr(a, b);
            break;
        case bwr::BinOp::Xor:
            result = B.CreateXor(a, b);
            break;
        case bwr::BinOp::Shl:
            result = B.CreateShl(a, b);
            break;
        case bwr::BinOp::LShr:
            result = B.CreateLShr(a, b);
            break;
        case bwr::BinOp::AShr:
            result = B.CreateAShr(a, b);
            break;
        case bwr::BinOp::RotL:
            result = B.CreateIntrinsic(llvm::Intrinsic::fshl, {ty}, {a, a, b});
            break;
        case bwr::BinOp::RotR:
            result = B.CreateIntrinsic(llvm::Intrinsic::fshr, {ty}, {a, a, b});
            break;
        case bwr::BinOp::UMulHi: {
            unsigned w = n.width;
            llvm::Type *wide = llvm::IntegerType::get(ctx, w * 2);
            result = B.CreateTrunc(
                B.CreateLShr(
                    B.CreateMul(B.CreateZExt(a, wide), B.CreateZExt(b, wide)),

                    llvm::ConstantInt::get(wide, w)),
                ty);
            break;
        }
        case bwr::BinOp::SMulHi: {
            unsigned w = n.width;
            llvm::Type *wide = llvm::IntegerType::get(ctx, w * 2);
            result = B.CreateTrunc(
                B.CreateAShr(
                    B.CreateMul(B.CreateSExt(a, wide), B.CreateSExt(b, wide)),

                    llvm::ConstantInt::get(wide, w)),
                ty);
            break;
        }
        case bwr::BinOp::Pdep:
            result = B.CreateIntrinsic(
                n.width == 32 ? llvm::Intrinsic::x86_bmi_pdep_32 : llvm::Intrinsic::x86_bmi_pdep_64,
                {},
                {a, b});
            break;
        case bwr::BinOp::Pext:
            result = B.CreateIntrinsic(
                n.width == 32 ? llvm::Intrinsic::x86_bmi_pext_32 : llvm::Intrinsic::x86_bmi_pext_64,
                {},
                {a, b});
            break;
        default:
            return nullptr;
        }
        break;
    }
    case bwr::Kind::Compare: {
        if (n.children.size() < 2)
            return nullptr;
        llvm::Value *a = bwr_to_llvm(n.children[0], B, cache, sym);
        llvm::Value *b = bwr_to_llvm(n.children[1], B, cache, sym);
        if (!a || !b)
            return nullptr;
        llvm::CmpInst::Predicate pred;
        switch (static_cast<bwr::CmpOp>(n.op)) {
        case bwr::CmpOp::Eq:
            pred = llvm::CmpInst::ICMP_EQ;
            break;
        case bwr::CmpOp::Ne:
            pred = llvm::CmpInst::ICMP_NE;
            break;
        case bwr::CmpOp::Ult:
            pred = llvm::CmpInst::ICMP_ULT;
            break;
        case bwr::CmpOp::Ule:
            pred = llvm::CmpInst::ICMP_ULE;
            break;
        case bwr::CmpOp::Ugt:
            pred = llvm::CmpInst::ICMP_UGT;
            break;
        case bwr::CmpOp::Uge:
            pred = llvm::CmpInst::ICMP_UGE;
            break;
        case bwr::CmpOp::Slt:
            pred = llvm::CmpInst::ICMP_SLT;
            break;
        case bwr::CmpOp::Sle:
            pred = llvm::CmpInst::ICMP_SLE;
            break;
        case bwr::CmpOp::Sgt:
            pred = llvm::CmpInst::ICMP_SGT;
            break;
        case bwr::CmpOp::Sge:
            pred = llvm::CmpInst::ICMP_SGE;
            break;
        default:
            return nullptr;
        }

        result = B.CreateICmp(pred, a, b);
        break;
    }
    case bwr::Kind::Zext: {
        if (n.children.empty())
            return nullptr;
        llvm::Value *x = bwr_to_llvm(n.children[0], B, cache, sym);
        if (!x)
            return nullptr;
        result = B.CreateZExt(x, ty);
        break;
    }
    case bwr::Kind::Sext: {
        if (n.children.empty())
            return nullptr;
        llvm::Value *x = bwr_to_llvm(n.children[0], B, cache, sym);
        if (!x)
            return nullptr;
        result = B.CreateSExt(x, ty);
        break;
    }
    case bwr::Kind::Extract: {
        if (n.children.empty())
            return nullptr;
        llvm::Value *x = bwr_to_llvm(n.children[0], B, cache, sym);
        if (!x)
            return nullptr;
        if (n.lo != 0)
            x = B.CreateLShr(x, llvm::ConstantInt::get(x->getType(), n.lo));
        result = B.CreateTrunc(x, ty);
        break;
    }
    case bwr::Kind::Concat: {
        if (n.children.size() < 2)
            return nullptr;
        llvm::Value *hi = bwr_to_llvm(n.children[0], B, cache, sym);
        llvm::Value *lo = bwr_to_llvm(n.children[1], B, cache, sym);
        if (!hi || !lo)
            return nullptr;
        uint16_t lo_w = n.children[1].width();
        result = B.CreateOr(
            B.CreateShl(B.CreateZExt(hi, ty), llvm::ConstantInt::get(ty, lo_w)),

            B.CreateZExt(lo, ty));
        break;
    }
    case bwr::Kind::Select: {
        if (n.children.size() < 3)
            return nullptr;

        llvm::Value *c = bwr_to_llvm(n.children[0], B, cache, sym);
        llvm::Value *t = bwr_to_llvm(n.children[1], B, cache, sym);
        llvm::Value *e = bwr_to_llvm(n.children[2], B, cache, sym);

        if (!c || !t || !e)
            return nullptr;

        if (!c->getType()->isIntegerTy(1))
            c = B.CreateICmpNE(c, llvm::ConstantInt::get(c->getType(), 0));
        result = B.CreateSelect(c, t, e);
        break;
    }
    default:
        return nullptr;
    }

    if (result)
        cache[handle] = result;

    return result;
}

static void blacklist_slice(
    llvm::Instruction *inst,
    std::unordered_set<llvm::Value *> &bl,
    std::unordered_set<llvm::Instruction *> &visited)

{
    if (!inst || !visited.insert(inst).second)
        return;

    bl.insert(inst);
    if (llvm::isa<llvm::LoadInst>(inst) || llvm::isa<llvm::PHINode>(inst))
        return;
    for (llvm::Use &op : inst->operands())
        if (auto *oi = llvm::dyn_cast<llvm::Instruction>(op.get()))
            blacklist_slice(oi, bl, visited);
}

static SymMap build_sym_map(llvm::Function *func, vexa::symex *sx)
{
    SymMap m;
    auto try_add = [&](llvm::Value *v) {
        if (!sx->is_sync(v))
            return;

        vexa::value *val = nullptr;
        try {
            val = sx->get(v);
        } catch (...) {
            return;
        }

        if (!val)
            return;

        bwr::Expr e = val->as_expr();
        if (e.kind() != bwr::Kind::Symbol)
            return;
        auto name = e.name();
        if (name)
            m.emplace(*name, v);
    };

    for (auto &arg : func->args())
        try_add(&arg);

    for (auto &BB : *func)
        for (auto &I : BB)
            try_add(&I);

    return m;
}
static void collect_pred_terms(bwr::Expr e, std::vector<bwr::Expr>& syms, std::vector<uint64_t>& consts, std::unordered_set<uint32_t>& visited) {
    if (!e) return;
    uint32_t h = e.raw();
    if (!visited.insert(h).second) return;
    bwr::Node n = e.node();
    if (n.kind == bwr::Kind::Symbol) {
        syms.push_back(e);
    } else if (n.kind == bwr::Kind::Const) {
        if (auto v = e.value()) {
            uint64_t c = v->limb(0);
            for (uint64_t cand : {c, c + 1, c - 1, (uint64_t)(-(int64_t)c), ~c}) {
                consts.push_back(cand);
            }
        }
    }
    for (const auto& ch : n.children) {
        collect_pred_terms(ch, syms, consts, visited);
    }
}


bool vexa::passes::predicate_solver::run(llvm::Function *func)
{
    for (llvm::BasicBlock &BB : *func) {
        for (llvm::Instruction &I : BB) {
            if (!llvm::isa<llvm::LoadInst>(&I)) {
                symex->erase(&I);
            }
        }
    }

    SymMap sym = build_sym_map(func, symex.get());
    llvm::IRBuilder<> B(func->getContext());
    std::vector<llvm::Instruction *> to_delete;
    bool changed = false;

    for (llvm::BasicBlock &BB : *func) {
        for (auto it = BB.begin(), end = BB.end(); it != end; ++it) {
            llvm::Instruction *inst = &*it;

            if (!blacklist.insert(inst).second && true)
                continue;

            llvm::Type *ty = inst->getType();
            if (!ty->isIntegerTy() || inst->isTerminator() || llvm::isa<llvm::StoreInst>(inst)
                || llvm::isa<llvm::LoadInst>(inst) || llvm::isa<llvm::PHINode>(inst))
            {
                continue;
            }

            vexa::value *sym_val = nullptr;
            try {
                sym_val = symex->get(inst);
            } catch (...) {
                continue;
            }
            if (!sym_val)
                continue;

            bwr::Expr original = sym_val->as_expr();
            if (original.kind() == bwr::Kind::Const)
                continue;

            auto outcomes = context->bitwright_engine->run({original});
            bwr::Expr simplified = outcomes.empty() ? original : outcomes[0].expr;
            if (simplified.width() == 1 || simplified.kind() == bwr::Kind::Select) {
                if (auto syn = bwr::synthesize(simplified, 5)) {
                    simplified = *syn;
                }
            }
            // If a 1-bit predicate remains in complex MBA form, verify candidate comparisons
            // using Bitwright's native CDCL SAT prover (bwr::equivalent).
            if (simplified.width() == 1 && simplified.kind() != bwr::Kind::Compare) {
                std::vector<bwr::Expr> vars;
                std::vector<uint64_t> cand_consts;
                std::unordered_set<uint32_t> visited_nodes;
                collect_pred_terms(simplified, vars, cand_consts, visited_nodes);
                if (!vars.empty() && vars.size() <= 2) {
                    std::sort(cand_consts.begin(), cand_consts.end());
                    cand_consts.erase(std::unique(cand_consts.begin(), cand_consts.end()), cand_consts.end());
                    if (cand_consts.size() > 12) cand_consts.resize(12);
                    bwr::Expr target_var = vars[0];
                    uint16_t vw = target_var.width();
                    bool found_equiv = false;
                    for (uint64_t K : cand_consts) {
                        bwr::Expr K_expr = context->bitwright_ctx.constant(vw, K);
                        for (bwr::CmpOp op : {bwr::CmpOp::Eq, bwr::CmpOp::Ne, bwr::CmpOp::Sle, bwr::CmpOp::Slt}) {
                            bwr::Expr cand_cmp = target_var.cmp(op, K_expr);
                            if (bwr::equivalent(simplified, cand_cmp, 50000) == true) {
                                simplified = cand_cmp;
                                found_equiv = true;
                                break;
                            }
                        }
                        if (found_equiv) break;
                    }
                }
            }
            if (simplified == original)
                continue;
            if (simplified.width() != original.width())
                continue;

            B.SetInsertPoint(inst);
            ExprCache cache;
            llvm::Value *new_val = bwr_to_llvm(simplified, B, cache, sym);
            if (!new_val || new_val->getType() != ty)
                continue;

            LOG_DEBUG(logger, "Rewrote {} -> {}", original.str(), simplified.str());

            symex->set(new_val, symex->value(simplified));
            inst->replaceAllUsesWith(new_val);

            std::unordered_set<llvm::Instruction *> visited;
            blacklist_slice(llvm::dyn_cast<llvm::Instruction>(new_val), blacklist, visited);

            to_delete.push_back(inst);
        }
    }

    for (auto it = to_delete.rbegin(); it != to_delete.rend(); ++it) {
        if ((*it)->use_empty()) {
            symex->erase(*it);
            (*it)->eraseFromParent();
        }
    }

    return true;
}