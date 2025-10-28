#include <VEXA/VEXA.h>
#include <Zydis/Mnemonic.h>
#include <Zydis/SharedTypes.h>

VEXA::Engine::Engine()
{
	TRY()
	{
		InitHandlers();
		context = std::make_shared<z3::context>();
		state = std::make_shared<SymbolicState>(context);
		path_manager = std::make_shared<PathManager>();

		// symbolize all registers
		for (reg_t r = 0; r < X64::NB_REGS; r++)
		{
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
		lifter = std::make_shared<VEXA::Lifter>(this->ReadRegister(X64::RIP).as_uint64(), this);

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
				Value address = RIP + offset;

				Value v = ReadMemory(address, 8);
				if (v.type() == ValueType::CONCRETE)
					bytes.push_back(static_cast<uint8_t>(v.as_uint64()));
				else
					break;
			}

			// disassemble the bytes and fetch the instruction
			ZyanU64 runtime_address = RIP.as_uint64();
			ZyanUSize offset = 0;
			ZydisDisassembledInstruction instruction;

			if (ZYAN_SUCCESS(ZydisDisassembleIntel(
					ZYDIS_MACHINE_MODE_LONG_64,
					runtime_address,
					bytes.data() + offset,
					bytes.size(),
					&instruction)) &&
				bytes.size() > 0)
			{
				offset += instruction.info.length;
				ProcessInstruction(instruction);
			}
			else
			{ // if there is any unexplored path, restore it's state and keep executing
				if (path_manager->paths.size() > 0)
				{
					Path path = path_manager->paths.top();
					path_manager->paths.pop();

					// TODO: put these into a function
					RestoreSnapshot(path.snapshot);
					lifter->registers = path.registers;
					lifter->builder->SetInsertPoint(path.block);
				}
				else // if not, stop execution
					break;
			}
		}

		unoptimized_ir = lifter->GetIRString();
		lifter->Optimize();
		optimized_ir = lifter->GetIRString();

		printf("\n");
	}
	CATCH("Engine error")
}

void VEXA::Engine::ProcessInstruction(ZydisDisassembledInstruction &instruction)
{
	TRY()
	{
		auto handler = handlers.find(instruction.info.mnemonic);
		if (handler != handlers.end())
		{
			EventHooks(EventType::EXEC, EventWhen::BEFORE, instruction);
			uint64_t nextRip;

			if (IsBranching(instruction))
			{

				EventHooks(EventType::PATH, EventWhen::BEFORE, instruction);
				nextRip = HandlePath(instruction);
				EventHooks(EventType::PATH, EventWhen::AFTER, instruction);
			}
			else
			{
				lifter->LiftInstruction(instruction);
				nextRip = handler->second(instruction).as_uint64();
			}

			EventHooks(EventType::EXEC, EventWhen::AFTER, instruction);
			WriteRegister(X64::RIP, CreateConcreteVar(nextRip, 64));
		}
		else
			throw std::runtime_error(std::string("Unimplemented instruction: ") + ZydisMnemonicGetString(instruction.info.mnemonic));
		}
	CATCH("Engine error")
}

uint64_t VEXA::Engine::HandlePath(ZydisDisassembledInstruction &instruction)
{
	auto handler = handlers.find(instruction.info.mnemonic);
	Value dest = handler->second(instruction);

	if (dest.type() == ValueType::SYMBOLIC)
	{
		auto [_then, _else] = ResolveSymbolicDest(dest);

		if (_then.type() == ValueType::SYMBOLIC || _else.type() == ValueType::SYMBOLIC)
			throw std::runtime_error("Couldnt resolve symbolic destinations!");

		// TODO: put these into a function
		Path p;
		p.snapshot = TakeSnapshot();
		p.snapshot->cpu->Write(X64::RIP, _else);

		p.registers = lifter->registers;
		p.block = lifter->CreateCondBr(instruction);
		path_manager->paths.push(p);

		return _then.as_uint64();
	}
	else
	{
		// destination is not symbolic, means this is a direct jump
		lifter->CreateDirectJmp(instruction);
		return dest.as_uint64();
	}
}

