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

#include "Value.hpp"
#include "TupleValue.hpp"

namespace IR
{
	//Forward declarations
	class Builder;
	class Procedure;

	class Environment : public TupleValue
	{
	public:
		//Constructor
		inline Environment(Procedure& procedure) : procedure(procedure)
		{
		}

		//Methods
		String ToReferenceString() const override
		{
			return u8"__env";
		}

		String ToString() const override
		{
			return u8"(" + this->ValuesToString() + u8")";
		}

		//Inline
		inline IR::Value* CaptureByReference(IR::Value* value, IR::Builder& builder)
		{
			this->Add(value);
			return value;
		}

		void Visit(ValueVisitor &visitor) override
		{

		}

	private:
		//Members
		Procedure& procedure;
	};
}