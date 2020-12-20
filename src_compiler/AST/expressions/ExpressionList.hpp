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
#pragma once
#include "../Node.hpp"

namespace AST
{
	class ExpressionList : public Node
	{
	public:
		//Constructors
		inline ExpressionList(UniquePointer<Expression> &&expr)
		{
			this->expressions.InsertTail(Move(expr));
		}

		inline ExpressionList(UniquePointer<Expression> &&expr1, UniquePointer<Expression> &&expr2)
		{
			this->expressions.InsertTail(Move(expr1));
			this->expressions.InsertTail(Move(expr2));
		}

		//Properties
		inline const LinkedList<UniquePointer<Expression>> &Expressions() const
		{
			return this->expressions;
		}

		//Inline
		inline void InsertAtBeginning(UniquePointer<Expression> &&expr)
		{
			this->expressions.InsertFront(Move(expr));
		}

	private:
		//Members
		LinkedList<UniquePointer<Expression>> expressions;
	};
}