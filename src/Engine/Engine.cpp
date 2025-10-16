#include <VEXA/VEXA.h>

VEXA::Engine::Engine()
{
	TRY()
	{
		InitHandlers();
		context = std::make_shared<z3::context>();
		state = std::make_shared<SymbolicState>(context);

		// symbolize all registers
		for (reg_t r = 0; r < X64::NB_REGS; r++) {
			if (X64::reg_info.at(r).size_bits == 64)
				state->cpu->Write(r, CreateSymbolicVar(X64::reg_to_str.at(r), 64));
		}
	}
	CATCH("Engine init error")
}

void VEXA::Engine::Run()
{
	TRY()
	{
		lifter = std::make_shared<VEXA::Lifter>(this->ReadRegister(X64::RIP).as_int64(), this);

		while (true)
		{
			Value RIP = this->ReadRegister(X64::RIP);
			if (RIP.type() != ValueType::CONCRETE)
				throw std::runtime_error("Whooops.. RIP is symbolic!");

			// read 15 bytes from rip
			std::vector<uint8_t> bytes;
			for (int i = 0; i < 15; i++)
			{
				Value offset = CreateConcreteVar(i, 64);
				Value address = Add(RIP, offset);

				Value v = ReadMemory(address, 8);
				if (v.type() == ValueType::CONCRETE)
					bytes.push_back(static_cast<uint8_t>(v.as_int64()));
			}

			// disassemble the bytes and fetch the instruction
			ZyanU64 runtime_address = RIP.as_int64();
			uint8_t* data = bytes.data();

			ZyanUSize offset = 0;
			ZydisDisassembledInstruction instruction;

			if (ZYAN_SUCCESS(ZydisDisassembleIntel(
				ZYDIS_MACHINE_MODE_LONG_64,
				runtime_address,
				data + offset,
				bytes.size(),
				&instruction
			)) && bytes.size() > 0)
			{
				offset += instruction.info.length;
				ProcessInstruction(instruction);
			}
			else
				break;
		}
		printf("\n");

		//std::cout << "---- Unoptimized IR ----" << std::endl;
		//lifter->PrintIR();

		lifter->Optimize();
		std::cout << "\n---- Optimized IR ----" << std::endl;
		lifter->PrintIR();

		printf("\n");
	}
	CATCH("Engine error")
}

void VEXA::Engine::ProcessInstruction(ZydisDisassembledInstruction instruction)
{
	auto handler = handlers.find(instruction.info.mnemonic);
	if (handler != handlers.end()) 
	{
		bool is_path = IsPath(instruction);
		EventHooks(EventType::EXEC, EventWhen::BEFORE, instruction);

		if (is_path)
			EventHooks(EventType::PATH, EventWhen::BEFORE, instruction);

		lifter->LiftInstruction(instruction);
		uint64_t nextRip = handler->second(instruction);

		EventHooks(EventType::EXEC, EventWhen::AFTER, instruction);

		if (is_path)
			EventHooks(EventType::PATH, EventWhen::AFTER, instruction);

		WriteRegister(X64::RIP, CreateConcreteVar(nextRip, 64));
	}
	else 
		throw std::runtime_error(std::string("Unimplemented instruction:") + ZydisMnemonicGetString(instruction.info.mnemonic));
	
}

VEXA::Value VEXA::Engine::GetOperand(ZydisDecodedOperand op)
{
	switch (op.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
	{
		return ReadRegister(X64::zydisToVexaReg[op.reg.value]);
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_IMMEDIATE:
	{
		return CreateConcreteVar(op.imm.value.u, op.size);
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
	{
		// TODO: implement memory read from operand
		throw std::runtime_error("Memory operand is not implemented yet");
	}
	default:
		break;
	}
}

void VEXA::Engine::SetOperand(ZydisDecodedOperand op, Value value)
{
	switch (op.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
	{
		return WriteRegister(X64::zydisToVexaReg[op.reg.value], value);
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
	{
		throw std::runtime_error("Memory operand is not implemented yet");
	}
	default:
		break;
	}
}

bool VEXA::Engine::IsPath(ZydisDisassembledInstruction instruction)
{
	if (instruction.info.mnemonic == ZYDIS_MNEMONIC_CMOVNZ)
	{
		return ReadRegister(X64::ZF).type() == ValueType::SYMBOLIC;
	}
	else
		return false;
}

std::shared_ptr<z3::context> VEXA::Engine::GetContext()
{
	return context;
}

void VEXA::Engine::AddEventHook(EventHook callback_fn)
{
	event_hooks.push_back(callback_fn);
}

std::shared_ptr<VEXA::SymbolicState> VEXA::Engine::TakeSnapshot()
{
	return state->clone();
}

void VEXA::Engine::RestoreSnapshot(std::shared_ptr<SymbolicState> newState)
{
	state = newState;
}

void VEXA::Engine::WriteMemory(Value addr, uint8_t* buffer, int64_t size)
{
	// write into address byte by byte
	for (int64_t i = 0; i < size; i++)
	{
		Value offset = CreateConcreteVar(i, 64);
		Value target_addr = Add(addr, offset);

		Value byte_to_write = CreateConcreteVar(*(uint8_t*)(buffer + i), 8);
		WriteMemory(target_addr, byte_to_write, 8);
	}
}

void VEXA::Engine::WriteMemory(Value addr, Value value, int32_t size)
{
	state->memory->Write(addr, value, size);
}

VEXA::Value VEXA::Engine::ReadMemory(Value addr, int32_t size)
{
	return state->memory->Read(addr, size);
}

VEXA::Value VEXA::Engine::CreateSymbolicVar(std::string name, int32_t size)
{
	return state->memory->SymVar(name, size);
}

VEXA::Value VEXA::Engine::CreateConcreteVar(uint64_t val, int32_t size)
{
	return state->memory->ConVar(val, size);
}

void VEXA::Engine::WriteRegister(reg_t reg, Value val)
{
	state->cpu->Write(reg, val);
}

VEXA::Value VEXA::Engine::ReadRegister(reg_t reg)
{
	return state->cpu->Read(reg);
}

VEXA::Value VEXA::Engine::Add(Value a, Value b)
{
	return Value(a.expr() + b.expr());
}

VEXA::Value VEXA::Engine::Sub(Value a, Value b)
{
	return Value(a.expr() - b.expr());
}

void VEXA::Engine::PrintState()
{
	std::cout << "------ VEXA Symbolic State ------" << std::endl;
	for (auto& [reg_id, reg_info] : X64::reg_info)
	{
		if (X64::reg_info.at(reg_id).size_bits < 64)
			continue;

		Value registerValue = ReadRegister(reg_id);
		std::cout << X64::reg_to_str.at(reg_id) << ": ";

		if (registerValue.type() == ValueType::CONCRETE)
			std::cout << std::hex << registerValue.as_int64() << std::endl;
		else
			std::cout << registerValue.expr().to_string() << std::endl;
	}
}

void VEXA::Engine::EventHooks(EventType type, EventWhen when, ZydisDisassembledInstruction instr = ZydisDisassembledInstruction())
{
	for (auto& event : event_hooks)
	{
		if (event.type == type && event.when == when)
			event.callback(*this, instr);
	}
}