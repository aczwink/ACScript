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
#include "ConstantString.hpp"

namespace IR
{
	class TupleValue : public virtual Value
	{
	public:
		//Constructors
		inline TupleValue()
		{
		}

		inline TupleValue(DynamicArray<Value *>&& values) : values(Move(values))
		{
		}

		//Properties
		inline const DynamicArray<Value *>& Values() const
		{
			return this->values;
		}

	protected:
		//Inline
		inline void Add(IR::Value* value)
		{
			this->values.Push(value);
		}

		inline String ValuesToString() const
		{
			DynamicArray<String> strings;
			for(const Value*const& value : this->values)
				strings.Push(value->ToReferenceString());

			return String::Join(strings, u8", ");
		}

	private:
		//Members
		DynamicArray<Value *> values;
	};
}