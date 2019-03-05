/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "Symbol.hpp"
//Namespaces
using namespace IR;

//Operators
bool Symbol::operator==(const Symbol& rhs) const
{
	if(this->symbolType != rhs.symbolType)
		return false;

	switch (this->symbolType)
	{
	case SymbolType::LocalVariable:
		return this->identifier == rhs.identifier;
	case SymbolType::Temporary:
		return this->number == rhs.number;
	}

	NOT_IMPLEMENTED_ERROR;
	return false;
}

//Public methods
String IR::Symbol::ToString() const
{
	if (this->symbolType == SymbolType::Null)
		return this->ToStringWithoutType();
	return this->ToStringWithoutType() + u8": " + this->type.ToString();
}

String IR::Symbol::ToStringWithoutType() const
{
	switch (this->symbolType)
	{
	case SymbolType::Constant:
		return u8"c" + String::Number(this->number);
	case SymbolType::GlobalVariable:
		return u8"%" + this->identifier;
	case SymbolType::LocalVariable:
		return u8"$" + this->identifier;
	case SymbolType::Null:
		return u8"null";
	case SymbolType::Procedure:
		return u8"__func_" + String::Number(this->number);
	case SymbolType::Temporary:
		return u8"t" + String::Number(this->number);
	case SymbolType::This:
		return u8"_this";
	}
	
	NOT_IMPLEMENTED_ERROR;
	return "";
}