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
#include "AccessExpression.hpp"

//Public methods
IR::Symbol AccessExpression::Compile(IR::Block & block, const ExpressionCompileFlags & flags) const
{
	IR::Symbol container = this->container->Compile(block, flags);

	ExpressionCompileFlags accessorFlags;
	accessorFlags.isLeftValue = flags.isLeftValue;
	accessorFlags.isMember = true;
	IR::Symbol accessor = this->accessor->Compile(block, accessorFlags);
	return block.AddAccess(Move(container), Move(accessor));
}

void AccessExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	NOT_IMPLEMENTED_ERROR;
}

IR::Type AccessExpression::GetType() const
{
	NOT_IMPLEMENTED_ERROR;
	return IR::Type(IR::LeafType::Any);
}
