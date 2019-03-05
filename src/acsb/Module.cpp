/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "Module.hpp"
//Namespaces
using namespace ACSB;

//Constructor
Module::Module(const uint8* code, uint32 codeSize, DynamicArray<Procedure>&& procedures, const IR::Program& program) : procedures(Move(procedures))
{
	this->code = (uint8*)MemAlloc(codeSize);
	MemCopy(this->code, code, codeSize);

	this->nGlobals = program.GetNumberOfGlobals();

	this->constants.Resize(program.GetNumberOfConstants());
	for (const auto& kv : program.GetI64Constants())
		this->constants[kv.value] = RuntimeObject(kv.key);
	for (const auto& kv : program.GetUI64Constants())
		this->constants[kv.value] = RuntimeObject(kv.key);
	for (const auto& kv : program.GetStringConstants())
		this->constants[kv.value] = RuntimeObject(kv.key);
}

//Destructor
Module::~Module()
{
	MemFree(this->code);
}