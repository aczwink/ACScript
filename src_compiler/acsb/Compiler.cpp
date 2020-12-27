/*
* Copyright (c) 2019-2020 Amir Czwink (amir130@hotmail.de)
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
//Namespaces
using namespace ACSB;
using namespace IR;

//Public methods
void Compiler::Compile(const Module &module)
{
	this->module = &module;

	for(const auto& proc : module.Procedures())
	{
		Optimization::DependencyGraph dependencyGraph(*proc);
		for(const auto& kv : dependencyGraph.Dependencies())
			this->instructionReferenceCounts[kv.key] = kv.value.GetNumberOfElements();

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

void Compiler::Write(OutputStream &outputStream)
{
	DataWriter dataWriter(true, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::UTF8);

	textWriter.WriteString(u8"ACSB");

	dataWriter.WriteUInt16(this->externals.GetNumberOfElements());
	for(const String& externalName : this->externals)
		textWriter.WriteStringZeroTerminated(externalName);

	dataWriter.WriteUInt16(this->constants.GetNumberOfElements());
	for(const Constant& constant : this->constants)
	{
		dataWriter.WriteByte(constant.isString);
		if(constant.isString)
			textWriter.WriteStringZeroTerminated(constant.string);
		else
			dataWriter.WriteFloat64(constant.f64);
	}

	dataWriter.WriteUInt16(this->procedureOffsetMap[u8"main"]);

	FixedSizeBuffer tmpBuffer(this->codeSegment.GetSize());
	this->codeSegment.ReadBytes(tmpBuffer.Data(), tmpBuffer.Size());
	BufferOutputStream bufferOutputStream(tmpBuffer.Data(), tmpBuffer.Size());
	DataWriter bufferDataWriter(true, bufferOutputStream);

	for(const auto& kv: this->missingBlockOffsets)
	{
		uint16 blockOffset = this->blockOffsets[kv.key];
		for(uint16 offsetToWrite : kv.value)
		{
			bufferOutputStream.SeekTo(offsetToWrite);
			bufferDataWriter.WriteUInt16(blockOffset);
		}
	}

	outputStream.WriteBytes(tmpBuffer.Data(), tmpBuffer.Size());
}

//Private methods
void Compiler::PushValue(const IR::Value* value)
{
	uint32 offset = Unsigned<uint32>::Max();
	for (int32 idx = this->valueStack.GetNumberOfElements() - 1; idx >= 0; idx--)
	{
		if (this->valueStack[idx] == value)
		{
			offset = this->valueStack.GetNumberOfElements() - 1 - idx;
			break;
		}
	}
	ASSERT(offset != Unsigned<uint32>::Max(), u8"Value not found on stack");

	if(--this->instructionReferenceCounts[value] > 0)
	{
		this->AddInstruction(Opcode::Push, offset);
		this->valueStack.Push(this->valueStack[this->valueStack.GetNumberOfElements() - 1 - offset]);
	}
}

//Event handlers
void Compiler::OnVisitingCallInstruction(CallInstruction &callInstruction)
{
	callInstruction.argument->Visit(*this);

	const Procedure* procedure = dynamic_cast<const Procedure *>(callInstruction.function);
	this->AddInstruction(Opcode::Call, this->procedureOffsetMap[procedure->name]);

	this->valueStack.Pop();
	this->valueStack.Push(&callInstruction);
}

void Compiler::OnVisitingConditionalBranchInstruction(const BranchOnTrueInstruction &branchOnTrueInstruction)
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

void Compiler::OnVisitingNewObjectInstruction(const CreateNewObjectInstruction &createNewObjectInstruction)
{
	this->AddInstruction(Opcode::NewDictionary);
	const IR::Value* dictInstance = &createNewObjectInstruction;
	this->valueStack.Push(dictInstance);

	const External* setter = this->module->FindExternal(u8"__set");

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

void Compiler::OnVisitingInstructionResultValue(const Instruction &instruction)
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
