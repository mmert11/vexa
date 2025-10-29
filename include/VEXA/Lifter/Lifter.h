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
	class Lifter
	{
	public:
		Lifter();
		Lifter(uint64_t address, VEXA::Engine* engine);
		void LiftInstruction(ZydisDisassembledInstruction& instruction);
		
		void Optimize();
		std::string GetIRString();

		std::unordered_map<VEXA::reg_t, int> vexaToLLVMRegId = {
			{X64::RAX, 0},
			{X64::RBX, 1},
			{X64::RCX, 2},
			{X64::RDX, 3},
			{X64::RFLAGS, 4}
		};
		std::map<int, std::shared_ptr<llvm::Value>> registers;
		std::shared_ptr<llvm::IRBuilder<>> builder;

		std::string InstrToBrName(std::string text);
		void SetOperand(ZydisDecodedOperand operand, llvm::Value* value);
		llvm::Value* GetOperand(ZydisDecodedOperand operand);
		void WriteRegister(VEXA::reg_t reg, llvm::Value* value);
		llvm::Value* ReadRegister(VEXA::reg_t reg);
		void SetLLVMRegister(int reg_id, llvm::Value* value);
		llvm::Value* GetLLVMRegister(int reg_id);
		llvm::Value* GetCondition(ZydisDisassembledInstruction instruction);
		llvm::BasicBlock* CreateCondBr(ZydisDisassembledInstruction instruction);
		llvm::BasicBlock* CreateIndirectJmp(ZydisDisassembledInstruction instruction, VEXA::Value true_dest, VEXA::Value false_dest);
		void CreateDirectJmp(ZydisDisassembledInstruction instruction);
		
	private:
		std::string GetBlockNameFromInstr(ZydisDisassembledInstruction instruction, int id);
		void NormalizeIntSizes(llvm::Value*& a, llvm::Value*& b);
		void InitHandlers();
		bool IsBranching();

		std::map<ZydisMnemonic, std::function<void(ZydisDisassembledInstruction)>> handlers;
		int vip;

		InstrHandler(mov);
		InstrHandler(add);
		InstrHandler(cmp);
		InstrHandler(cmovnz);
		InstrHandler(ret);

		VEXA::Engine* symEngine;
		std::shared_ptr<llvm::LLVMContext> llvm_context;
		std::shared_ptr<llvm::Module> module;
		llvm::Function* func = nullptr;
	};
}
