#include <VEXA/VEXA.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/OptimizationLevel.h>

#include <sstream>

VEXA::Lifter::Lifter() {}

VEXA::Lifter::Lifter(uint64_t address, VEXA::Engine* engine)
{
	symEngine = engine;
	llvm_context = std::make_shared<llvm::LLVMContext>();
	module = std::make_shared<llvm::Module>("VexaLifter", *llvm_context);

	llvm::Type* i64_t = llvm::Type::getInt64Ty(*llvm_context);
	int arg_size = vexaToLLVMRegId.size();

	std::vector<llvm::Type*> args;
	for (int i = 0; i < arg_size; i++) // this will be 16+
		args.push_back(i64_t);

	std::stringstream func_name_stream;
	func_name_stream << std::hex << address;

	llvm::FunctionType* funcType = llvm::FunctionType::get(i64_t, args, false);
	func = llvm::Function::Create(
		funcType,
		llvm::Function::ExternalLinkage,
		"VexaLifted_" + func_name_stream.str(),
		module.get());

	llvm::IRBuilder<> Builder(*llvm_context);

	// initialize ir registers as arguments
	for (auto& reg : vexaToLLVMRegId)
	{
		llvm::Argument* argument = func->getArg(reg.second);
		argument->setName(X64::reg_to_str.at(reg.first));
		registers[reg.second] = argument;
	}

	// create entry block of the function
	llvm::BasicBlock* entry_bb = llvm::BasicBlock::Create(*llvm_context, "entry", func);
	builder = std::make_shared<llvm::IRBuilder<>>(entry_bb);

	InitHandlers();
}

void VEXA::Lifter::LiftInstruction(ZydisDisassembledInstruction instruction)
{
	TRY();

	// create basic block for each lifted instruction
	// example: 140001000_mov_eax_ebx:
	std::string bb_name = GetBlockNameFromInstr(instruction, vip);
	llvm::BasicBlock* address_bb = llvm::BasicBlock::Create(*llvm_context,
		bb_name, func);

	builder->CreateBr(address_bb);
	builder = std::make_shared<llvm::IRBuilder<>>(address_bb);

	// match the instruction with handlers and execute
	auto handler = handlers.find(instruction.info.mnemonic);
	if (handler != handlers.end())
	{
		handler->second(instruction);
		vip++;
	}
	else
		throw std::runtime_error(std::string("Unimplemented instruction:") + ZydisMnemonicGetString(instruction.info.mnemonic));

	CATCH("Lifter error")
}

llvm::Value* VEXA::Lifter::GetLLVMRegister(int reg_id)
{
	return registers.at(reg_id);
}

llvm::Value* VEXA::Lifter::GetCondition(ZydisDisassembledInstruction instruction)
{
	switch (instruction.info.mnemonic)
	{
	case ZYDIS_MNEMONIC_CMOVNZ:
		return builder->CreateICmpEQ(ReadRegister(X64::ZF), llvm::ConstantInt::get(builder->getInt1Ty(), 0));
	default:
		break;
	}
}

void VEXA::Lifter::SetLLVMRegister(int reg_id, llvm::Value* value)
{
	registers[reg_id] = value;
}

