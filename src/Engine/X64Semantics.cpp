#include <VEXA/VEXA.h>

#define VexaInstHandlerSrc(instName) VEXA::Value VEXA::Engine::instName(ZydisDisassembledInstruction& instruction)
#define VexaLambdaWrapper(instName) [this](ZydisDisassembledInstruction& insn) { return instName(insn); }
#define Const64(expr) CreateConcreteVar(expr, 64)

void VEXA::Engine::InitHandlers()
{
	handlers = {
		{ ZydisMnemonic::ZYDIS_MNEMONIC_MOV, VexaLambdaWrapper(mov)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_ADD, VexaLambdaWrapper(add)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_RET, VexaLambdaWrapper(ret)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_CMP, VexaLambdaWrapper(cmp)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_CMOVNZ, VexaLambdaWrapper(cmovnz)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_JMP, VexaLambdaWrapper(jmp)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_JNZ, VexaLambdaWrapper(jnz)}
	};
}

VexaInstHandlerSrc(mov)
{
	TRY()
	{
		Value op2 = GetOperand(instruction.operands[1]);
		SetOperand(instruction.operands[0], op2);
		return Const64(ReadRegister(X64::RIP).as_uint64() + instruction.info.length);
	}
	CATCH("MOV Handler")
}

VexaInstHandlerSrc(add)
{
	TRY()
	{
		Value op1 = GetOperand(instruction.operands[0]);
		Value op2 = GetOperand(instruction.operands[1]);
		EqualizeSizes(op1, op2);
		Value resultAdd = op1 + op2;
		SetOperand(instruction.operands[0], resultAdd);
		return Const64(ReadRegister(X64::RIP).as_uint64() + instruction.info.length);
	}
	CATCH("ADD Handler")
}

VexaInstHandlerSrc(cmp)
{
	TRY()
	{
		Value op1 = GetOperand(instruction.operands[0]);
		Value op2 = GetOperand(instruction.operands[1]);
		EqualizeSizes(op1, op2);
		Value resultSub = op1 - op2;

		Value zf_bool = resultSub == CreateConcreteVar(0, resultSub.expr().get_sort().bv_size());
		Value zf_expr = z3::ite(zf_bool.expr(), context->bv_val(1, 1), context->bv_val(0, 1));
		WriteRegister(X64::ZF, zf_expr);
		return Const64(ReadRegister(X64::RIP).as_uint64() + instruction.info.length);
	}
	CATCH("CMP Handler")
	
}

VexaInstHandlerSrc(jmp)
{
	TRY()
	{
		Value op1 = GetOperand(instruction.operands[0]);
		if (op1.type() == ValueType::SYMBOLIC)
			return op1;			

		// resolve relative address
		uint64_t address = instruction.operands[0].imm.is_relative ?
			ResolveRelativeAddress(instruction, op1.as_uint64()) :
			op1.as_uint64();

		return Const64(address);
	}
	CATCH("JMP Handler")
}

VexaInstHandlerSrc(jnz)
{
	TRY()
	{
		Value op1 = GetOperand(instruction.operands[0]);

		if (op1.type() == ValueType::SYMBOLIC)
			throw std::runtime_error("This shouldnt happen..");

		// resolve relative address
		uint64_t address = instruction.operands[0].imm.is_relative ?
			ResolveRelativeAddress(instruction, op1.as_uint64()) :
			op1.as_uint64();

		Value zf = ReadRegister(X64::ZF);
		Value rip = z3::ite(zf.expr() != CreateConcreteVar(0, 1).expr(),
			CreateConcreteVar(address, 64).expr(),
			CreateConcreteVar(ReadRegister(X64::RIP).as_uint64() + (unsigned int)instruction.info.length, 64).expr()
		);
		return rip;
	}
	CATCH("JNZ Handler")
}

VexaInstHandlerSrc(cmovnz)
{
	TRY()
	{
		Value op1 = GetOperand(instruction.operands[0]);
		Value op2 = GetOperand(instruction.operands[1]);
		Value zf = ReadRegister(X64::ZF);
		EqualizeSizes(op1, op2);
		Value newValue = z3::ite((zf == CreateConcreteVar(0, 1)).expr(), op2.expr(), op1.expr());
		SetOperand(instruction.operands[0], newValue);

		return Const64(ReadRegister(X64::RIP).as_uint64() + instruction.info.length);
	}
	CATCH("CMOVNZ Handler")
}

VexaInstHandlerSrc(ret)
{
	TRY()
	{
		// TODO: need to implement stack
		return Const64(0);//Const64(ReadRegister(X64::RIP).as_uint64() + instruction.info.length);
	}
	CATCH("RET Handler")
}
