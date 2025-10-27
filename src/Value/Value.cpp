#include <VEXA/VEXA.h>

using namespace VEXA;

VEXA::Value::Value() {}
VEXA::Value::Value(z3::expr exp) : val(std::make_shared<z3::expr>(exp.simplify())) { }

Value VEXA::Value::operator==(const Value& other) const
{
	return Value(this->expr() == other.expr());
}

void EqualizeSizes(z3::expr& x, z3::expr& y)
{
    TRY()
    {
        // get the bitvector size of expressions
        unsigned left_size = x.get_sort().bv_size();
        unsigned right_size = y.get_sort().bv_size();
        // take the larger of two sizes
        unsigned target_size = std::max(left_size, right_size);

        // zero-extend the smaller value to the larger
        if (left_size < target_size) {
            unsigned num_bits_to_add = target_size - left_size; 
            x = z3::zext(x, num_bits_to_add);
        } 
        
        if (right_size < target_size) {
            unsigned num_bits_to_add = target_size - right_size;
            y = z3::zext(y, num_bits_to_add);
        }
    }
    CATCH("Value error")
}

Value Value::operator+(const Value& other) const
{
    z3::expr left = this->expr();
    z3::expr right = other.expr();

    // equalize the sizes of values
    EqualizeSizes(left, right);
    return Value(left + right);
}

Value Value::operator-(const Value& other) const 
{
    z3::expr left = this->expr();
    z3::expr right = other.expr();

    // equalize the sizes of values
    EqualizeSizes(left, right);
    return Value(left - right);
}

int64_t VEXA::Value::as_int64()
{
	TRY()
		if (this->type() == ValueType::CONCRETE)
			return val->get_numeral_int64();
		else
			throw std::runtime_error("Cannot concretize a symbolic value!");
	CATCH("Value error")
}

VEXA::ValueType VEXA::Value::type()
{
	TRY()
	return val->is_numeral() ? ValueType::CONCRETE : ValueType::SYMBOLIC;
	CATCH("Value error")
}

z3::expr VEXA::Value::expr() const
{
	return *val;
}
