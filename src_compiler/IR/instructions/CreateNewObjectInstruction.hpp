/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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

//Forward declarations
class TypeCatalog;

namespace IR
{
	class CreateNewObjectInstruction : public Instruction
	{
	public:
		//Constructors
		inline CreateNewObjectInstruction()
		{
		}

		//Properties
		inline const Map<String, Value *>& Members() const
		{
			return this->members;
		}

		//Methods
		String ToString() const override
		{
			DynamicArray<String> strings;
			for(const auto& kv : this->members)
				strings.Push(kv.key + u8": " + kv.value->ToReferenceString());

			return Symbol::ToString() + u8" <-- new_object " + String::Join(strings, u8", ");
		}

		void UpdateType(TypeCatalog& typeCatalog);

		void Visit(BasicBlockVisitor &visitor) override
		{
			visitor.OnVisitingNewObjectInstruction(*this);
		}

	private:
		//Members
		Map<String, Value *> members;
	};
}