#include <vexa/vexa.h>

std::vector<bw::Term>
    vexa::value::possible_values(const std::vector<bw::Term> &constraints, bw::Result *result)
{
    std::vector<bw::Term> assumptions = constraints;
    std::vector<bw::Term> solved_values;
    solved_values.reserve(20);

    bw::Result check = bw::Result::UNKNOWN;
    while (solved_values.size() < 50) {
        check = solver->check_sat(assumptions);
        if (check != bw::Result::SAT)
            break;

        bw::Term evaluated = solver->get_value(term);
        assumptions.push_back(term_manager->mk_term(bw::Kind::DISTINCT, {term, evaluated}));
        solved_values.push_back(std::move(evaluated));
    }

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
        return term;
    return term_manager->mk_term(bw::Kind::EQUAL, {term, term_manager->mk_bv_one(term.sort())});
}

bw::Term vexa::value::unary(bw::Kind op) const
{
    return term_manager->mk_term(op, {term});
}

bw::Term vexa::value::binary(bw::Kind op, const value &rhs) const
{
    return term_manager->mk_term(op, {term, rhs.term});
}

bw::Term vexa::value::operator+(const value &rhs) const
{
    return binary(bw::Kind::BV_ADD, rhs);
}
bw::Term vexa::value::operator-(const value &rhs) const
{
    return binary(bw::Kind::BV_SUB, rhs);
}
bw::Term vexa::value::operator*(const value &rhs) const
{
    return binary(bw::Kind::BV_MUL, rhs);
}
bw::Term vexa::value::operator/(const value &rhs) const
{
    return binary(bw::Kind::BV_SDIV, rhs);
}
bw::Term vexa::value::operator%(const value &rhs) const
{
    return binary(bw::Kind::BV_SREM, rhs);
}
bw::Term vexa::value::operator!() const
{
    return term_manager->mk_term(bw::Kind::NOT, {as_expr_bool()});
}
bw::Term vexa::value::operator&(const value &rhs) const
{
    return binary(bw::Kind::BV_AND, rhs);
}
bw::Term vexa::value::operator|(const value &rhs) const
{
    return binary(bw::Kind::BV_OR, rhs);
}
bw::Term vexa::value::operator^(const value &rhs) const
{
    return binary(bw::Kind::BV_XOR, rhs);
}
bw::Term vexa::value::operator<<(const value &rhs) const
{
    return binary(bw::Kind::BV_SHL, rhs);
}
bw::Term vexa::value::operator>>(const value &rhs) const
{
    return binary(bw::Kind::BV_ASHR, rhs);
}
bw::Term vexa::value::operator-() const
{
    return unary(bw::Kind::BV_NEG);
}
bw::Term vexa::value::operator~() const
{
    return unary(bw::Kind::BV_NOT);
}
bw::Term vexa::value::operator==(const value &rhs) const
{
    return binary(bw::Kind::EQUAL, rhs);
}
bw::Term vexa::value::operator!=(const value &rhs) const
{
    return binary(bw::Kind::DISTINCT, rhs);
}
bw::Term vexa::value::operator<(const value &rhs) const
{
    return binary(bw::Kind::BV_SLT, rhs);
}
bw::Term vexa::value::operator<=(const value &rhs) const
{
    return binary(bw::Kind::BV_SLE, rhs);
}
bw::Term vexa::value::operator>(const value &rhs) const
{
    return binary(bw::Kind::BV_SGT, rhs);
}
bw::Term vexa::value::operator>=(const value &rhs) const
{
    return binary(bw::Kind::BV_SGE, rhs);
}
bw::Term vexa::value::udiv(const value &rhs) const
{
    return binary(bw::Kind::BV_UDIV, rhs);
}
bw::Term vexa::value::urem(const value &rhs) const
{
    return binary(bw::Kind::BV_UREM, rhs);
}
bw::Term vexa::value::lshr(const value &rhs) const
{
    return binary(bw::Kind::BV_SHR, rhs);
}
bw::Term vexa::value::ult(const value &rhs) const
{
    return binary(bw::Kind::BV_ULT, rhs);
}
bw::Term vexa::value::ule(const value &rhs) const
{
    return binary(bw::Kind::BV_ULE, rhs);
}
bw::Term vexa::value::ugt(const value &rhs) const
{
    return binary(bw::Kind::BV_UGT, rhs);
}
bw::Term vexa::value::uge(const value &rhs) const
{
    return binary(bw::Kind::BV_UGE, rhs);
}
bw::Term vexa::value::concat(const value &rhs) const
{
    return binary(bw::Kind::BV_CONCAT, rhs);
}

bw::Term vexa::value::extract(uint64_t high, uint64_t low) const
{
    return term_manager->mk_term(bw::Kind::BV_EXTRACT, {term}, {high, low});
}

bw::Term vexa::value::zext(uint64_t amount) const
{
    return term_manager->mk_term(bw::Kind::BV_ZERO_EXTEND, {term}, {amount});
}

bw::Term vexa::value::sext(uint64_t amount) const
{
    return term_manager->mk_term(bw::Kind::BV_SIGN_EXTEND, {term}, {amount});
}

bw::Term vexa::value::ite(const value &then_value, const value &else_value) const
{
    return term_manager->mk_term(bw::Kind::ITE, {as_expr_bool(), then_value.term, else_value.term});
}