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
#include "TupleExpression.hpp"

//Public methods
IR::Symbol TupleExpression::Compile(IR::Block & block, const ExpressionCompileFlags & flags) const
{
	NOT_IMPLEMENTED_ERROR;
	return IR::Symbol::Temporary(0);
}

void TupleExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	NOT_IMPLEMENTED_ERROR;
}

IR::Type TupleExpression::GetType() const
{
	NOT_IMPLEMENTED_ERROR;
	return IR::Type(IR::LeafType::Any);
}