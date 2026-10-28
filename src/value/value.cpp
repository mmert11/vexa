#include <vexa/vexa.h>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <string>

vexa::value::value(vexa::context *c)
    : context(c), _kind(kind::value), simplified(false)
{}

vexa::value::value(vexa::context *c, kind k)
    : context(c), _kind(k), simplified(false)
{}

vexa::value::value(vexa::context *c, bwr::Expr e)
    : context(c),
      _kind(kind::value),
      expr(std::move(e)),
      simplified(expr.kind() == bwr::Kind::Const)
{}

static void collect_bool_atoms(bwr::Expr e, std::vector<bwr::Expr>& atoms, std::unordered_set<uint32_t>& seen) {
    if (!e || atoms.size() >= 8) return;
    uint32_t h = e.raw();
    if (!seen.insert(h).second) return;
    bwr::Node n = e.node();
    if (n.width == 1 && n.kind != bwr::Kind::Const 
                     && n.kind != bwr::Kind::Select 
                     && n.kind != bwr::Kind::Binary 
                     && n.kind != bwr::Kind::Unary) {
        atoms.push_back(e);
        return;
    }
    for (const auto& ch : n.children) {
        collect_bool_atoms(ch, atoms, seen);
    }
}

std::vector<bwr::Expr>
    vexa::value::possible_values(const std::vector<bwr::Expr> &constraints, vexa::result *result)
{
    std::vector<bwr::Expr> solved_values;
    solved_values.reserve(64);

    bwr::Assumptions base_ass;
    for (const auto &c : constraints)
        base_ass.assume(c, true);

    if (base_ass.infeasible()) {
        if (result) *result = vexa::result::unsat;
        return solved_values;
    }

    if (is_concrete()) {
        solved_values.push_back(expr);
        if (result) *result = vexa::result::unsat;
        return solved_values;
    }

    auto outcome = context->bitwright_engine->run({expr}, nullptr, &base_ass);
    bwr::Expr curr = outcome.empty() ? expr : outcome[0].expr;
    if (curr.kind() == bwr::Kind::Const) {
        solved_values.push_back(curr);
        if (result) *result = vexa::result::unsat;
        return solved_values;
    }

    bwr::Facts f0 = bwr::facts(curr, &base_ass);
    if (f0.umin == f0.umax) {
        solved_values.push_back(context->bitwright_ctx.constant(curr.width(), f0.umin.limb(0)));
        if (result) *result = vexa::result::unsat;
        return solved_values;
    }
    // 1. Bool-atom pivoting: enumerate branches for <= 8 decision atoms (<= 256 combos).
    std::vector<bwr::Expr> atoms;
    std::unordered_set<uint32_t> seen_nodes;
    collect_bool_atoms(curr, atoms, seen_nodes);
    if (!atoms.empty() && atoms.size() <= 8) {
        size_t combos = 1ULL << atoms.size();
        bool all_resolved = true;
        std::unordered_set<uint64_t> seen_vals;
        for (size_t mask = 0; mask < combos; ++mask) {
            bwr::Assumptions ass;
            for (const auto &c : constraints) ass.assume(c, true);
            for (size_t i = 0; i < atoms.size(); ++i)
                ass.assume(atoms[i], (mask >> i) & 1);
            if (ass.infeasible()) continue;
            bwr::Facts fa = bwr::facts(curr, &ass);
            if (ass.infeasible()) continue;
            if (fa.umin == fa.umax) {
                uint64_t v = fa.umin.limb(0);
                if (seen_vals.insert(v).second)
                    solved_values.push_back(context->bitwright_ctx.constant(curr.width(), v));
            } else {
                all_resolved = false;
                break;
            }
        }
        if (all_resolved && !solved_values.empty()) {
            if (result)
                *result = vexa::result::unsat;
            return solved_values;
        }
        solved_values.clear();
    }


    // 2. Unbounded input guard:
    // If range is missing or span > 512 bytes, treat as free symbolic input.
    auto lo0 = f0.umin.to_u64();
    auto hi0 = f0.umax.to_u64();

    // check if the constraints prove an upper bound on curr's symbol via Bitwright's CDCL SAT solver.
    if (!constraints.empty() && (!lo0.has_value() || !hi0.has_value() || (*hi0 - *lo0) > 512)) {
        std::vector<bwr::Expr> syms;
        std::unordered_set<uint32_t> visited_syms;
        auto get_syms = [&](auto self, bwr::Expr e) -> void {
            if (!e) return;
            if (!visited_syms.insert(e.raw()).second) return;
            bwr::Node n = e.node();
            if (n.kind == bwr::Kind::Symbol) { syms.push_back(e); return; }
            for (const auto& ch : n.children) self(self, ch);
        };
        get_syms(get_syms, curr);

        if (syms.size() == 1) {
            bwr::Expr s = syms[0];
            uint16_t sw = s.width();
            bwr::Expr true_lit = context->bitwright_ctx.constant(1, 1);
            std::vector<uint64_t> cand_bounds;
            std::unordered_set<uint32_t> visited_consts;
            auto get_consts = [&](auto self, bwr::Expr e) -> void {
                if (!e) return;
                if (!visited_consts.insert(e.raw()).second) return;
                bwr::Node n = e.node();
                if (n.kind == bwr::Kind::Const) {
                    if (auto v = e.value()) {
                        uint64_t val = v->limb(0);
                        if (val <= 64 && val > 0) cand_bounds.push_back(val);
                    }
                }
                for (const auto& ch : n.children) self(self, ch);
            };
            for (const auto& c : constraints) get_consts(get_consts, c);
            std::sort(cand_bounds.begin(), cand_bounds.end());
            cand_bounds.erase(std::unique(cand_bounds.begin(), cand_bounds.end()), cand_bounds.end());

            for (uint64_t K : cand_bounds) {
                bwr::Expr ule_k = s.ule(context->bitwright_ctx.constant(sw, K));
                bool implied = false;
                for (const auto& c : constraints) {
                    auto eq = bwr::equivalent(c.select(ule_k, true_lit), true_lit, 20000);
                    if (eq.has_value() && *eq) {
                        implied = true;
                        break;
                    }
                }
                if (implied) {
                    base_ass.assume(ule_k, true);
                    f0 = bwr::facts(curr, &base_ass);
                    lo0 = f0.umin.to_u64();
                    hi0 = f0.umax.to_u64();
                    break;
                }
            }
        }
    }

    if (!lo0.has_value() || !hi0.has_value() || (*hi0 - *lo0) > 512) {
        if (result) *result = vexa::result::unknown;
        return solved_values;
    }

    // 3. Bounded table / index enumeration (<= 6 unknown bits, <= 64 values).
    uint16_t w = curr.width();
    uint64_t full_mask = (w == 64) ? ~0ULL : ((1ULL << w) - 1);
    uint64_t kz0 = f0.known_zero.to_u64().value_or(0) & full_mask;
    uint64_t ko0 = f0.known_one.to_u64().value_or(0) & full_mask;
    uint64_t unk0 = ~(kz0 | ko0) & full_mask;
    int unk_bits = __builtin_popcountll(unk0);

    if (unk_bits <= 6) {
        uint64_t sub = 0;
        do {
            uint64_t cand = ko0 | sub;
            if (cand >= *lo0 && cand <= *hi0 && (f0.ustride == 0 || (cand - *lo0) % f0.ustride == 0)) {
                bwr::Assumptions verify_ass;
                for (const auto &c : constraints) verify_ass.assume(c, true);
                verify_ass.assume(curr.eq(context->bitwright_ctx.constant(w, cand)), true);
                if (!verify_ass.infeasible()) {
                    bwr::Facts vf = bwr::facts(curr, &verify_ass);
                    if (!verify_ass.infeasible() && vf.umin.to_u64().value_or(0) == cand && vf.umax.to_u64().value_or(0) == cand)
                        solved_values.push_back(context->bitwright_ctx.constant(w, cand));
                }
            }
            sub = (sub - unk0) & unk0;
        } while (sub != 0 && solved_values.size() < 64);

        if (!solved_values.empty()) {
            if (result) *result = vexa::result::unsat;
            return solved_values;
        }
    }

    if (result)
        *result = vexa::result::unknown;
    return solved_values;
}

