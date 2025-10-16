#include <z3++.h>

namespace VEXA
{
	class Solver
	{
	public:
		Solver(std::shared_ptr<z3::context> c);
		void AddConstraint(z3::expr constraint);
		bool Check();
		z3::model GetModel();
		void Reset();
	private:
		std::shared_ptr<z3::context> context;
		std::shared_ptr<z3::solver> solver;
	};
}