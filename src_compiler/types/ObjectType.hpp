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
#include "LeafType.hpp"

class ObjectType : public ::Type
{
public:
	//Constructor
	inline ObjectType(Map<String, const ::Type*>&& types) : types(Move(types))
	{
		for(const auto& kv : this->types)
		{
			this->typesOrdered.Push(kv.key);
		}
	}

	//Properties
	inline const DynamicArray<String>& TypesOrdered() const
	{
		return this->typesOrdered;
	}

	//Methods
	bool IsTriviallyAssignableTo(const Type &other) const override
	{
		const LeafType* otherLeafType = dynamic_cast<const LeafType *>(&other);
		if(otherLeafType && (otherLeafType->EnumType() == LeafTypeEnum::Any))
			return true;

		NOT_IMPLEMENTED_ERROR;
		return false;
	}

	String ToString() const override
	{
		DynamicArray<String> strings;
		for(const auto& kv : this->types)
			strings.Push(kv.key + u8": " + TypePointerToString(kv.value));

		return u8"{ " + String::Join(strings, u8", ") + u8" }";
	}

	void Visit(TypeVisitor &visitor) const override
	{
		NOT_IMPLEMENTED_ERROR;
	}

private:
	//Members
	Map<String, const ::Type*> types;
	DynamicArray<String> typesOrdered;
};