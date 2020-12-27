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
#include "Value.hpp"

namespace IR
{
	class ConstantString : public Value
	{
	public:
		//Constructor
		inline ConstantString(const String& value) : value(value)
		{
		}

		//Properties
		inline const String& Value() const
		{
			return this->value;
		}

		//Methods
		String ToString() const override
		{
			return this->value;
		}

		void Visit(ValueVisitor &visitor) const override
		{
			visitor.OnVisitingConstantString(*this);
		}

	private:
		//Members
		String value;
	};
}