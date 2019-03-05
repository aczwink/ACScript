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
#include "CallOperation.hpp"
//Local
#include "Compiler.hpp"

//Public methods
void IR::CallOperation::Compile(Compiler & compiler) const
{
	compiler.CompileCall(*this);
}

void IR::CallOperation::ReplaceSymbol(const Symbol & from, const Symbol & to)
{
	if (this->arg == from)
		this->arg = to;
	if (this->function == from)
		this->function = to;
}

String IR::CallOperation::ToString() const
{
	return this->GetValue().ToString() + u8" := call context: " + this->context.ToString() + u8", function: " + this->function.ToString() + u8", arg: " + this->arg.ToString();
}

void IR::CallOperation::Verify() const
{
}
