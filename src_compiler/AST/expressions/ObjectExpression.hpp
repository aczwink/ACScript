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
//Local
#include "Expression.hpp"

namespace AST
{
	class ObjectExpression : public Expression
	{
	public:
		//Constructor
		inline ObjectExpression(const String& memberName)
		{
			this->members[memberName] = nullptr;
		}

		inline ObjectExpression(const String& memberName, UniquePointer<Expression>&& expr)
		{
			this->members[memberName] = Move(expr);
		}

		//Properties
		inline const Map<String, UniquePointer<Expression>>& Members() const
		{
			return this->members;
		}

		//Methods
		void Visit(ExpressionVisitor &visitor) const override
		{
			visitor.OnVisitingObjectExpression(*this);
		}

		//Inline
		inline void AddMember(UniquePointer<ObjectExpression>&& objectExpression)
		{
			auto it = objectExpression->members.begin();
			this->members[(*it).key] = Move((*it).value);
			objectExpression->members.Release();
		}

	private:
		//Members
		Map<String, UniquePointer<Expression>> members;
	};
}