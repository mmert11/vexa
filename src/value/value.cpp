#include <vexa/vexa.h>

vexa::value::value(vexa::context *c)
    : context(c), _kind(kind::value), term_manager(nullptr), solver(nullptr), simplified(false)
{}

vexa::value::value(vexa::context *c, kind k)
    : context(c), _kind(k), term_manager(nullptr), solver(nullptr), simplified(false)
{}

vexa::value::value(vexa::context *c, bw::Term e, bw::TermManager &tm, bw::Bitwuzla &bzla)
    : context(c),
      _kind(kind::value),
      term(std::move(e)),
      term_manager(&tm),
      solver(&bzla),
      simplified(term.is_value())
{}

std::vector<bw::Term>
    vexa::value::possible_values(const std::vector<bw::Term> &constraints, bw::Result *result)
{
    std::vector<bw::Term> assumptions = constraints;
    std::vector<bw::Term> solved_values;
    solved_values.reserve(20);

    bw::Result check = bw::Result::UNKNOWN;
    while (solved_values.size() < 20) {
        check = solver->check_sat(assumptions);
        if (check != bw::Result::SAT)
        {
            break;
        }

        bw::Term evaluated = solver->get_value(term);
        assumptions.push_back(term_manager->mk_term(bw::Kind::DISTINCT, {term, evaluated}));
        solved_values.push_back(std::move(evaluated));
    }

    check = solver->check_sat(assumptions);

    if (result)
        *result = check;
    return solved_values;
}

vexa::value *vexa::value::simplify()
{
    if (!simplified) {
        term = solver->simplify(term);
        simplified = true;
    }
    return this;
}

vexa::value *vexa::value::simplify(const std::vector<bw::Term> &constraints)
{
    simplify();
    if (is_concrete() || solver->check_sat(constraints) != bw::Result::SAT)
        return this;

    bw::Term candidate = solver->get_value(term);
    std::vector<bw::Term> assumptions = constraints;

    // check if it can take another possible value
    //
    assumptions.push_back(term_manager->mk_term(bw::Kind::DISTINCT, {term, candidate}));
    if (solver->check_sat(assumptions) == bw::Result::UNSAT) {
        return context->symex->value(std::move(candidate));
    }
    return this;
}

bool vexa::value::is_symbolic() const
{
    return !term.is_value();
}

bool vexa::value::is_concrete() const
{
    return term.is_value();
}

uint64_t vexa::value::as_uint64() const
{
    if (!is_concrete())
        THROW("cannot concretize value");

    return as_uint64(term);
}

uint64_t vexa::value::as_uint64(const bw::Term &term)
{
    uint64_t width = term.sort().bv_size();
    llvm::APInt value(width, term.value<std::string>(10), 10);
    return width <= 64 ? value.getZExtValue() : value.trunc(64).getZExtValue();
}

uint64_t vexa::value::size() const
{
    return term.sort().bv_size();
}

bw::Term vexa::value::as_expr() const
{
    return term;
}

bw::Term vexa::value::as_expr_bool() const
{
    if (term.sort().is_bool())
        return (term);
    return term_manager->mk_term(bw::Kind::EQUAL, {term, term_manager->mk_bv_one(term.sort())});
}

vexa::value *vexa::value::unary(bw::Kind op) const
{
    return context->symex->value(term_manager->mk_term(op, {term}));
}

vexa::value *vexa::value::binary(bw::Kind op, const value &rhs) const
{
    return context->symex->value(term_manager->mk_term(op, {term, rhs.term}));
}

