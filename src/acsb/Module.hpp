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
#pragma once
//Local
#include "../ir/Program.hpp"
#include "Procedure.hpp"
#include "RuntimeObject.hpp"

namespace ACSB
{
	class Module
	{
	public:
		//Constructors
		Module(const uint8* code, uint32 codeSize, DynamicArray<Procedure>&& procedures, const IR::Program& program);

		inline Module(Module&& m) //Move ctor
		{
			this->code = m.code;
			m.code = nullptr;
		}

		//Destructor
		~Module();

		//Inline
		inline const uint8* GetCode() const
		{
			return this->code;
		}

		inline const RuntimeObject& GetConstant(uint32 constantIndex) const
		{
			return this->constants[constantIndex];
		}

		inline uint16 GetNumberOfGlobals() const
		{
			return this->nGlobals;
		}

		inline const Procedure& GetProcedure(uint32 procIndex) const
		{
			return this->procedures[procIndex];
		}

	private:
		//Members
		uint8* code;
		uint16 nGlobals;
		DynamicArray<RuntimeObject> constants;
		DynamicArray<Procedure> procedures;
	};
}