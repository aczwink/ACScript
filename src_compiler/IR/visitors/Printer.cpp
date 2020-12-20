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
//this->textWriter << constantFloat.name << u8" := " << constantFloat.Value() << endl;
void Printer::Print(const Module &module)
{
	for(const auto& kv : module.Externals())
	{
		this->textWriter << kv.value->ToString() << endl;
	}

	this->textWriter << endl;

	for(Procedure*const& procedure : module.Procedures())
	{
		this->textWriter << u8"procedure " << procedure->ToString() << endl;
		for(const auto& basicBlock : procedure->BasicBlocks())
		{
			this->textWriter << basicBlock->Name() << u8":" << endl;
			for(const Instruction*const& instruction : basicBlock->Instructions())
				this->PrintInstruction(*instruction);
		}
		this->textWriter << endl;
	}
}