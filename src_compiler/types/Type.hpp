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
#include <StdXX.hpp>
using namespace StdXX;
#include "TypeVisitor.hpp"

class Type
{
public:
	//Destructor
	virtual ~Type(){}

	//Abstract
	virtual bool IsTriviallyAssignableTo(const Type& other) const = 0;
	virtual String ToString() const = 0;
	virtual void Visit(TypeVisitor& visitor) const = 0;
};

inline String TypePointerToString(const ::Type* type)
{
	if(type)
		return type->ToString();
	return u8"unknown";
}