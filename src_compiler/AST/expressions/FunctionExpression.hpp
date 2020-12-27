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
#include "Expression.hpp"
#include "../statements/StatementBlock.hpp"
#include "../statements/ReturnStatement.hpp"

namespace AST
{
	class FunctionExpression : public Expression
	{
	public:
		//Constructor
		inline FunctionExpression(UniquePointer<Expression>&& pattern, UniquePointer<Expression>&& guard, UniquePointer<Expression>&& expr)
		{
			this->AddRule(Move(pattern), Move(guard), Move(expr));
		}

		//Properties
		inline const LinkedList<Tuple<UniquePointer<Expression>, UniquePointer<Expression>, StatementBlock>>& Rules() const
		{
			return this->rules;
		}

		//Inline
		inline void AddRule(UniquePointer<Expression>&& parameter, UniquePointer<Expression>&& guard, UniquePointer<Expression>&& expr)
		{
			StatementBlock statementBlock;
			statementBlock.AddStatement(new ReturnStatement(Move(expr)));
			this->rules.InsertFront({ Move(parameter), Move(guard), Move(statementBlock) });
		}

		inline void CombineRulesAndPrepend(UniquePointer<FunctionExpression> functionExpression)
		{
			while (!functionExpression->Rules().IsEmpty())
			{
				this->rules.InsertFront(functionExpression->rules.PopTail());
			}
		}

		void Visit(ExpressionVisitor &visitor) const override
		{
			visitor.OnVisitingFunctionExpression(*this);
		}

	private:
		//Members
		LinkedList<Tuple<UniquePointer<Expression>, UniquePointer<Expression>, StatementBlock>> rules;
	};
}