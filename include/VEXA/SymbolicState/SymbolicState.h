#include "../Memory/Memory.h"
#include "../CPU/CPU.h"

namespace VEXA
{
	class SymbolicState
	{
	public:
		SymbolicState(std::shared_ptr<z3::context> c);
		SymbolicState(bool empty);
		std::shared_ptr<SymbolicState> clone();
		std::shared_ptr<z3::context> context;
		std::shared_ptr<Memory> memory;
		std::shared_ptr<X86CPU> cpu;
	};
}