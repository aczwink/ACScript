/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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

	//Properties
	inline const auto& MemberConstraints() const
	{
		return this->memberConstraints;
	}

	inline void MemberConstraints(Map<String, const ::Type*>&& memberConstraints)
	{
		this->memberConstraints = Move(memberConstraints);
	}

	//Methods
	bool IsTriviallyAssignableTo(const Type &other) const override
	{
		return this == &other;
	}

	String ToString() const override
	{
		String result = u8"T" + String::Number(this->number);

		if(!this->memberConstraints.IsEmpty())
		{
			DynamicArray<String> strings;
			for(const auto& kv : this->memberConstraints)
				strings.Push(kv.key + u8": " + TypePointerToString(kv.value));

			result += u8"{ " + String::Join(strings, u8", ") + u8" }";
		}
		return result;
	}

	void Visit(TypeVisitor &visitor) const override
	{
		visitor.OnVisitingGenericType(*this);
	}

	//Inline
	inline void AddMemberConstraint(const String& memberName, const ::Type* memberType)
	{
		this->memberConstraints.Insert(memberName, memberType);
	}

	inline const ::Type* GetMemberConstraint(const String& memberName) const
	{
		return this->memberConstraints[memberName];
	}

private:
	//Members
	uint32 number;
	Map<String, const ::Type*> memberConstraints;
};