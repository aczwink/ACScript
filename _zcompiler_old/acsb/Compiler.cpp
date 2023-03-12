/*
* Copyright (c) 2019-2021 Amir Czwink (amir130@hotmail.de)
*
* This file is part of ACScript.
*
* ACScript is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ACScript is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ACScript.  If not, see <http://www.gnu.org/licenses/>.
*/
//Class header
#include "Compiler.hpp"
//Local
#include "../IR/visitors/AllSymbols.hpp"
#include "../types/ObjectType.hpp"
#include "../optimization/BackwardsEvaluator.hpp"
//Namespaces
using namespace ACSB;
using namespace IR;

//Public methods
void Compiler::Compile(Module &module)
{
	this->module = &module;

	for(const auto& proc : module.Procedures())
	{
		this->procedureOffsetMap[proc->name] = this->codeSegment.GetRemainingBytes();

		this->valueStack.Release();
		this->valueStack.Push(proc->parameter);

		for(const auto& basicBlock : proc->BasicBlocks())
		{
			this->blockOffsets[basicBlock] = this->codeSegment.GetRemainingBytes();
			basicBlock->Visit(*this);
		}
	}
}

//Event handlers
void Compiler::OnVisitingCallInstruction(CallInstruction &callInstruction)
{
	callInstruction.argument->Visit(*this);
	callInstruction.function->Visit(*this);

	this->AddInstruction(Opcode::Call);

	this->valueStack.Pop();
	this->valueStack.Pop();
	this->valueStack.Push(&callInstruction);
}

void Compiler::OnVisitingConditionalBranchInstruction(BranchOnTrueInstruction &branchOnTrueInstruction)
{
	branchOnTrueInstruction.Condition()->Visit(*this);

	this->missingBlockOffsets[branchOnTrueInstruction.ElseBlock()].Push(this->codeSegment.GetRemainingBytes() + 1);
	this->AddInstruction(Opcode::JumpOnFalse, 0);

	this->valueStack.Pop();
}

void ACSB::Compiler::OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction)
{
	externalCallInstruction.external->Visit(*this);
	externalCallInstruction.argument->Visit(*this);

	this->AddInstruction(Opcode::CallExtern, this->externalMap[externalCallInstruction.external]);

	this->valueStack.Pop();
	this->valueStack.Push(&externalCallInstruction);
}

void Compiler::OnVisitingNewObjectInstruction(CreateNewObjectInstruction &createNewObjectInstruction)
{
	this->AddInstruction(Opcode::NewDictionary);
	const IR::Value* dictInstance = &createNewObjectInstruction;
	this->valueStack.Push(dictInstance);

	External* setter = this->module->FindExternal(u8"__set");

	for(const auto& kv : createNewObjectInstruction.Members())
	{
		DynamicArray<Value*> values;
		values.Push(const_cast<Value*>(dictInstance));

		ConstantString constantString(kv.key);
		values.Push(&constantString);

		values.Push(kv.value);

		IR::CreateNewTupleInstruction argInstruction(Move(values));
		argInstruction.Visit(*this);

		IR::ExternalCallInstruction externalCallInstruction(setter, &argInstruction);
		externalCallInstruction.Visit(*this);

		this->AddInstruction(Opcode::Pop);
		this->valueStack.Pop();
	}
}

void ACSB::Compiler::OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction)
{
	for(uint32 i = 0; i < createNewTupleInstruction.Values().GetNumberOfElements(); i++)
		createNewTupleInstruction.Values()[createNewTupleInstruction.Values().GetNumberOfElements() - 1 - i]->Visit(*this);

	this->AddInstruction(Opcode::NewTuple, createNewTupleInstruction.Values().GetNumberOfElements());

	for(uint32 i = 0; i < createNewTupleInstruction.Values().GetNumberOfElements(); i++)
		this->valueStack.Pop();
	this->valueStack.Push(&createNewTupleInstruction);
}

void Compiler::OnVisitingReturnInstruction(ReturnInstruction &returnInstruction)
{
	returnInstruction.returnValue->Visit(*this);

	//remove spilled
	bool hasReturn = true;//this->currentProcedure->name != u8"main";
	while (this->valueStack.GetNumberOfElements() > (hasReturn ? 1 : 0))
		this->AddPopAssignInstruction();

	this->AddInstruction(Opcode::Return);

	if(hasReturn)
		this->valueStack.Pop();
}

void Compiler::OnVisitingSelectInstruction(SelectInstruction &selectInstruction)
{
	selectInstruction.inner->Visit(*this);
	selectInstruction.selector->Visit(*this);

	this->AddInstruction(Opcode::Select);

	this->valueStack.Pop();
	this->valueStack.Pop();
	this->valueStack.Push(&selectInstruction);
}

void Compiler::OnVisitingConstantFloat(const ConstantFloat &constantFloat)
{
	this->AddInstruction(Opcode::LoadConstant, this->AddConstant(constantFloat.Value()));

	this->valueStack.Push(&constantFloat);
}

void Compiler::OnVisitingConstantString(const ConstantString &constantString)
{
	this->AddInstruction(Opcode::LoadConstant, this->AddConstant(constantString.Value()));

	this->valueStack.Push(&constantString);
}

void Compiler::OnVisitingExternal(const External &external)
{
	if(this->externalMap.Contains(&external))
		return;

	uint16 idx = this->externals.Push(external.name);
	this->externalMap[&external] = idx;
}

void Compiler::OnVisitingInstructionResultValue(Instruction &instruction)
{
	this->PushValue(&instruction);
}

void Compiler::OnVisitingParameter(const Parameter &parameter)
{
	this->PushValue(&parameter);
}

void Compiler::OnVisitingProcedure(const Procedure &procedure)
{
	this->AddInstruction(Opcode::LoadConstant, this->AddConstant(this->procedureOffsetMap[procedure.name]));
	this->valueStack.Push(&procedure);
}