std::pair<VEXA::Value, VEXA::Value> VEXA::Engine::ResolveSymbolicDest(VEXA::Value sym_dest)
{
	z3::expr expr = sym_dest.expr();
	if (expr.is_app())
	{
		z3::context &ctx = expr.ctx();
		z3::expr _cond(ctx), _then(ctx), _else(ctx);

		switch (expr.decl().decl_kind())
		{
		case Z3_OP_ITE:
			_cond = expr.arg(0);
			_then = expr.arg(1);
			_else = expr.arg(2);
			break;
		default:
			std::cerr << "ResolveSymbolicDest: unsupported expr kind "
					  << expr.decl().name() << std::endl;
			throw std::runtime_error("Could not resolve symbolic destination!");
		}

		return {VEXA::Value(_then), VEXA::Value(_else)};
	}
	throw std::runtime_error("Symbolic destination is not an app!");
}

VEXA::Value VEXA::Engine::GetOperand(ZydisDecodedOperand op)
{
	switch (op.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
		return ReadRegister(X64::zydisToVexaReg[op.reg.value]);
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_IMMEDIATE:
		return CreateConcreteVar(op.imm.value.u, op.size);
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
	{
		// TODO: implement memory read from operand
		throw std::runtime_error("Memory operand is not implemented yet");
	}
	default:
		throw std::runtime_error("Unimplemented operand");
	}
}

void VEXA::Engine::SetOperand(ZydisDecodedOperand op, Value value)
{
	switch (op.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
		return WriteRegister(X64::zydisToVexaReg[op.reg.value], value);
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
	{
		throw std::runtime_error("Memory operand is not implemented yet");
	}
	default:
		throw std::runtime_error("Unimplemented operand");
	}
}

// TODO: we can think of a better way to implement this
bool VEXA::Engine::IsBranching(ZydisDisassembledInstruction &instruction)
{
	// TODO: add ret here
	switch (instruction.info.mnemonic)
	{
	case ZYDIS_MNEMONIC_JMP:
	case ZYDIS_MNEMONIC_JNZ:
		return true;
	default:
		return false;
	}
}

uint64_t VEXA::Engine::ResolveRelativeAddress(ZydisDisassembledInstruction instruction, uint64_t offset)
{
	return instruction.runtime_address + instruction.info.length + offset;
}

void VEXA::Engine::WriteMemory(Value addr, uint8_t *buffer, int64_t size)
{
	// write into address byte by byte
	for (int64_t i = 0; i < size; i++)
	{
		Value offset = CreateConcreteVar(i, 64);
		Value target_addr = addr + offset;

		Value byte_to_write = CreateConcreteVar(*(uint8_t *)(buffer + i), 8);
		WriteMemory(target_addr, byte_to_write, 8);
	}
}

void VEXA::Engine::PrintState()
{
	std::cout << "------ VEXA Symbolic State ------" << std::endl;
	for (auto &[reg_id, reg_info] : X64::reg_info)
	{
		if (X64::reg_info.at(reg_id).size_bits < 64)
			continue;

		Value registerValue = ReadRegister(reg_id);
		std::cout << X64::reg_to_str.at(reg_id) << ": ";

		if (registerValue.type() == ValueType::CONCRETE)
			std::cout << std::hex << registerValue.as_uint64() << std::endl;
		else
			std::cout << registerValue.expr().to_string() << std::endl;
	}
}

void VEXA::Engine::PrintIR()
{
	std::cout << unoptimized_ir << std::endl;
}

void VEXA::Engine::PrintOptimizedIR()
{
	std::cout << optimized_ir << std::endl;
}

void VEXA::Engine::EventHooks(EventType type, EventWhen when, ZydisDisassembledInstruction instr = ZydisDisassembledInstruction())
{
	for (auto &event : event_hooks)
	{
		if (event.type == type && event.when == when)
			event.callback(*this, instr);
	}
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
