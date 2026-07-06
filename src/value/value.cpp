#include <vexa/vexa.h>

void vexa::value::simplify()
{
    expr = expr->simplify();
}

bool vexa::value::is_symbolic() const
{
    return !expr->is_numeral();
}

bool vexa::value::is_concrete() const
{
    return expr->is_numeral();
}

uint64_t vexa::value::as_uint64() const
{
    return is_concrete()
        ? expr->get_numeral_uint64()
        : THROW("cannot concretize value");
}

uint64_t vexa::value::size() const
{
    return expr->get_sort().bv_size();
}

z3::expr vexa::value::as_expr() const
{
    
    return *expr;
}
