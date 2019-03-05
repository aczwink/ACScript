/*
* Copyright (c) 2018 Amir Czwink (amir130@hotmail.de)
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
#include "IntLiteralExpression.hpp"
//Local
#include "../ir/Program.hpp"

//Public methods
IR::Symbol IntLiteralExpression::Compile(IR::Block& block, const ExpressionCompileFlags& flags) const
{
	if (this->value < 0)
		return block.GetProgram().AddConstant(this->value);
	return block.GetProgram().AddConstant(uint64(this->value));
}

void IntLiteralExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	/*
	p.AddInstruction(Opcode::Push, 0); //push func-arg to stack, i.e.: copy it
	this->Compile(p);
	p.AddInstruction(Opcode::Equals);
	*/
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

IR::Type IntLiteralExpression::GetType() const
{
	if (value >= 0)
		return IR::Type(IR::LeafType::Any);
	return IR::Type(IR::LeafType::Any);
}