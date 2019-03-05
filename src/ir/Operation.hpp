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
#include <Std++.hpp>
using namespace StdXX;
//Local
#include "Symbol.hpp"

namespace IR
{
	//Forward declarations
	class Block;
	class Compiler;
	class Program;

	class Operation
	{
	public:
		//Constructor
		inline Operation(const Block& block) : block(block)
		{
		}

		//Abstract
		virtual void Compile(Compiler& compiler) const = 0;
		virtual void ReplaceSymbol(const Symbol& from, const Symbol& to) = 0;
		virtual String ToString() const = 0;
		virtual void Verify() const = 0;

	protected:
		//Members
		const Block& block;

		//Methods
		const Program& GetProgram() const;
	};
}