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
#include "Lexer.hpp"

//Public methods
bool Lexer::GetNextToken(Token& t)
{
	while (true)
	{
		if (this->AtEnd())
			return false;

		uint32 c = this->ReadNextChar();

		if (c == u8'\n')
		{
			this->lineCounter++;
			continue; //ignore
		}
		if (IsWhiteSpaceChar(c))
			continue; //ignore
		if (this->IsDigit(c))
		{
			this->ReadNumber(c);
			t = Token::Number;
			return true;
		}
		switch (c)
		{
		case u8'(':
			t = Token::LeftParenthesis;
			return true;
		case u8')':
			t = Token::RightParenthesis;
			return true;
		case u8'<':
			if (this->PeekNextChar() == u8'=')
			{
				this->ReadNextChar();
				t = Token::LessOrEqual;
				return true;
			}
			t = Token::Less;
			return true;
		case u8':':
			if (this->PeekNextChar() == u8'=')
			{
				this->ReadNextChar();
				t = Token::Assign;
				return true;
			}
			t = Token::Colon;
			return true;
			break;
		default:
			//its an id or keyword
			this->ReadIdentifierOrKeyword(c);
			t = Token::Identifier;
			return true;
		}
	}
}

//Private methods
void Lexer::ReadIdentifierOrKeyword(uint32 firstChar)
{
	String s;

	s += firstChar;
	while (true)
	{
		uint32 c = this->PeekNextChar();

		if (IsWhiteSpaceChar(c))
			break;
		bool skip = false;
		switch (c)
		{
		case u8')':
			skip = true;
		}
		if (skip)
			break;

		s += c;
		this->ReadNextChar(); //consume it
	}

	this->tokenValue = s;
}

void Lexer::ReadNumber(uint32 firstChar)
{
	String s;

	s += firstChar;
	while (true)
	{
		uint32 c = this->PeekNextChar();
		if (this->IsDigit(c))
		{
			s += c;
			this->ReadNextChar();
		}
		else
			break;
	}

	this->tokenValue = s;
}