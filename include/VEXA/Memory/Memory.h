#include "../Value/Value.h"

namespace VEXA
{
	class Memory
	{
	public:
		Memory();
		Memory(std::shared_ptr<z3::context> c);

		void Write(Value addr, Value value, int32_t size);
		Value Read(Value addr, int32_t size);
		Value SymVar(std::string name, int32_t size);
		Value ConVar(uint64_t val, int32_t size);
		std::shared_ptr<Memory> clone();
		std::shared_ptr<z3::context> context;
		std::shared_ptr<z3::expr> memory;
	};
}