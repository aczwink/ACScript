/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "Operation.hpp"

namespace IR
{
	class StoreOperation : public Operation
	{
	public:
		//Constructor
		inline StoreOperation(Symbol&& lhs, Symbol&& rhs, const Block& block) : Operation(block),
			lhs(Move(lhs)), rhs(Move(rhs))
		{
		}

		//Methods
		void Compile(Compiler & compiler) const override;
		void ReplaceSymbol(const Symbol & from, const Symbol & to) override;
		String ToString() const override;
		void Verify() const override;

		//Inline
		inline const Symbol& GetLHS() const
		{
			return this->lhs;
		}

		inline const Symbol& GetRHS() const
		{
			return this->rhs;
		}

	private:
		//Members
		Symbol lhs;
		Symbol rhs;
	};
}