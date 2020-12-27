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
#pragma once
#include <StdXX.hpp>
using namespace StdXX;

enum class RuntimeValueType
{
	Bool,
	Dictionary,
	Float64,
	Null,
	String,
	Tuple
};

class RuntimeValue
{
public:
	//Constructors
	inline RuntimeValue() : type(RuntimeValueType::Null)
	{
	}

	inline RuntimeValue(bool b) : type(RuntimeValueType::Bool), b(b)
	{
	}

	inline RuntimeValue(float64 f64) : type(RuntimeValueType::Float64), f64(f64)
	{
	}

	inline RuntimeValue(String* string) : type(RuntimeValueType::String), string(string)
	{
	}

	//Properties
	inline RuntimeValueType Type() const
	{
		return this->type;
	}

	inline bool ValueBool() const
	{
		return this->b;
	}

	inline float64 ValueF64() const
	{
		return this->f64;
	}

	inline DynamicArray<RuntimeValue>& ValuesArray()
	{
		return *this->array;
	}

	inline const DynamicArray<RuntimeValue>& ValuesArray() const
	{
		return *this->array;
	}

	inline Map<String, RuntimeValue>& ValuesDictionary()
	{
		return *this->dictionary;
	}

	inline const Map<String, RuntimeValue>& ValuesDictionary() const
	{
		return *this->dictionary;
	}

	inline const String& ValueString() const
	{
		return *this->string;
	}

	//Functions
	inline static RuntimeValue CreateDictionary(Map<String, RuntimeValue>* values)
	{
		RuntimeValue v;
		v.type = RuntimeValueType::Dictionary;
		v.dictionary = values;
		return v;
	}

	inline static RuntimeValue CreateTuple(DynamicArray<RuntimeValue>* values)
	{
		RuntimeValue v;
		v.type = RuntimeValueType::Tuple;
		v.array = values;
		return v;
	}

private:
	//Members
	RuntimeValueType type;
	union
	{
		bool b;
		float64 f64;
		DynamicArray<RuntimeValue>* array;
		Map<String, RuntimeValue>* dictionary;
		String* string;
	};
};