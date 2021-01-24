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
//Local
#include "Statement.hpp"

namespace AST
{
	class TypeDeclarationStatement : public Statement
	{
	public:
		//Constructor
		inline TypeDeclarationStatement(UniquePointer<StatementBlock>&& declarations) : declarations(Move(declarations))
		{
		}

		void Visit(StatementVisitor &visitor) const override
		{

		}

	private:
		//Members
		UniquePointer<StatementBlock> declarations;
	};
}