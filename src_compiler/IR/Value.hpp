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
#include <StdXX.hpp>
using namespace StdXX;
#include "../types/Type.hpp"
#include "visitors/ValueVisitor.hpp"

namespace IR
{
	class Value
	{
	public:
		//Members
		const ::Type* type;

		//Constructor
		inline Value() : type(nullptr)
		{
		}

		//Destructor
		virtual ~Value(){}

		//Abstract
		virtual String ToString() const = 0;
		virtual void Visit(ValueVisitor& visitor) const = 0;

		//Overrideable
		virtual String ToReferenceString() const
		{
			return this->ToString();
		}
	};
}