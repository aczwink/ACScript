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
#include "Instruction.hpp"
#include "visitors/BasicBlockVisitor.hpp"

namespace IR
{
	class BasicBlock
	{
	public:
		//Constructor
		inline BasicBlock(const String& name) : name(name)
		{
		}

		//Members
		Map<String, IR::Value*> namedValues;

		//Properties
		inline const DynamicArray<Instruction*>& Instructions() const
		{
			return this->instructions;
		}

		inline const String& Name() const
		{
			return this->name;
		}

		//Methods
		void Visit(BasicBlockVisitor& visitor)
		{
			for(Instruction*const& instruction : this->instructions)
				instruction->Visit(visitor);
		}

		//Inline
		inline void Add(Instruction* instruction)
		{
			this->instructions.Push(instruction);
		}

	private:
		//Members
		String name;
		DynamicArray<Instruction*> instructions;
	};
}