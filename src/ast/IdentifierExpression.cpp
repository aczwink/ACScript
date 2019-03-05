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
#include "IdentifierExpression.hpp"
//Local
#include "../ir/Program.hpp"

//Public methods
IR::Symbol IdentifierExpression::Compile(IR::Block& block, const ExpressionCompileFlags& flags) const
{
	if (flags.isMember)
	{
		return block.GetProgram().AddConstant(this->identifier);
	}

	if (flags.declare)
	{
		return block.AddLocalVariable(this->identifier, flags.mutableFlag);
	}

	return block.DeriveIdentifierSymbol(this->identifier);

	/*
	RuntimeObject obj(this->identifier);
	p.AddLoadConstant(obj);
	if(!this->isLeftValue)
		p.AddInstruction(Opcode::Access);
		*/
	//NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

void IdentifierExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	NOT_IMPLEMENTED_ERROR;
	//TODO: this is only true, if we don't refer to a constant or so

	/*
	//no condition, but assign to variable
	p.AddInstruction(Opcode::Push, 0); //push func-arg to stack, i.e.: copy it
	RuntimeObject obj(this->identifier);
	p.AddLoadConstant(obj);
	if (localAssign)
		p.AddInstruction(Opcode::AssignLocal);
	else
		p.AddInstruction(Opcode::Assign);
	
	//simply a variable... no condition
	p.AddInstruction(Opcode::PushBool, true); //unpacking went fine
	*/
}

IR::Type IdentifierExpression::GetType() const
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return IR::Type(IR::LeafType::Any);
}