void VEXA::Lifter::SetOperand(ZydisDecodedOperand operand, llvm::Value* value)
{
	switch (operand.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
	{
		// get register info
		VEXA::reg_t reg_id = X64::zydisToVexaReg[operand.reg.value];
		WriteRegister(reg_id, value);
		return;
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
		throw std::runtime_error("Memory operand is not implemented yet");
	default:
		throw std::runtime_error("Unimplemented operand: " + operand.type);
		return;
	}
}

llvm::Value* VEXA::Lifter::GetOperand(ZydisDecodedOperand operand)
{
	switch (operand.type)
	{
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER:
	{
		VEXA::reg_t reg_id = X64::zydisToVexaReg[operand.reg.value];
		return ReadRegister(reg_id);
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_IMMEDIATE:
	{
		llvm::Type* target_type = llvm::Type::getIntNTy(builder->getContext(), operand.size);
		return llvm::ConstantInt::get(target_type, operand.imm.value.u);
	}
	case ZydisOperandType::ZYDIS_OPERAND_TYPE_MEMORY:
		throw std::runtime_error("Memory operand is not implemented yet");
	default:
		throw std::runtime_error("Unimplemented operand type!");
		break;
	}
}

llvm::Value* VEXA::Lifter::ReadRegister(VEXA::reg_t reg)
{
	VEXA::Value inEngineVal = symEngine->ReadRegister(reg);
	X64::RegInfo reg_info = X64::reg_info.at(reg);

	// we are not sure about this part yet
	if (inEngineVal.type() == VEXA::ValueType::CONCRETE) // inEngineVal.type() == VEXA::ValueType::CONCRETE
	{
		// if the value could be resolved by symex engine, use constant value directly
		llvm::Type* target_type = llvm::Type::getIntNTy(builder->getContext(), reg_info.size_bits);
		return llvm::ConstantInt::get(target_type, inEngineVal.as_int64());
	}
	else
	{
		// else, we must lift the access to the register
		llvm::Value* base_reg = GetLLVMRegister(vexaToLLVMRegId[reg_info.base_id]);

		if (reg_info.size_bits == 64)
			return base_reg;

		llvm::Value* shifted = builder->CreateLShr(
			base_reg,
			llvm::ConstantInt::get(builder->getInt64Ty(), reg_info.offset_bits),
			std::string("shifted_") + X64::reg_to_str.at(reg));

		llvm::Type* target_type = llvm::Type::getIntNTy(*llvm_context, reg_info.size_bits);
		return builder->CreateTrunc(shifted, target_type, X64::reg_to_str.at(reg));
	}
}

void VEXA::Lifter::WriteRegister(VEXA::reg_t reg, llvm::Value* value)
{
	X64::RegInfo reg_info = X64::reg_info.at(reg);
	llvm::Value* base_reg_value = GetLLVMRegister(vexaToLLVMRegId[reg_info.base_id]);

	llvm::Type* base_type = llvm::Type::getIntNTy(*llvm_context, 64);
	llvm::Value* value_64 = builder->CreateZExtOrTrunc(value, base_type, X64::reg_to_str.at(reg_info.base_id));

	std::string base_reg_name = X64::reg_to_str.at(reg_info.base_id);

	if (reg_info.size_bits == 64)
	{
		SetLLVMRegister(vexaToLLVMRegId[reg_info.base_id], value_64);
		return;
	}

	// if register is 32 bits, reset upper 32 bits
	if (reg_info.size_bits == 32)
	{
		uint64_t clear_mask_upper = 0xFFFFFFFFULL;
		std::stringstream upmask_hex_str;
		upmask_hex_str << std::hex << clear_mask_upper;

		base_reg_value = builder->CreateAnd(
			base_reg_value,
			llvm::ConstantInt::get(base_type, clear_mask_upper),
			base_reg_name + "_" + upmask_hex_str.str());
	}

	// TODO: learn
	uint64_t size_mask = (1ULL << reg_info.size_bits) - 1;
	uint64_t clear_mask = ~(size_mask << reg_info.offset_bits);

	std::stringstream mask_hex_str;
	mask_hex_str << std::hex << clear_mask;

	llvm::Value* masked_old_val = builder->CreateAnd(
		base_reg_value,
		llvm::ConstantInt::get(base_type, clear_mask),
		base_reg_name + "_" + mask_hex_str.str());

	llvm::Value* shifted_new_val = builder->CreateShl(
		value_64,
		llvm::ConstantInt::get(base_type, reg_info.offset_bits),
		"shifted_" + base_reg_name);

	llvm::Value* final_value = builder->CreateOr(masked_old_val, shifted_new_val, base_reg_name);
	SetLLVMRegister(vexaToLLVMRegId[reg_info.base_id], final_value);
}

void VEXA::Lifter::Optimize()
{
	TRY()
		if (!this->module)
			throw std::runtime_error("Module is null!");

	if (llvm::verifyModule(*this->module, &llvm::errs()))
		throw std::runtime_error("Invalid IR before optimization!");

	llvm::LoopAnalysisManager LAM;
	llvm::FunctionAnalysisManager FAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::ModuleAnalysisManager MAM;
	llvm::PassBuilder PB;

	static bool passes_registered = false;
	if (!passes_registered) {
		PB.registerModuleAnalyses(MAM);
		PB.registerCGSCCAnalyses(CGAM);
		PB.registerFunctionAnalyses(FAM);
		PB.registerLoopAnalyses(LAM);
		PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
		passes_registered = true;
	}

	llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(
		llvm::OptimizationLevel::O3);

	MPM.run(*this->module, MAM);
	CATCH("Lifter error")
}

void VEXA::Lifter::PrintIR()
{
	module->print(llvm::outs(), nullptr);
}

std::string VEXA::Lifter::InstrToBrName(std::string text)
{
	std::string processed_text = text;

	std::replace(processed_text.begin(), processed_text.end(), ' ', '_');

	processed_text.erase(
		std::remove(processed_text.begin(), processed_text.end(), ','),
		processed_text.end()
	);

	return processed_text;
}

std::string VEXA::Lifter::GetBlockNameFromInstr(ZydisDisassembledInstruction instruction, int id)
{
	std::stringstream bb_adress_stream;
	bb_adress_stream << std::hex << instruction.runtime_address;
	std::string bb_name = bb_adress_stream.str() + std::string("_") + InstrToBrName(instruction.text) +
		"_" + std::to_string(id);
	return bb_name;
}