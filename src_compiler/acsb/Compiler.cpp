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
void Compiler::OnVisitedNewTupleInstruction(const CreateNewTupleInstruction &createNewTupleInstruction)
{
	this->AddInstruction(Opcode::NewTuple, createNewTupleInstruction.Values().GetNumberOfElements());
}

void Compiler::OnVisitingConstant(const ConstantFloat &constantFloat)
{
	this->constants.Push(constantFloat.Value());

	this->AddInstruction(Opcode::LoadConstant, this->AddConstant(constantFloat.Value()));
}

void ACSB::Compiler::OnVisitingExternal(const IR::External& external)
{
	uint16 idx = this->externals.Push(external.name);
	this->externalMap[&external] = idx;
}

void ACSB::Compiler::OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction)
{
	this->AddInstruction(Opcode::CallExtern, this->externalMap[externalCallInstruction.external]);
}

void ACSB::Compiler::OnVisitingNewTupleInstruction(const IR::CreateNewTupleInstruction &createNewTupleInstruction)
{
}

void ACSB::Compiler::OnVisitingProcedure(const IR::Procedure &procedure)
{
}

void Compiler::OnVisitingReturnInstruction(const ReturnInstruction &returnInstruction)
{
	this->AddInstruction(Opcode::Return);
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

	this->codeSegment.FlushTo(outputStream);
}