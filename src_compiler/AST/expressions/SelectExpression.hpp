/*
* Copyright (c) 2021 Amir Czwink (amir130@hotmail.de)
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
#include "Expression.hpp"

namespace AST
{
	class SelectExpression : public Expression
	{
	public:
		//Constructor
		inline SelectExpression(UniquePointer<class Expression>&& expr, const String& memberName)
			: expr(Move(expr)), memberName(memberName)
		{
		}

		//Properties
		inline const Expression& Expression() const
		{
			return *this->expr;
		}

		inline const String& MemberName() const
		{
			return this->memberName;
		}

		//Methods
		void Visit(ExpressionVisitor &visitor) const override
		{
			visitor.OnVisitingSelectExpression(*this);
		}

	private:
		UniquePointer<class Expression> expr;
		String memberName;
	};
}