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
#include "Block.hpp"
#include "Type.hpp"

namespace IR
{
	//Forward declarations
	class Program;

	class Procedure
	{
	public:
		//Constructors
		inline Procedure(const String& name, const Type& resultType, Program& program) : name(name), resultType(resultType), program(program), nextValueNumber(0), block(*this)
		{
		}

		inline Procedure(Procedure&& proc) : name(Move(proc.name)), resultType(Move(proc.resultType)), program(proc.program), nextValueNumber(proc.nextValueNumber), block(Move(proc.block), *this)
		{
		}

		//Operators
		Procedure& operator=(Procedure&&) = default;

		//Methods
		void Compile(Compiler& compiler);
		String ToString() const;

		//Inline
		inline Block& GetBlock()
		{
			return this->block;
		}

		inline const String& GetName() const
		{
			return this->name;
		}

		inline uint32 GetNextValueNumber()
		{
			return this->nextValueNumber++;
		}

		inline Program& GetProgram()
		{
			return this->program;
		}

		inline void Verify() const
		{
			this->block.Verify();
		}

	private:
		//Members
		String name;
		Type resultType;
		Program& program;
		Block block;
		uint32 nextValueNumber;
	};
}