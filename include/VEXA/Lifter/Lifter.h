#pragma once
#include <Zydis/Zydis.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

#define InstrHandler(insName) void insName(ZydisDisassembledInstruction instr)

namespace VEXA { class Engine; }

namespace VEXA
{
	struct Path
	{
	public:
		llvm::BasicBlock* true_block; 
		llvm::BasicBlock* false_block;
	};

	class Lifter
	{
	public:
		Lifter();
		Lifter(uint64_t address, VEXA::Engine* engine);
		void LiftInstruction(ZydisDisassembledInstruction instruction);
		void Optimize();
		void PrintIR();

		std::unordered_map<VEXA::reg_t, int> vexaToLLVMRegId = {
			{X64::RAX, 0},
			{X64::RBX, 1},
			{X64::RCX, 2},
			{X64::RDX, 3},
			{X64::RFLAGS, 4}
		};

		std::string InstrToBrName(std::string text);

		void SetOperand(ZydisDecodedOperand operand, llvm::Value* value);
		llvm::Value* GetOperand(ZydisDecodedOperand operand);

		void WriteRegister(VEXA::reg_t reg, llvm::Value* value);
		llvm::Value* ReadRegister(VEXA::reg_t reg);

		void SetLLVMRegister(int reg_id, llvm::Value* value);
		llvm::Value* GetLLVMRegister(int reg_id);

		llvm::Value* GetCondition(ZydisDisassembledInstruction instruction);
		
	//private:
		std::string GetBlockNameFromInstr(ZydisDisassembledInstruction instruction, int id);
		void InitHandlers();
		bool IsBranching();

		std::unordered_map<ZydisMnemonic, std::function<void(ZydisDisassembledInstruction)>> handlers;
		std::map<uint32_t, Path> paths;
		int vip;

		InstrHandler(mov);
		InstrHandler(add);
		InstrHandler(cmp);
		InstrHandler(cmovnz);
		InstrHandler(ret);
		InstrHandler(jmp);

		VEXA::Engine* symEngine;
		std::shared_ptr<llvm::LLVMContext> llvm_context;
		std::shared_ptr<llvm::IRBuilder<>> builder;
		std::shared_ptr<llvm::Module> module;
		llvm::Function* func = nullptr;

		std::unordered_map<int, llvm::Value*> registers;
	};
}