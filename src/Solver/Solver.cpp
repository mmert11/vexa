#include <VEXA/VEXA.h>

VEXA::Solver::Solver(std::shared_ptr<z3::context> c) : 
	context(c), solver(std::make_shared<z3::solver>(*c))
{
}

z3::model VEXA::Solver::GetModel()
{
	return solver->get_model();
}

void VEXA::Solver::Reset()
{
	solver->reset();
}

bool VEXA::Solver::Check()
{
	TRY()

		return solver->check() == z3::check_result::sat;

	CATCH("Solver error")
}

void VEXA::Solver::AddConstraint(z3::expr constraint)
{
	TRY()

		solver->add(constraint);

	CATCH("Solver error")
}
