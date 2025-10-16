#include <VEXA/VEXA.h>

#include <iostream>
#include <Windows.h>

VEXA::Memory::Memory()
{
}

VEXA::Memory::Memory(std::shared_ptr<z3::context> c)
{
	context = c;

	// initialize symbolic memory as array theory
	z3::sort mem_sort = context->array_sort(context->bv_sort(64), context->bv_sort(8));
	memory = std::make_shared<z3::expr>(context->constant("memory", mem_sort));
}

void VEXA::Memory::Write(Value addr, Value value, int32_t size)
{
	TRY()
	if (size <= 0)
		throw std::runtime_error("Size must be greater than 0");
	
	z3::expr value_expr = value.expr();
	z3::expr addr_expr = addr.expr();

	// write the value byte by byte
	for (int i = 0; i < size / 8; ++i) 
	{
		z3::expr byte_val = value_expr.extract(i * 8 + 7, i * 8); // extract(high bit, low bit) (7,0), (13,8) etc.
		z3::expr byte_addr = context->bv_val(i, 64) + addr_expr; // calculate byte address
		*memory = z3::store(*memory, byte_addr, byte_val);
	}
	CATCH("Memory write error")
}

VEXA::Value VEXA::Memory::Read(Value addr, int32_t size)
{
	TRY()
	if (size <= 0)
		throw std::runtime_error("Size must be greater than 0");

	// read first byte
	z3::expr result = z3::select(*memory, addr.expr());

	// read more if needed
	for (int i = 1; i < size / 8; ++i)
	{
		z3::expr byte_addr = context->bv_val(i, 64) + addr.expr();
		z3::expr byte_val = z3::select(*memory, byte_addr);
		result = z3::concat(byte_val, result); // combine the read bytes
	}

	return Value(result);
	CATCH("Memory read error")
}

std::shared_ptr<VEXA::Memory> VEXA::Memory::clone()
{
	std::shared_ptr<Memory> newMemory = std::make_shared<VEXA::Memory>(this->context);
	newMemory->memory = std::make_shared<z3::expr>(*this->memory);
	return newMemory;
}

VEXA::Value VEXA::Memory::SymVar(std::string name, int32_t size)
{
	return Value(context->constant(name.c_str(), context->bv_sort(size)));
}

VEXA::Value VEXA::Memory::ConVar(uint64_t val, int32_t size)
{
	return Value(context->bv_val(val, size));
}