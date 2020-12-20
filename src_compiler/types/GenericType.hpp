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
#include "Type.hpp"

class GenericType : public ::Type
{
public:
	//Constructor
	inline GenericType(uint32 number) : number(number)
	{
	}

	//Methods
	bool IsTriviallyAssignableTo(const Type &other) const override
	{
		return this == &other;
	}

	String ToString() const override
	{
		return u8"T" + String::Number(this->number);
	}

	void Visit(TypeVisitor &visitor) const override
	{
		visitor.OnVisitingGenericType(*this);
	}

private:
	//Members
	uint32 number;
};