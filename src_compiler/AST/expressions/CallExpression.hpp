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
//Local
#include "TupleExpression.hpp"

class CallExpression : public Expression
{
public:
	//Constructor
	inline CallExpression(const String& functionName, UniquePointer<TupleExpression>&& argument)
		: functionName(functionName), arg(Move(argument))
	{
	}

	//Properties
	inline const TupleExpression& Argument() const
	{
		return *this->arg;
	}

	inline const String& FunctionName() const
	{
		return this->functionName;
	}

	//Methods
	void Visit(ASTVisitor& visitor) const
	{
		this->arg->Visit(visitor);
		visitor.OnVisitedCall(*this);
	}

private:
	//Members
	String functionName;
	UniquePointer<TupleExpression> arg;
};