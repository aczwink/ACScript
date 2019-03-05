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
#include "StatementBlock.hpp"
#include "Statement.hpp"
#include "Lexer.hpp"

class Parser
{
public:
	//Constructor
	inline Parser(InputStream& inputStream) : lexer(inputStream), haveLookahead(false)
	{
	}

	//Methods
	UniquePointer<StatementBlock> Parse();

private:
	//Members
	bool haveLookahead;
	Token lookahead;
	Lexer lexer;

	//Methods
	Expression* ParseCallExpression(Expression* inner);
	Expression* ParseExpression();
	void ParseFunctionMappings(DynamicArray<FuncMap>& funcMaps);
	Statement* ParseStatement();

	//Inline
	inline bool Accept(Token t)
	{
		this->RequireLookahead();
		if (this->lookahead == t)
		{
			this->haveLookahead = false;
			return true;
		}
		return false;
	}

	inline bool CheckLookahead()
	{
		if (!this->haveLookahead)
			this->haveLookahead = this->lexer.GetNextToken(this->lookahead);
		return this->haveLookahead;
	}

	inline void Expect(Token t)
	{
		this->RequireLookahead();
		ASSERT(this->lookahead == t, u8"TODO: implement this correctly");
		this->haveLookahead = false;
	}

	inline void RequireLookahead()
	{
		this->CheckLookahead();
		ASSERT(this->haveLookahead, u8"TODO: implement this correctly.");
	}
};