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
#include "Module.hpp"
#include "RuntimeValue.hpp"
#include "GarbageCollector.hpp"

class VM
{
public:
	//Constructor
	inline VM(const Module& module) : module(module)
	{
	}

	//Methods
	void Run();

private:
	//Members
	const Module& module;
	GarbageCollector garbageCollector;

	//Inline
	inline uint16 ExtractUInt16FromProgramCounter(const uint8*& pc)
	{
		uint16 v = *pc++ << 8_u16;
		v |= *pc++;
		return v;
	}
};