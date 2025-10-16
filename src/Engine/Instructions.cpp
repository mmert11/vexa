#include <VEXA/VEXA.h>

#define VexaInstHandlerSrc(instName) uint64_t VEXA::Engine::instName(ZydisDisassembledInstruction instruction)
#define VexaLambdaWrapper(instName) [this](ZydisDisassembledInstruction insn) { return instName(insn); }

void VEXA::Engine::InitHandlers()
{
	handlers = {
		{ ZydisMnemonic::ZYDIS_MNEMONIC_MOV, VexaLambdaWrapper(mov)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_ADD, VexaLambdaWrapper(add)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_RET, VexaLambdaWrapper(ret)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_CMP, VexaLambdaWrapper(cmp)},
		{ ZydisMnemonic::ZYDIS_MNEMONIC_CMOVNZ, VexaLambdaWrapper(cmovnz)}
	};
}

VexaInstHandlerSrc(mov)
{
	Value op2 = GetOperand(instruction.operands[1]);
	SetOperand(instruction.operands[0], op2);
	return ReadRegister(X64::RIP).as_int64() + instruction.info.length;
}

VexaInstHandlerSrc(add)
{
	Value op1 = GetOperand(instruction.operands[0]);
	Value op2 = GetOperand(instruction.operands[1]);
	Value resultAdd = Add(op1, op2);
	SetOperand(instruction.operands[0], resultAdd);
	return ReadRegister(X64::RIP).as_int64() + instruction.info.length;
}

VexaInstHandlerSrc(cmp)
{
	Value op1 = GetOperand(instruction.operands[0]);
	Value op2 = GetOperand(instruction.operands[1]);
	Value resultSub = Sub(op1, op2);

	Value zf_bool = resultSub == CreateConcreteVar(0, resultSub.expr().get_sort().bv_size());
	Value zf_expr = z3::ite(zf_bool.expr(), context->bv_val(1, 1), context->bv_val(0, 1));
	WriteRegister(X64::ZF, zf_expr);
	return ReadRegister(X64::RIP).as_int64() + instruction.info.length;
}

VexaInstHandlerSrc(cmovnz)
{
	Value op1 = GetOperand(instruction.operands[0]);
	Value op2 = GetOperand(instruction.operands[1]);
	Value zf = ReadRegister(X64::ZF);
	Value newValue = z3::ite((zf == CreateConcreteVar(0, 1)).expr(), op2.expr(), op1.expr());
	SetOperand(instruction.operands[0], newValue);

	return ReadRegister(X64::RIP).as_int64() + instruction.info.length;
}

VexaInstHandlerSrc(ret)
{
	// TODO: need to implement stack
	return ReadRegister(X64::RIP).as_int64() + instruction.info.length;
}