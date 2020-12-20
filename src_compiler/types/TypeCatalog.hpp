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
#include "TupleType.hpp"
#include "FunctionType.hpp"
#include "GenericType.hpp"

class TypeCatalog
{
public:
	//Inline
	inline GenericType* CreateGenericType()
	{
		GenericType* genericType = new GenericType(this->genericTypes.GetNumberOfElements());
		this->genericTypes.Push(genericType);
		return genericType;
	}

	inline const ::TupleType* GetEmptyTupleType()
	{
		return dynamic_cast<const TupleType *>(this->FindOrInsert(new TupleType()));
	}

	const ::Type* GetFunctionType(const ::Type* returnType, const ::Type* argumentType)
	{
		return this->FindOrInsert(new FunctionType(returnType, argumentType));
	}

	inline const ::Type* GetLeafType(LeafTypeEnum leafType)
	{
		String key = LeafType(leafType).ToString();

		if(!this->types.Contains(key))
			this->types.Insert(key, new LeafType(leafType));
		return this->types[key].operator->();
	}

	inline const ::TupleType* GetTupleType(DynamicArray<const ::Type*>&& types, bool lastArgIsVariadic = false)
	{
		return dynamic_cast<const TupleType *>(this->FindOrInsert(new TupleType(Move(types), lastArgIsVariadic)));
	}

private:
	//Members
	HashMap<String, UniquePointer<::Type>> types;
	DynamicArray<UniquePointer<GenericType>> genericTypes;

	//Inline
	inline const ::Type* FindOrInsert(UniquePointer<::Type>&& type)
	{
		String key = type->ToString();

		if(!this->types.Contains(key))
			this->types.Insert(key, Move(type));
		return this->types[key].operator->();
	}
};