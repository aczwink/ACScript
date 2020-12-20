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
//Namespaces
using namespace ACSB;
using namespace IR;

//Public methods
void Compiler::Compile(const Module &module)
{
	for(const auto& proc : module.Procedures())
	{
		this->procedureOffsetMap[proc->name] = this->codeSegment.GetRemainingBytes();
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
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	textWriter.WriteString(u8"ACSB");

	dataWriter.WriteUInt16(this->externals.GetNumberOfElements());
	for(const String& externalName : this->externals)
		textWriter.WriteStringZeroTerminated(externalName);

	dataWriter.WriteUInt16(this->constants.GetNumberOfElements());
	for(float64 constant : this->constants)
		dataWriter.WriteFloat64(constant);

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

//Event handlers
void Compiler::OnVisitingCallInstruction(CallInstruction &callInstruction)
{
	callInstruction.argument->Visit(*this);

	const Procedure* procedure = dynamic_cast<const Procedure *>(callInstruction.function);
	this->AddInstruction(Opcode::Call, this->procedureOffsetMap[procedure->name]);
}

void Compiler::OnVisitingConditionalBranchInstruction(const BranchOnTrueInstruction &branchOnTrueInstruction)
{
	this->missingBlockOffsets[branchOnTrueInstruction.ElseBlock()].Push(this->codeSegment.GetRemainingBytes() + 1);
	this->AddInstruction(Opcode::JumpOnFalse, 0);
}

void ACSB::Compiler::OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction)
{
	externalCallInstruction.external->Visit(*this);
	externalCallInstruction.argument->Visit(*this);

	this->AddInstruction(Opcode::CallExtern, this->externalMap[externalCallInstruction.external]);
}

void ACSB::Compiler::OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction)
{
	for(uint32 i = 0; i < createNewTupleInstruction.Values().GetNumberOfElements(); i++)
		createNewTupleInstruction.Values()[createNewTupleInstruction.Values().GetNumberOfElements() - 1 - i]->Visit(*this);
	this->AddInstruction(Opcode::NewTuple, createNewTupleInstruction.Values().GetNumberOfElements());
}

void Compiler::OnVisitingReturnInstruction(const ReturnInstruction &returnInstruction)
{
	returnInstruction.returnValue->Visit(*this);

	this->AddInstruction(Opcode::Return);
}

void Compiler::OnVisitingConstantFloat(const ConstantFloat &constantFloat)
{
	this->AddInstruction(Opcode::LoadConstant, this->AddConstant(constantFloat.Value()));
}

void Compiler::OnVisitingExternal(const External &external)
{
	if(this->externalMap.Contains(&external))
		return;

	uint16 idx = this->externals.Push(external.name);
	this->externalMap[&external] = idx;
}

void Compiler::OnVisitingParameter(const Parameter &parameter)
{
	this->AddInstruction(Opcode::PushParameter);
}