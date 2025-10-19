#pragma once
#include "../SymbolicState/SymbolicState.h"
#include "../Lifter/Lifter.h"

#include <iostream>

#define VexaInstHandler(instName) uint64_t instName(ZydisDisassembledInstruction& instruction)

namespace VEXA
{
	enum class EventType
	{
		EXEC,
		PATH
	};

	enum class EventWhen
	{
		BEFORE,
		AFTER
	};

	enum class EventAction
	{
		CONTINUE,
		HALT
	};

	struct EventHook {
		EventType type;
		EventWhen when;
		std::function<EventAction(Engine&, ZydisDisassembledInstruction)> callback;
	};

	class Engine
	{
	public:
		Engine();
		/// @brief Starts the symbolic execution at RIP
		/// (you need to set RIP before execution)
		void Run();
		// TODO: add brief
		bool IsPath(ZydisDisassembledInstruction instruction);
		std::shared_ptr<z3::context> GetContext();
		void AddEventHook(EventHook callback_fn);
		std::shared_ptr<SymbolicState> TakeSnapshot();
		void RestoreSnapshot(std::shared_ptr<SymbolicState> newState);
		/// @brief Writes the values comes from buffer at the specified address
		/// @param addr Value representing the starting address in memory.
		/// @param buffer Pointer that points to the values to be written
		/// @param size Size of the buffer
		void WriteMemory(Value addr, uint8_t* buffer, int64_t size);
		/// @brief Writes a value to memory at the specified address.
		/// @param addr Value representing the address in memory.
		/// @param value Value to be written to memory.
		/// @param size The size of the value in bits (e.g., 32, 64).
		void WriteMemory(Value addr, Value value, int32_t size);
		/// @brief Reads a value from memory at the specified address.
		/// @param addr Value representing the starting address in memory.
		/// @param size The size of the value in bits.
		/// @return Returns a VEXA::Value object representing the read value.
		Value ReadMemory(Value addr, int32_t size);
		/// @brief Creates a new symbolic (unknown) variable.
		/// @param name The symbolic name of the variable.
		/// @param size The size of the variable in bits.
		/// @return Returns a VEXA::Value object representing the new symbolic variable.
		Value CreateSymbolicVar(std::string name, int32_t size);
		/// @brief Creates a new concrete (known) variable with the specified value and size.
		/// @param val The concrete value of the variable.
		/// @param size The size of the variable in bits.
		/// @return Returns a VEXA::Value object representing the new concrete variable.
		Value CreateConcreteVar(uint64_t val, int32_t size);
		/// @brief Writes to the specified register
		/// @param reg Register to write to
		/// @param val Value to write
		void WriteRegister(reg_t reg, Value val);
		/// @brief Reads the value at specified register.
		/// @param reg Register to read.
		/// @return Returns a VEXA::Value object representing the read value.
		Value ReadRegister(reg_t reg);
		/// @brief Prints the CPU state
		void PrintState();
		
		Value GetOperand(ZydisDecodedOperand op);
		void SetOperand(ZydisDecodedOperand op, Value value);

	private:
		std::shared_ptr<z3::context> context;
		std::shared_ptr<SymbolicState> state;
		std::shared_ptr<Lifter> lifter;

		void ProcessInstruction(ZydisDisassembledInstruction& instruction);

		// operations
		Value Add(Value a, Value b);
		Value Sub(Value a, Value b);

		// instructions
		void InitHandlers();

		VexaInstHandler(mov);
		VexaInstHandler(add);
		VexaInstHandler(ret);
		VexaInstHandler(cmp);
		VexaInstHandler(cmovnz);
		VexaInstHandler(jmp);

		std::unordered_map<ZydisMnemonic, std::function<uint64_t(ZydisDisassembledInstruction&)>> handlers;

		// events
		std::vector<EventHook> event_hooks;
		void EventHooks(EventType type, EventWhen when, ZydisDisassembledInstruction instr);
	};
}
