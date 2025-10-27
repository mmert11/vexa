#pragma once
#include <z3++.h>

namespace VEXA
{
	enum class ValueType
	{
		SYMBOLIC,
		CONCRETE
	};

	class Value
	{
	public:
		Value();
		Value(z3::expr exp);
		Value operator==(const Value& other) const;
		Value operator+(const Value& other) const;
		Value operator-(const Value& other) const;

		z3::expr expr() const;
		ValueType type();
		int64_t as_int64();
	private:
		std::shared_ptr<z3::expr> val;
	};
}
