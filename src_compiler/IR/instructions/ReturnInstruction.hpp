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
//Local
#include "../Instruction.hpp"

namespace IR
{
	class ReturnInstruction : public Instruction
	{
	public:
		//Members
		const Value* returnValue;

		//Constructor
		inline ReturnInstruction(const Value* returnValue) : returnValue(returnValue)
		{
			this->type = returnValue->type;
		}

		//Methods
		void Visit(Visitor &visitor) const override
		{
			visitor.OnVisitingReturnInstruction(*this);
		}

		String ToString() const override
		{
			return u8"ret " + this->returnValue->ToReferenceString();
		}
	};
}