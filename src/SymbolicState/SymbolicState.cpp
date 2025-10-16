#include <VEXA/SymbolicState/SymbolicState.h>

#include <iostream>

VEXA::SymbolicState::SymbolicState(std::shared_ptr<z3::context> c)
{
	context = c;
	memory = std::make_shared<Memory>(context);
	cpu = std::make_shared<X86CPU>(context);
}

VEXA::SymbolicState::SymbolicState(bool empty)
{
	// empty constructor to use in clone()
}

std::shared_ptr<VEXA::SymbolicState> VEXA::SymbolicState::clone()
{
	std::shared_ptr<VEXA::SymbolicState> new_state = std::make_shared<VEXA::SymbolicState>(true);
	new_state->context = this->context;
	new_state->memory = this->memory->clone();
	new_state->cpu = this->cpu->clone();

	return new_state;
}