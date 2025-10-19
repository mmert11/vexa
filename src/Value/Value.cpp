#include <VEXA/VEXA.h>

using namespace VEXA;

VEXA::Value::Value() {}
VEXA::Value::Value(z3::expr exp) : val(std::make_shared<z3::expr>(exp.simplify()))
{ }

Value VEXA::Value::operator==(const Value& other) const
{
	return Value(this->expr() == other.expr());
}

// TODO: learn
Value Value::operator+(const Value& other) const {
    z3::expr left = this->expr();
    z3::expr right = other.expr();
    
    unsigned left_size = left.get_sort().bv_size();
    unsigned right_size = right.get_sort().bv_size();
    
    unsigned target_size = std::max(left_size, right_size);

    if (left_size < target_size) {
        unsigned num_bits_to_add = target_size - left_size; 
        left = z3::zext(left, num_bits_to_add);
    } 
    
    if (right_size < target_size) {
        unsigned num_bits_to_add = target_size - right_size;
        right = z3::zext(right, num_bits_to_add);
    }
    
    return Value(left + right);
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
