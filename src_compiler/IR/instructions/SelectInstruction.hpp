/*
* Copyright (c) 2021 Amir Czwink (amir130@hotmail.de)
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
	class SelectInstruction : public Instruction
	{
	public:
		//Members
		Value* inner;
		Value* selector;

		//Constructor
		inline SelectInstruction(Value* inner, Value* selector) : inner(inner), selector(selector)
		{
		}

		//Methods
		void Visit(BasicBlockVisitor &visitor) override
		{
			visitor.OnVisitingSelectInstruction(*this);
		}

		String ToString() const override
		{
			return Symbol::ToString() + u8" <-- select " + this->inner->ToReferenceString() + u8", " + this->selector->ToReferenceString();
		}
	};
}