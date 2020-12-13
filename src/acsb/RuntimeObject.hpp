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

//Forward declarations
namespace ACSB
{
	class Procedure;
	struct RuntimeCall;
}

//Forward declarations
class Program;
class RuntimeObject;
typedef RuntimeObject(*NativeFunction)(const RuntimeObject&);

enum class RuntimeObjectType
{
	Array,
	Bool,
	Function,
	Null,
	MachineInt,
	MachineUnsignedInt,
	NativeFunction,
	Object,
	String,
	TaskReference,
};

struct RuntimeObjectResource
{
	uint32 nReferences;
	union
	{
		DynamicArray<RuntimeObject>* array;
		Map<RuntimeObject, RuntimeObject>* object;
		struct
		{
			bool finished;
			ACSB::RuntimeCall* caller;
			RuntimeObject* returnValue;
		} taskRef;
	};
};

struct RuntimeObjectValue
{
	union
	{
		bool b;
		int64 i64;
		uint64 u64;
		NativeFunction func;
		const ACSB::Procedure* proc;
		RuntimeObjectResource* resource;
	};

	String stringValue;
};

class RuntimeObject
{
public:
	//Constructors
	inline RuntimeObject() : type(RuntimeObjectType::Null)
	{
	}

	inline RuntimeObject(bool value) : type(RuntimeObjectType::Bool)
	{
		this->value.b = value;
	}

	inline RuntimeObject(int64 value) : type(RuntimeObjectType::MachineInt)
	{
		this->value.i64 = value;
	}

	inline RuntimeObject(uint64 value) : type(RuntimeObjectType::MachineUnsignedInt)
	{
		this->value.u64 = value;
	}

	inline RuntimeObject(const char* string) : type(RuntimeObjectType::String)
	{
		this->value.stringValue = string;
	}

	inline RuntimeObject(const String& string) : type(RuntimeObjectType::String)
	{
		this->value.stringValue = string;
	}

	inline RuntimeObject(const ACSB::Procedure* proc) : type(RuntimeObjectType::Function)
	{
		this->value.proc = proc;
	}

	inline RuntimeObject(NativeFunction func) : type(RuntimeObjectType::NativeFunction)
	{
		this->value.func = func;
	}

	inline RuntimeObject(const RuntimeObject& other) //copy ctor
	{
		this->type = RuntimeObjectType::Null;
		*this = other;
	}

	inline RuntimeObject(RuntimeObject&& other) //move ctor
	{
		this->type = other.type;
		this->value = Move(other.value);
		other.type = RuntimeObjectType::Null;
	}

	//Destructor
	~RuntimeObject();

	//Operators
	RuntimeObject& operator=(const RuntimeObject& other);

	inline RuntimeObject& operator=(RuntimeObject&& other)
	{
		this->type = other.type;
		this->value = other.value;
		other.type = RuntimeObjectType::Null;

		return *this;
	}

	inline RuntimeObject& operator[](const RuntimeObject& key)
	{
		return this->value.resource->object->operator[](key);
	}

	bool operator<(const RuntimeObject& other) const;
	bool operator==(const RuntimeObject& other) const;

	inline bool operator!=(const RuntimeObject& other) const
	{
		return !(*this == other);
	}

	inline bool operator<=(const RuntimeObject& other) const
	{
		return (*this < other) || (*this == other);
	}

	inline bool operator>(const RuntimeObject& other) const
	{
		return !(*this <= other);
	}

	inline auto& GetTaskRefData()
	{
		ASSERT(this->type == RuntimeObjectType::TaskReference, u8"The object is not a task reference!");
		return this->value.resource->taskRef;
	}

	//Inline
	inline const RuntimeObjectType GetType() const
	{
		return this->type;
	}

	inline const RuntimeObjectValue& GetValue() const
	{
		return this->value;
	}

	//Operators
	bool operator==(const RuntimeObject& other);

	//Methods
	bool IsTaskReady();
	String ToString() const;

	//Functions
	static inline RuntimeObject Array()
	{
		RuntimeObject obj;
		obj.type = RuntimeObjectType::Array;
		obj.value.resource = new RuntimeObjectResource;
		obj.value.resource->nReferences = 1;
		obj.value.resource->array = new DynamicArray<RuntimeObject>();
		return obj;
	}

	static inline RuntimeObject Object()
	{
		RuntimeObject obj;
		obj.type = RuntimeObjectType::Object;
		obj.value.resource = new RuntimeObjectResource;
		obj.value.resource->nReferences = 1;
		obj.value.resource->object = new Map<RuntimeObject, RuntimeObject>();
		return obj;
	}

	static inline RuntimeObject TaskRef(ACSB::RuntimeCall* caller)
	{
		RuntimeObject obj;
		obj.type = RuntimeObjectType::TaskReference;
		obj.value.resource = new RuntimeObjectResource;
		obj.value.resource->nReferences = 1;
		obj.value.resource->taskRef.finished = false;
		obj.value.resource->taskRef.caller = caller;
		obj.value.resource->taskRef.returnValue = new RuntimeObject();
		return obj;
	}

private:
	//Members
	RuntimeObjectType type;
	RuntimeObjectValue value;

	//Methods
	void Release();
};