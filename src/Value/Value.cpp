#include <VEXA/VEXA.h>

using namespace VEXA;

VEXA::Value::Value() {}
VEXA::Value::Value(z3::expr exp) : val(std::make_shared<z3::expr>(exp.simplify()))
{ }

Value VEXA::Value::operator==(const Value& other) const
{
	return Value(this->expr() == other.expr());
}

Value Value::operator+(const Value& other) const {
	return Value(this->expr() + other.expr());
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
