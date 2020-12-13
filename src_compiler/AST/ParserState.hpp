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
//Local
#include "statements/StatementBlock.hpp"
#include "External.hpp"

namespace AST
{
	class ParserState
	{
	public:
		//Properties
		inline StatementBlock &ModuleStatements()
		{
			return this->moduleStatements;
		}

		//Inline
		inline void AddExternal(External* external)
		{
			this->externals.Push(external);
		}

		inline String *CreateString(const char *string)
		{
			this->strings.Push(new String(String::CopyRawString(string)));
			return this->strings.Last().operator->();
		}

	private:
		//Members
		DynamicArray<UniquePointer<External>> externals;
		StatementBlock moduleStatements;
		DynamicArray<UniquePointer<String>> strings;
	};
}