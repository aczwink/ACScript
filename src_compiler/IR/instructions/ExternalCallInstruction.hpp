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
#include "../External.hpp"
#include "../Instruction.hpp"

namespace IR
{
	class ExternalCallInstruction : public Instruction
	{
	public:
		//Members
		const External* external;
		const Value* argument;

		//Constructor
		inline ExternalCallInstruction(const External* external, const Value* argument) : external(external), argument(argument)
		{
			this->type = external->returnType;
		}

		//Methods
		void Visit(BasicBlockVisitor &visitor) override
		{
			visitor.OnVisitingExternalCallInstruction(*this);
		}

		String ToString() const override
		{
			return Symbol::ToString() + u8" <-- callext " + this->external->ToReferenceString() + u8", " + this->argument->ToReferenceString();
		}
	};
}