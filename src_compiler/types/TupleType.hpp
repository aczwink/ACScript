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

class TupleType : public ::Type
{
public:
	//Constructors
	inline TupleType()
	{
		this->lastArgIsVariadic = false;
	}

	inline TupleType(DynamicArray<const ::Type*>&& types, bool lastArgIsVariadic) : types(Move(types)), lastArgIsVariadic(lastArgIsVariadic)
	{
	}

	//Properties
	inline const DynamicArray<const ::Type*>& Types() const
	{
		return this->types;
	}

	//Methods
	bool IsTriviallyAssignableTo(const Type& other) const override
	{
		const TupleType* rightTupleType = dynamic_cast<const TupleType *>(&other);
		if(rightTupleType)
		{
			if(this->types.GetNumberOfElements() != rightTupleType->types.GetNumberOfElements())
				return false;
			for(uint32 i = 0; i < this->types.GetNumberOfElements(); i++)
			{
				const ::Type* t1 = this->types[i];
				const ::Type* t2 = rightTupleType->types[i];
				if(t1 && !t1->IsTriviallyAssignableTo(*t2))
					return false;
			}
			return true;
		}

		return false;
	}

	String ToString() const override
	{
		DynamicArray<String> strings;
		for(const ::Type*const& type : this->types)
			strings.Push(TypePointerToString(type));
		return u8"(" + String::Join(strings, u8" x ") + (this->lastArgIsVariadic ? u8"..." : u8"") + u8")";
	}

	void Visit(TypeVisitor &visitor) const override
	{
		visitor.OnVisitingTupleType(*this);
	}

private:
	//Members
	DynamicArray<const ::Type*> types;
	bool lastArgIsVariadic;
};