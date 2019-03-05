/*
* Copyright (c) 2018-2019 Amir Czwink (amir130@hotmail.de)
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
//Local
#include "AccessExpression.hpp"
#include "ArrayExpression.hpp"
#include "AssignmentStatement.hpp"
#include "BinaryExpression.hpp"
#include "CallExpression.hpp"
#include "DeclarationStatement.hpp"
#include "ExpressionStatement.hpp"
#include "FunctionExpression.hpp"
#include "IdentifierExpression.hpp"
#include "IntLiteralExpression.hpp"
#include "SetExpression.hpp"
#include "TupleExpression.hpp"
#include "WaitStatement.hpp"

//Public methods
UniquePointer<StatementBlock> Parser::Parse()
{
	UniquePointer<StatementBlock> m = new StatementBlock;
	
	while (true)
	{
		Statement* stmt = this->ParseStatement();
		if (!stmt)
			break;

		m->AddStatement(stmt);
	}

	return m;
}

//Private methods
Expression* Parser::ParseCallExpression(Expression* inner)
{
	CallExpression* callExpr = new CallExpression(inner);

	if (this->Accept(Token::RightParenthesis))
		return callExpr;

	while (true)
	{
		callExpr->SetArg(this->ParseExpression());
		
		if (this->Accept(Token::RightParenthesis))
			break;
		else
			this->Expect(Token::Comma);
	}

	return callExpr;
}

Expression* Parser::ParseExpression()
{
	Expression* expr = nullptr;

	if (!this->CheckLookahead())
		return nullptr;

	//try leafs first
	if (this->Accept(Token::Identifier))
	{
		String identifier = this->lexer.GetTokenValue();
		expr = new IdentifierExpression(identifier);
	}
	else if (this->Accept(Token::IntegerLiteral))
		expr = new IntLiteralExpression(this->lexer.GetTokenValue());
	else if (this->Accept(Token::Keyword_Func))
	{
		DynamicArray<FuncMap> mappings;
		this->ParseFunctionMappings(mappings);
		
		expr = new FunctionExpression(Move(mappings));
	}
	else if (this->Accept(Token::LeftSquaredBracket))
	{
		DynamicArray<UniquePointer<Expression>> exprs;
		if (!this->Accept(Token::RightSquaredBracket))
		{
			while (true)
			{
				exprs.Push(this->ParseExpression());
				if (this->Accept(Token::RightSquaredBracket))
					break;
				this->Expect(Token::Comma);
			}
		}
		expr = new ArrayExpression(Move(exprs));
	}
	else if (this->Accept(Token::LeftBrace))
	{
		DynamicArray<UniquePointer<Expression>> exprs;
		if (!this->Accept(Token::RightBrace))
		{
			while (true)
			{
				exprs.Push(this->ParseExpression());
				if (this->Accept(Token::RightBrace))
					break;
				this->Expect(Token::Comma);
			}
		}
		expr = new SetExpression(Move(exprs));
	}
	else if (this->Accept(Token::LeftParenthesis))
	{ //TODO: this can also be a bracketed expression
		DynamicArray<UniquePointer<Expression>> exprs;
		if (!this->Accept(Token::RightParenthesis))
		{
			while (true)
			{
				exprs.Push(this->ParseExpression());
				if (this->Accept(Token::RightParenthesis))
					break;
				this->Expect(Token::Comma);
			}
		}
		expr = new TupleExpression(Move(exprs));
	}

	//try all with left recursion

	//have leaf, try dot
	if (this->Accept(Token::Dot))
	{
		this->Expect(Token::Identifier);
		expr = new AccessExpression(expr, new IdentifierExpression(this->lexer.GetTokenValue()));
	}
	if(this->Accept(Token::LeftParenthesis))
		expr = this->ParseCallExpression(expr);

	//now try binary expressions
	this->RequireLookahead();
	switch (this->lookahead)
	{
	case Token::LessOrEqual:
	case Token::Minus:
	case Token::Multiply:
	case Token::Plus:
	{
		Token op = this->lookahead;
		this->haveLookahead = false;
		expr = new BinaryExpression(op, expr, this->ParseExpression());
	}
	break;
	}

	return expr;
}

void Parser::ParseFunctionMappings(DynamicArray<FuncMap>& funcMaps)
{
	while (true)
	{
		FuncMap m;

		if (this->Accept(Token::Map))
		{
			m.condition = nullptr;
			m.pattern = nullptr;
		}
		else
		{
			m.pattern = this->ParseExpression();

			if (this->Accept(Token::Pipe))
				m.condition = this->ParseExpression();
			else
				m.condition = nullptr;

			this->Expect(Token::Map);
		}

		this->RequireLookahead();
		if (this->Accept(Token::LeftBrace))
		{
			m.expr = nullptr;
			m.statements = this->Parse();
			this->Expect(Token::RightBrace);
		}
		else
			m.expr = this->ParseExpression();
		funcMaps.Push(Move(m));

		this->RequireLookahead();
		if (this->lookahead == Token::Semicolon)
			break;
		else
			this->Expect(Token::Comma);
	}
}

Statement* Parser::ParseStatement()
{
	if (!this->CheckLookahead())
		return nullptr;

	if (this->Accept(Token::Keyword_Wait))
	{
		UniquePointer<Expression> expr = this->ParseExpression();
		this->Expect(Token::Semicolon);
		return new WaitStatement(Move(expr));
	}
	else if (this->Accept(Token::Keyword_Let))
	{
		UniquePointer<Expression> lhs = this->ParseExpression();
		this->Expect(Token::Assign);
		UniquePointer<Expression> rhs = this->ParseExpression();
		this->Expect(Token::Semicolon);

		return new DeclarationStatement(false, Move(lhs), Move(rhs));
	}
	else if (this->Accept(Token::Keyword_Mut))
	{
		UniquePointer<Expression> lhs = this->ParseExpression();
		this->Expect(Token::Assign);
		UniquePointer<Expression> rhs = this->ParseExpression();
		this->Expect(Token::Semicolon);

		return new DeclarationStatement(true, Move(lhs), Move(rhs));
	}
	
	Expression* expression = this->ParseExpression();
	if ((expression == nullptr) && (!this->CheckLookahead()))
		return nullptr;

	if (this->Accept(Token::Assign))
	{
		Expression* rhs = this->ParseExpression();
		this->Expect(Token::Semicolon);

		return new AssignmentStatement(expression, rhs);
	}
	else if (this->Accept(Token::Semicolon))
		return new ExpressionStatement(expression);
	else if (this->lookahead == Token::RightBrace)
		return nullptr;
	
	
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}