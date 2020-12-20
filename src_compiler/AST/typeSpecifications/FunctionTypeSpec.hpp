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
#include "TypeSpec.hpp"

namespace AST
{
	class FunctionTypeSpec : public TypeSpec
	{
	public:
		//Constructor
		FunctionTypeSpec(UniquePointer<TypeSpec>&& argTypeSpec, UniquePointer<TypeSpec>&& resultTypeSpec)
			: argTypeSpec(Move(argTypeSpec)), resultTypeSpec(Move(resultTypeSpec))
		{
		}

		//Properties
		inline const TypeSpec& ArgTypeSpec() const
		{
			return *this->argTypeSpec;
		}

		inline const TypeSpec& ResultTypeSpec() const
		{
			return *this->resultTypeSpec;
		}

		//Methods
		void Visit(TypeSpecVisitor &visitor) const override
		{
			visitor.OnVisitedFunctionTypeSpec(*this);
		}

	private:
		//Members
		UniquePointer<TypeSpec> argTypeSpec;
		UniquePointer<TypeSpec> resultTypeSpec;
	};
}