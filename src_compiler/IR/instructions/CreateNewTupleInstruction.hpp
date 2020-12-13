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
//Local
#include "../Instruction.hpp"

namespace IR
{
	class CreateNewTupleInstruction : public Instruction
	{
	public:
		//Constructor
		inline CreateNewTupleInstruction(DynamicArray<Value *>&& values) : values(Move(values))
		{
		}

		//Properties
		inline const DynamicArray<Value *>& Values() const
		{
			return this->values;
		}

		//Methods
		String ToString() const override
		{
			DynamicArray<String> strings;
			for(const Value*const& value : this->values)
				strings.Push(value->ToString());

			return Symbol::ToString() + u8" <-- new_tuple " + String::Join(strings, u8", ");
		}

		void Visit(Visitor &visitor) const override
		{
			visitor.OnVisitingNewTupleInstruction(*this);
			for(const Value*const& value : this->values)
				value->Visit(visitor);
			visitor.OnVisitedNewTupleInstruction(*this);
		}

	private:
		//Members
		DynamicArray<Value *> values;
	};
}