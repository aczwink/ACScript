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
#include <Std++.hpp>
using namespace StdXX;
//Local
#include "Token.hpp"

class Lexer
{
public:
	//Constructor
	inline Lexer(InputStream& input) : reader(input, TextCodecType::UTF8), haveLookahead(false)
	{
	}

	//Methods
	bool GetNextToken(Token& t);

	//Inline
	inline const String& GetTokenValue() const
	{
		return this->tokenValue;
	}

private:
	//Members
	TextReader reader;
	uint32 lineCounter;
	bool haveLookahead;
	uint32 lookahead;
	String tokenValue;

	//Methods
	void ReadIdentifierOrKeyword(uint32 firstChar, Token& t);
	void ReadNumber(uint32 firstChar);

	//Inline
	inline bool IsDigit(uint32 c) const
	{
		return Math::IsValueInInterval(c, (uint32)u8'0', (uint32)u8'9');
	}

	inline uint32 PeekNextChar()
	{
		if (!this->haveLookahead)
		{
			this->lookahead = this->reader.ReadCodePoint();
			this->haveLookahead = true;
		}
		return this->lookahead;
	}

	inline uint32 ReadNextChar()
	{
		if (this->haveLookahead)
		{
			uint32 c = this->lookahead;
			this->haveLookahead = false;
			return this->lookahead;
		}
		return this->reader.ReadCodePoint();
	}

	inline bool AtEnd() const
	{
		return !this->haveLookahead && this->reader.IsAtEnd();
	}
};