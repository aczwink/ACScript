/*
* Copyright (c) 2018-2020 Amir Czwink (amir130@hotmail.de)
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
#pragma once
//Local
#include "Statement.hpp"
#include "../ASTVisitor.hpp"
#include "../ASTNode.hpp"

class StatementBlock : public ASTNode
{
public:
	//Inline
	inline void AddStatement(UniquePointer<Statement>&& stmt)
	{
		this->statements.InsertTail(Move(stmt));
	}

	inline void Visit(ASTVisitor& visitor) const
	{
		for (const UniquePointer<Statement>& statement : this->statements)
			statement->Visit(visitor);
		visitor.OnVisitedBlock(*this);
	}
private:
	//Members
	LinkedList<UniquePointer<Statement>> statements;
};