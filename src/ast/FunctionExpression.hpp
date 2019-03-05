/*
* Copyright (c) 2018 Amir Czwink (amir130@hotmail.de)
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
#include "Expression.hpp"

class FunctionExpression : public Expression
{
public:
	//Constructor
	inline FunctionExpression(DynamicArray<FuncMap>&& mappings) : mappings(Move(mappings))
	{
	}

	//Methods
	IR::Symbol Compile(IR::Block& block, const ExpressionCompileFlags& flags) const override;
	void CompileAsUnpack(Program & p, bool localAssign) const override;
	IR::Type GetType() const override;

private:
	//Members
	DynamicArray<FuncMap> mappings;
};