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
#include "BinaryExpression.hpp"

IR::Symbol BinaryExpression::Compile(IR::Block& b, const ExpressionCompileFlags& flags) const
{
	/*
	this->left->Compile(p);
	this->right->Compile(p);

	switch (this->op)
	{
	case Token::LessOrEqual:
		p.AddInstruction(Opcode::LessOrEqual);
		break;
	case Token::Minus:
		p.AddInstruction(Opcode::Minus);
		break;
	case Token::Multiply:
		p.AddInstruction(Opcode::Multiply);
		break;
	case Token::Plus:
		p.AddInstruction(Opcode::Add);
		break;
	default:
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}*/
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return IR::Symbol::Temporary(0);
}

void BinaryExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

IR::Type BinaryExpression::GetType() const
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return IR::Type(IR::LeafType::Any);
}