vexa::value *vexa::value::simplify()
{
    if (!simplified) {
        expr = context->bitwright_engine->simplify(expr);
        simplified = true;
    }
    return this;
}

vexa::value *vexa::value::simplify(const std::vector<bwr::Expr> &constraints)
{
    simplify();
    if (is_concrete())
        return this;

    bwr::Assumptions assumptions;
    for (const auto &c : constraints) {
        assumptions.assume(c, true);
    }

    if (assumptions.infeasible())
        return this;

    auto outcome = context->bitwright_engine->run({expr}, nullptr, &assumptions);
    if (!outcome.empty() && outcome[0].expr.kind() == bwr::Kind::Const) {
        return context->symex->value(outcome[0].expr);
    }

    bwr::Facts f = bwr::facts(expr, &assumptions);
    if (f.umin == f.umax) {
        auto val = f.umin.to_u64();
        if (val.has_value()) {
            return context->symex->concrete(*val, expr.width());
        }
    }

    return this;
}

bool vexa::value::is_symbolic() const
{
    return expr.kind() != bwr::Kind::Const;
}

bool vexa::value::is_concrete() const
{
    return expr.kind() == bwr::Kind::Const;
}

uint64_t vexa::value::as_uint64() const
{
    return as_uint64(expr);
}

uint64_t vexa::value::as_uint64(const bwr::Expr &e)
{
    if (e.kind() == bwr::Kind::Const) {
        auto val = e.value();
        if (val.has_value()) {
            return val->limb(0);
        }
    }
    THROW("expecting a constant value: {}", e.str());
    return 0;
}

