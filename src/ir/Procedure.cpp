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
#include "Procedure.hpp"
//Local
#include "Compiler.hpp"
//Namespaces
using namespace IR;

//Methods
void Procedure::Compile(Compiler& compiler)
{
	compiler.BeginProcedure();
	this->block.Compile(compiler);
	compiler.EndProcedure();
}

String Procedure::ToString() const
{
	String result = u8"PROCEDURE " + this->name + u8": " + this->resultType.ToString() + u8"\r\n{\r\n";
	result += this->block.ToString(1);
	result += u8"}\r\n";

	return result;
}