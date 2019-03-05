/*
* Copyright (c) 2018-2019 Amir Czwink (amir130@hotmail.de)
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
//Class header
#include "RuntimeObject.hpp"

//Destructor
RuntimeObject::~RuntimeObject()
{
	this->Release();
}

//Operators
RuntimeObject& RuntimeObject::operator=(const RuntimeObject& other)
{
	this->Release();

	switch (other.type)
	{
	case RuntimeObjectType::Object:
	case RuntimeObjectType::TaskReference:
	{
		this->type = other.type;
		this->value = other.value;
		this->value.resource->nReferences++;
	}
	break;
	default:
		this->type = other.type;
		this->value = other.value;
	}

	return *this;
}

bool RuntimeObject::operator<(const RuntimeObject& other) const
{
	switch (this->type)
	{
	case RuntimeObjectType::Bool:
	{
		switch (other.type)
		{
		case RuntimeObjectType::Bool:
			return this->value.b < other.value.b;
		default:
			NOT_IMPLEMENTED_ERROR;
		}
	}
	break;
	case RuntimeObjectType::String:
	{
		switch (other.type)
		{
		case RuntimeObjectType::String:
			return this->value.stringValue < other.value.stringValue;
		default:
			NOT_IMPLEMENTED_ERROR;
		}
	}
	break;
	default:
		NOT_IMPLEMENTED_ERROR;
	}
	return false;
}

bool RuntimeObject::operator==(const RuntimeObject& other) const
{
	switch (this->type)
	{
	case RuntimeObjectType::Null:
		return other.type == RuntimeObjectType::Null;
	case RuntimeObjectType::String:
	{
		switch (other.type)
		{
		case RuntimeObjectType::String:
			return this->value.stringValue == other.value.stringValue;
		default:
			NOT_IMPLEMENTED_ERROR;
		}
	}
	break;
	case RuntimeObjectType::TaskReference:
	{
		switch (other.type)
		{
		case RuntimeObjectType::TaskReference:
			NOT_IMPLEMENTED_ERROR;
		default:
			return false;
		}
	}
	break;
	}

	NOT_IMPLEMENTED_ERROR;
	return false;
}

//Public methods
bool RuntimeObject::IsTaskReady()
{
	if (this->type == RuntimeObjectType::TaskReference)
	{
		return this->value.resource->taskRef.finished;
	}
	NOT_IMPLEMENTED_ERROR;
}

String RuntimeObject::ToString() const
{
	switch (this->type)
	{
	case RuntimeObjectType::Null:
		return u8"null";
	case RuntimeObjectType::MachineInt:
		return String::Number(this->value.i64);
	case RuntimeObjectType::NativeFunction:
		return u8"native func";
	case RuntimeObjectType::MachineUnsignedInt:
		return String::Number(this->value.u64);
	}

	return this->value.stringValue;
}

/*
bool RuntimeObject::operator==(const RuntimeObject & other)
{
	if(this->type != other.type)
		return false;

	switch (this->type)
	{
	case RuntimeObjectType::MachineUnsignedInt:
		return this->value.u64 == other.value.u64;
	default:
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
	
	return false;
}*/

//Private methods
void RuntimeObject::Release()
{
	switch (this->type)
	{
	case RuntimeObjectType::Object:
		if ((--this->value.resource->nReferences) == 0)
		{
			delete this->value.resource->object;
			delete this->value.resource;
		}
		break;
	case RuntimeObjectType::TaskReference:
		if ((--this->value.resource->nReferences) == 0)
		{
			delete this->value.resource->taskRef.returnValue;
			delete this->value.resource;
		}
		break;
	}
	this->type = RuntimeObjectType::Null;
}