vexa::value *vexa::value::operator+(const value &rhs) const
{
    return binary(bw::Kind::BV_ADD, rhs);
}
vexa::value *vexa::value::operator-(const value &rhs) const
{
    return binary(bw::Kind::BV_SUB, rhs);
}
vexa::value *vexa::value::operator*(const value &rhs) const
{
    return binary(bw::Kind::BV_MUL, rhs);
}
vexa::value *vexa::value::operator/(const value &rhs) const
{
    return binary(bw::Kind::BV_SDIV, rhs);
}
vexa::value *vexa::value::operator%(const value &rhs) const
{
    return binary(bw::Kind::BV_SREM, rhs);
}
vexa::value *vexa::value::operator!() const
{
    return context->symex->value(term_manager->mk_term(bw::Kind::NOT, {as_expr_bool()}));
}
vexa::value *vexa::value::operator&(const value &rhs) const
{
    return binary(bw::Kind::BV_AND, rhs);
}
vexa::value *vexa::value::operator|(const value &rhs) const
{
    return binary(bw::Kind::BV_OR, rhs);
}
vexa::value *vexa::value::operator^(const value &rhs) const
{
    return binary(bw::Kind::BV_XOR, rhs);
}
vexa::value *vexa::value::operator<<(const value &rhs) const
{
    return binary(bw::Kind::BV_SHL, rhs);
}
vexa::value *vexa::value::operator>>(const value &rhs) const
{
    return binary(bw::Kind::BV_ASHR, rhs);
}
vexa::value *vexa::value::operator-() const
{
    return unary(bw::Kind::BV_NEG);
}
vexa::value *vexa::value::operator~() const
{
    return unary(bw::Kind::BV_NOT);
}
vexa::value *vexa::value::operator==(const value &rhs) const
{
    return binary(bw::Kind::EQUAL, rhs);
}
vexa::value *vexa::value::operator!=(const value &rhs) const
{
    return binary(bw::Kind::DISTINCT, rhs);
}
vexa::value *vexa::value::operator<(const value &rhs) const
{
    return binary(bw::Kind::BV_SLT, rhs);
}
vexa::value *vexa::value::operator<=(const value &rhs) const
{
    return binary(bw::Kind::BV_SLE, rhs);
}
vexa::value *vexa::value::operator>(const value &rhs) const
{
    return binary(bw::Kind::BV_SGT, rhs);
}
vexa::value *vexa::value::operator>=(const value &rhs) const
{
    return binary(bw::Kind::BV_SGE, rhs);
}
vexa::value *vexa::value::udiv(const value &rhs) const
{
    return binary(bw::Kind::BV_UDIV, rhs);
}
vexa::value *vexa::value::urem(const value &rhs) const
{
    return binary(bw::Kind::BV_UREM, rhs);
}
vexa::value *vexa::value::lshr(const value &rhs) const
{
    return binary(bw::Kind::BV_SHR, rhs);
}
vexa::value *vexa::value::ult(const value &rhs) const
{
    return binary(bw::Kind::BV_ULT, rhs);
}
vexa::value *vexa::value::ule(const value &rhs) const
{
    return binary(bw::Kind::BV_ULE, rhs);
}
vexa::value *vexa::value::ugt(const value &rhs) const
{
    return binary(bw::Kind::BV_UGT, rhs);
}
vexa::value *vexa::value::uge(const value &rhs) const
{
    return binary(bw::Kind::BV_UGE, rhs);
}
vexa::value *vexa::value::concat(const value &rhs) const
{
    return binary(bw::Kind::BV_CONCAT, rhs);
}

vexa::value *vexa::value::extract(uint64_t high, uint64_t low) const
{
    return context->symex->value(term_manager->mk_term(bw::Kind::BV_EXTRACT, {term}, {high, low}));
}

vexa::value *vexa::value::zext(uint64_t amount) const
{
    return context->symex->value(term_manager->mk_term(bw::Kind::BV_ZERO_EXTEND, {term}, {amount}));
}

vexa::value *vexa::value::sext(uint64_t amount) const
{
    return context->symex->value(term_manager->mk_term(bw::Kind::BV_SIGN_EXTEND, {term}, {amount}));
}

vexa::value *vexa::value::ite(const value &then_value, const value &else_value) const
{
    return context->symex->value(
        term_manager->mk_term(bw::Kind::ITE, {as_expr_bool(), then_value.term, else_value.term}));
}