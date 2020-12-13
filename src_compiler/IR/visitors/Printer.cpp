/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#include "Printer.hpp"
//Local
#include "AllSymbols.hpp"
//Namespaces
using namespace IR;

//Public methods
void Printer::OnVisitedNewTupleInstruction(const CreateNewTupleInstruction &createNewTupleInstruction)
{
}

void Printer::OnVisitingConstant(const ConstantFloat &constantFloat)
{
	//this->textWriter << constantFloat.name << u8" := " << constantFloat.Value() << endl;
}

void Printer::OnVisitingExternal(const External &external)
{
	this->textWriter << external.ToString() << endl;
}

void Printer::OnVisitingExternalCallInstruction(const ExternalCallInstruction &externalCallInstruction)
{
	this->PrintInstruction(externalCallInstruction);
}

void Printer::OnVisitingNewTupleInstruction(const CreateNewTupleInstruction &createNewTupleInstruction)
{
	this->PrintInstruction(createNewTupleInstruction);
}

void Printer::OnVisitingProcedure(const Procedure &procedure)
{
	this->textWriter << procedure.ToString() << endl;
}

void Printer::OnVisitingReturnInstruction(const ReturnInstruction &returnInstruction)
{
	this->PrintInstruction(returnInstruction);
}
