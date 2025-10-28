#include <VEXA/VEXA.h>

#define LifterHandler(handlerName) void VEXA::Lifter::handlerName(ZydisDisassembledInstruction instr)
#define LLVMLambdaWrapper(instName) [this](ZydisDisassembledInstruction insn) { instName(insn); }

void VEXA::Lifter::InitHandlers()
{
	handlers = {
	{ ZydisMnemonic::ZYDIS_MNEMONIC_MOV, LLVMLambdaWrapper(mov)},
	{ ZydisMnemonic::ZYDIS_MNEMONIC_ADD, LLVMLambdaWrapper(add)},
	{ ZydisMnemonic::ZYDIS_MNEMONIC_CMP, LLVMLambdaWrapper(cmp)},
	{ ZydisMnemonic::ZYDIS_MNEMONIC_CMOVNZ, LLVMLambdaWrapper(cmovnz)},
	{ ZydisMnemonic::ZYDIS_MNEMONIC_RET, LLVMLambdaWrapper(ret)},
	};
}

LifterHandler(ret)
{
	builder->CreateRet(ReadRegister(X64::RAX));
}

LifterHandler(mov)
{
	llvm::Value* op2 = GetOperand(instr.operands[1]);
	SetOperand(instr.operands[0], op2);
}

LifterHandler(add)
{
	llvm::Value* op1 = GetOperand(instr.operands[0]);
	llvm::Value* op2 = GetOperand(instr.operands[1]);
    NormalizeIntSizes(op1, op2);
	llvm::Value* result = builder->CreateAdd(
		op1, op2,
		op1->getName().str());
	SetOperand(instr.operands[0], result);
}

LifterHandler(cmp)
{
	llvm::Value* op1 = GetOperand(instr.operands[0]);
	llvm::Value* op2 = GetOperand(instr.operands[1]);
    NormalizeIntSizes(op1, op2);
    llvm::Value* zf = builder->CreateICmpEQ(op1, op2);
    WriteRegister(X64::ZF, zf);
}

LifterHandler(cmovnz)
{
    llvm::Value* dest = GetOperand(instr.operands[0]);
    llvm::Value* src = GetOperand(instr.operands[1]);

    llvm::Value* zf = ReadRegister(X64::ZF);
	llvm::Value* cond = GetCondition(instr);

	llvm::Value* valueToSet = builder->CreateSelect(cond, src, dest);
	SetOperand(instr.operands[0], valueToSet);
}