uint64_t vexa::value::size() const
{
    return expr.width();
}

bwr::Expr vexa::value::as_expr() const
{
    return expr;
}

bwr::Expr vexa::value::as_expr_bool() const
{
    if (expr.width() == 1)
        return expr;
    return expr.ne(context->bitwright_ctx.constant(expr.width(), 0));
}

std::string vexa::value::name() const
{
    return expr.name().value_or("");
}

vexa::value *vexa::value::operator+(const value &rhs) const
{
    return context->symex->value(expr + rhs.expr);
}

vexa::value *vexa::value::operator-(const value &rhs) const
{
    return context->symex->value(expr - rhs.expr);
}

vexa::value *vexa::value::operator*(const value &rhs) const
{
    return context->symex->value(expr * rhs.expr);
}

vexa::value *vexa::value::operator/(const value &rhs) const
{
    return context->symex->value(expr.sdiv(rhs.expr));
}

vexa::value *vexa::value::operator%(const value &rhs) const
{
    return context->symex->value(expr.srem(rhs.expr));
}

vexa::value *vexa::value::operator!() const
{
    return context->symex->value(~as_expr_bool());
}

vexa::value *vexa::value::operator&(const value &rhs) const
{
    return context->symex->value(expr & rhs.expr);
}

vexa::value *vexa::value::operator|(const value &rhs) const
{
    return context->symex->value(expr | rhs.expr);
}

vexa::value *vexa::value::operator^(const value &rhs) const
{
    return context->symex->value(expr ^ rhs.expr);
}

vexa::value *vexa::value::operator<<(const value &rhs) const
{
    return context->symex->value(expr << rhs.expr);
}

vexa::value *vexa::value::operator>>(const value &rhs) const
{
    return context->symex->value(expr.ashr(rhs.expr));
}

vexa::value *vexa::value::operator-() const
{
    return context->symex->value(-expr);
}

vexa::value *vexa::value::operator~() const
{
    return context->symex->value(~expr);
}

vexa::value *vexa::value::operator==(const value &rhs) const
{
    return context->symex->value(expr.eq(rhs.expr));
}

vexa::value *vexa::value::operator!=(const value &rhs) const
{
    return context->symex->value(expr.ne(rhs.expr));
}

vexa::value *vexa::value::operator<(const value &rhs) const
{
    return context->symex->value(expr.slt(rhs.expr));
}

vexa::value *vexa::value::operator<=(const value &rhs) const
{
    return context->symex->value(expr.sle(rhs.expr));
}

vexa::value *vexa::value::operator>(const value &rhs) const
{
    return context->symex->value(expr.sgt(rhs.expr));
}

vexa::value *vexa::value::operator>=(const value &rhs) const
{
    return context->symex->value(expr.sge(rhs.expr));
}

vexa::value *vexa::value::udiv(const value &rhs) const
{
    return context->symex->value(expr.udiv(rhs.expr));
}

vexa::value *vexa::value::urem(const value &rhs) const
{
    return context->symex->value(expr.urem(rhs.expr));
}

vexa::value *vexa::value::lshr(const value &rhs) const
{
    return context->symex->value(expr >> rhs.expr);
}

vexa::value *vexa::value::ult(const value &rhs) const
{
    return context->symex->value(expr.ult(rhs.expr));
}

vexa::value *vexa::value::ule(const value &rhs) const
{
    return context->symex->value(expr.ule(rhs.expr));
}

vexa::value *vexa::value::ugt(const value &rhs) const
{
    return context->symex->value(expr.ugt(rhs.expr));
}

vexa::value *vexa::value::uge(const value &rhs) const
{
    return context->symex->value(expr.uge(rhs.expr));
}

vexa::value *vexa::value::concat(const value &rhs) const
{
    return context->symex->value(expr.concat(rhs.expr));
}

vexa::value *vexa::value::extract(uint64_t high, uint64_t low) const
{
    uint64_t len = high - low + 1;
    return context->symex->value(expr.extract(static_cast<uint16_t>(low), static_cast<uint16_t>(len)));
}

vexa::value *vexa::value::zext(uint64_t amount) const
{
    return context->symex->value(expr.zext(static_cast<uint16_t>(expr.width() + amount)));
}

vexa::value *vexa::value::sext(uint64_t amount) const
{
    return context->symex->value(expr.sext(static_cast<uint16_t>(expr.width() + amount)));
}

vexa::value *vexa::value::ite(const value &then_value, const value &else_value) const
{
    return context->symex->value(as_expr_bool().select(then_value.expr, else_value.expr));
}
