/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "StoreOperation.hpp"
//Local
#include "Compiler.hpp"
#include "Program.hpp"
//Namespaces
using namespace IR;

//Public methods
void IR::StoreOperation::Compile(Compiler & compiler) const
{
	compiler.CompileStore(*this);
}

void IR::StoreOperation::ReplaceSymbol(const Symbol & from, const Symbol & to)
{
	if (this->lhs == from)
		this->lhs = to;
	if (this->rhs == from)
		this->rhs = to;
}

String StoreOperation::ToString() const
{
	return this->lhs.ToString() + u8" := " + this->rhs.ToStringWithoutType();
}

void IR::StoreOperation::Verify() const
{
	switch (this->lhs.GetSymbolType())
	{
	case SymbolType::GlobalVariable:
		if (!this->GetProgram().GlobalIsMutable(this->lhs.GetIdentifier()))
			throw(u8"Can't assign to immutable variable '" + this->lhs.GetIdentifier() + u8"'.");
		break;
	}
}
