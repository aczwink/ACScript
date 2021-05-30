/*
* Copyright (c) 2019-2020 Amir Czwink (amir130@hotmail.de)
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
#include "ExternalsManager.hpp"

class Module
{
public:
	//Constructor
	Module(SeekableInputStream& inputStream, ExternalsManager& externalsManager);

	//Destructor
	~Module();

	//Properties
	inline const void* EntryPoint() const
	{
		return this->GetCodeAtOffset(this->entryPoint);
	}

	//Inline
	inline const void* GetCodeAtOffset(uint16 offset) const
	{
		return reinterpret_cast<const void *>( &((const uint8 *) this->code)[offset] );
	}

	inline const RuntimeValue& GetConstant(uint16 constantIndex) const
	{
		return this->constants[constantIndex];
	}

	inline External GetExternal(uint16 externalIndex) const
	{
		return this->moduleExternals[externalIndex];
	}

private:
	//Members
	uint16 entryPoint;
	void* code;
	DynamicArray<RuntimeValue> constants;
	DynamicArray<Math::Natural> constantNaturals;
	DynamicArray<String> constantStrings;
	DynamicArray<External> moduleExternals;
};