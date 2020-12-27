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
#include "../Instruction.hpp"

namespace IR
{
	class CreateNewObjectInstruction : public Instruction
	{
	public:
		//Constructor
		inline CreateNewObjectInstruction(Map<String, Value *>&& members) : members(Move(members))
		{
		}

		//Properties
		inline const Map<String, Value *>& Members() const
		{
			return this->members;
		}

		void Visit(BasicBlockVisitor &visitor) override
		{
			visitor.OnVisitingNewObjectInstruction(*this);
		}

		String ToString() const override
		{
			DynamicArray<String> strings;
			for(const auto& kv : this->members)
				strings.Push(kv.key + u8": " + kv.value->ToReferenceString());

			return Symbol::ToString() + u8" <-- new_object " + String::Join(strings, u8", ");
		}

	private:
		//Members
		Map<String, Value *> members;
	};
}