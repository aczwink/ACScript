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
			t = Token::IntegerLiteral;
			return true;
		}

		switch (c)
		{
		case u8'(': //0x28
			t = Token::LeftParenthesis;
			return true;
		case u8')': //0x29
			t = Token::RightParenthesis;
			return true;
		case u8'*': //0x2a
			t = Token::Multiply;
			return true;
		case u8'+': //0x2b
			t = Token::Plus;
			return true;
		case u8',': //0x2c
			t = Token::Comma;
			return true;
		case u8'-': //0x2d
			t = Token::Minus;
			return true;
		case u8':': //0x3a
			if (this->PeekNextChar() == u8'=')
			{
				this->ReadNextChar();
				t = Token::Assign;
				return true;
			}
			t = Token::Colon;
			return true;
		case u8';': //0x3b
			t = Token::Semicolon;
			return true;
		case u8'<': //0x3c
			if (this->PeekNextChar() == u8'=')
			{
				this->ReadNextChar();
				t = Token::LessOrEqual;
				return true;
			}
			t = Token::Less;
			return true;
		case u8'=': //0x3d
			if (this->PeekNextChar() == u8'>')
			{
				this->ReadNextChar();
				t = Token::Map;
				return true;
			}
			t = Token::Equals;
			return true;
		case u8'.': //0x46
			t = Token::Dot;
			return true;
		case u8'/': //0x47
			if (this->PeekNextChar() == u8'*')
			{
				//multi line comment
				this->ReadNextChar();
				uint32 lastChar = u8'*';
				while (true)
				{
					uint32 c = this->ReadNextChar();
					if ((lastChar == u8'*') && (c == u8'/'))
						break;
				}
				continue; //skip comment
			}
			NOT_IMPLEMENTED_ERROR;
		case u8'[': //0x5b
			t = Token::LeftSquaredBracket;
			return true;
		case u8']': //0x5d
			t = Token::RightSquaredBracket;
			return true;
		case u8'{': //0x7b
			t = Token::LeftBrace;
			return true;
		case u8'|': //0x7c
			t = Token::Pipe;
			return true;
		case u8'}': //0x7d
			t = Token::RightBrace;
			return true;
		default:
			//its an id or keyword
			this->ReadIdentifierOrKeyword(c, t);
			return true;
		}
	}
}

//Private methods
void Lexer::ReadIdentifierOrKeyword(uint32 firstChar, Token& t)
{
	String s;

	t = Token::Identifier;

	s += firstChar;
	while (true)
	{
		uint32 c = this->PeekNextChar();

		if (IsWhiteSpaceChar(c))
			break;
		bool skip = false;
		switch (c)
		{
		case u8'(':
		case u8')':
		case u8',':
		case u8'-':
		case u8'.':
		case u8':':
			skip = true;
		}
		if (skip)
			break;

		s += c;
		this->ReadNextChar(); //consume it
	}

	if (s == u8"func")
		t = Token::Keyword_Func;
	else if (s == u8"let")
		t = Token::Keyword_Let;
	else if (s == u8"mut")
		t = Token::Keyword_Mut;
	else if (s == u8"wait")
		t = Token::Keyword_Wait;

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