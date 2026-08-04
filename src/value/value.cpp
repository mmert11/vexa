#include <vexa/vexa.h>

vexa::value* vexa::value::simplify()
{
    expr = expr->simplify();
    return this;
}

bool vexa::value::is_symbolic() const
{
    return !expr->is_numeral();
}

bool vexa::value::is_concrete() const
{
    uint64_t u; int64_t s;
    return expr->is_numeral_u64(u) || expr->is_numeral_i64(s);
}

uint64_t vexa::value::as_uint64() const
{
    if (!is_concrete())
        THROW("cannot concretize value");

    uint64_t u64 = 0;
    if (expr->is_numeral_u64(u64))
        return u64;

    std::string num_str;
    if (expr->is_numeral(num_str))
    {
        llvm::APInt big_value(256, num_str, 10);
        return big_value.trunc(64).getZExtValue();
    }

    THROW("Value doesn't fit in 64 bit size");
}

uint64_t vexa::value::size() const
{
    return expr->get_sort().bv_size();
}

z3::expr vexa::value::as_expr() const
{

    return *expr;
}
