/*
* Copyright (c) 2018-2020 Amir Czwink (amir130@hotmail.de)
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
//Local
#include "../ir/Block.hpp"
#include "../ir/Symbol.hpp"
#include "../ir/Type.hpp"
#include "Token.hpp"

//Forward declarations
class Expression;
class Program;
class StatementBlock;

struct FuncMap
{
	Expression* pattern;
	Expression* condition;
	Expression* expr;
	UniquePointer<StatementBlock> statements;

	FuncMap& operator=(FuncMap&&) = default;
};

struct FuncParamDef
{
	String name;
};

struct ExpressionCompileFlags
{
	/**
	 * Set if expression is on left side of assignment statement.
	 */
	bool isLeftValue;
	/**
	 * Set if expression is behind a let/mut and may declare locals.
	 */
	bool declare;
	/**
	 * Set if expression is behind a mut and thus declare mutable variables.
	 */
	bool mutableFlag;
	/**
	 * Set if expression is a member access (i.e. can not access a global/local/special variable).
	 */
	bool isMember;
};

class Expression
{
public:
	//Destructor
	virtual ~Expression()
	{
	}

	//Abstract
	virtual IR::Symbol Compile(IR::Block& block, const ExpressionCompileFlags& flags = {}) const = 0;
	virtual void CompileAsUnpack(Program& p, bool localAssign = false) const = 0;
	virtual IR::Type GetType() const = 0;
};