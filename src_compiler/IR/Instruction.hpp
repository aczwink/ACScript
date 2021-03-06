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
#pragma once
//Local
#include "Symbol.hpp"
#include "visitors/BasicBlockVisitor.hpp"

namespace IR
{
	class Instruction : public virtual Symbol
	{
	public:
		//Abstract
		virtual void Visit(BasicBlockVisitor& visitor) = 0;

		void Visit(ValueVisitor &visitor) override
		{
			visitor.OnVisitingInstructionResultValue(*this);
		}
	};
}