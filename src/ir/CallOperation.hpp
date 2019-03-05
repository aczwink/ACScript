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
#include "ValueOperation.hpp"

namespace IR
{
	class CallOperation : public ValueOperation
	{
	public:
		//Constructor
		inline CallOperation(Symbol&& context, Symbol&& function, Symbol&& arg, Block& block) : ValueOperation(block),
			context(Move(context)), function(Move(function)), arg(Move(arg))
		{
		}

		//Methods
		void Compile(Compiler & compiler) const override;
		void ReplaceSymbol(const Symbol & from, const Symbol & to) override;
		String ToString() const override;
		void Verify() const override;

		//Inline
		inline const Symbol& GetArg() const
		{
			return this->arg;
		}

		inline const Symbol& GetContext() const
		{
			return this->context;
		}

		inline const Symbol& GetFunction() const
		{
			return this->function;
		}

	private:
		//Members
		Symbol context;
		Symbol function;
		Symbol arg;
	};
}