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
//Class header
#include "Parser.hpp"

//Public methods
LinkedList<Statement*> Parser::Parse()
{
	LinkedList<Statement*> statements;
	
	while (true)
	{
		Statement* stmt = this->ParseStatement();
		if (!stmt)
			break;

		statements.InsertTail(stmt);
	}

	return statements;
}

//Private methods
Expression* Parser::ParseExpression()
{
	Expression* expr = nullptr;

	//try leafs first
	if(this->Accept(Token::Identifier))
		expr = new Expression(this->lexer.GetTokenValue());
	else if (this->Accept(Token::LeftParenthesis))
	{
		//(expr) or function definition
		this->RequireLookahead();
		if (this->Accept(Token::Identifier))
		{
			//still no idea
			if (this->Accept(Token::RightParenthesis))
			{
				//still no idea
				if (this->Accept(Token::Colon))
				{
					//function definition
					DynamicArray<FuncMap> funcMaps;

					this->ParseFunctionMappings(funcMaps);
					NOT_IMPLEMENTED_ERROR; //TODO: implement me
				}
				else
				{
					NOT_IMPLEMENTED_ERROR; //TODO: implement me
				}
			}
			else
			{
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
			}
		}
		else
		{
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}

	//have leaf, now try binary expressions
	this->RequireLookahead();
	switch (this->lookahead)
	{
	case Token::LessOrEqual:
		this->haveLookahead = false;
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
		//return new Expression(this->lookahead, expr, this->ParseExpression());
	default:
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
	
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return nullptr;
}

void Parser::ParseFunctionMappings(DynamicArray<FuncMap>& funcMaps)
{
	while (true)
	{
		FuncMap m;
		if (this->Accept(Token::Map))
		{
			m.condition = nullptr;
		}
		else
		{
			this->RequireLookahead();
			if (this->lookahead == Token::Semicolon)
				break;

			m.condition = this->ParseExpression();
			this->Expect(Token::Map);
		}

		m.expr = this->ParseExpression();
		funcMaps.Push(m);
	}
}

Statement* Parser::ParseStatement()
{
	Expression* expression = this->ParseExpression();
	if (this->Accept(Token::Assign))
	{
		Expression* rhs = this->ParseExpression();
		this->Expect(Token::Semicolon);
	}
	else
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
	
	return nullptr;
}