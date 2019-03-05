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
#include "Type.hpp"
//Namespaces
using namespace IR;

//Methods
String Type::ToString() const
{
	String tmp;

	if (this->isConst)
		tmp += u8"const ";

	if (this->isSimple)
	{
		switch (this->leafType)
		{
		case LeafType::Any:
			tmp += u8"any";
			break;
		case LeafType::I64:
			tmp += u8"i64";
			break;
		case LeafType::Null:
			tmp += u8"null";
			break;
		case LeafType::String:
			tmp += u8"string";
			break;
		case LeafType::U64:
			tmp += u8"u64";
			break;
		default:
			NOT_IMPLEMENTED_ERROR;
		}
	}
	else
		NOT_IMPLEMENTED_ERROR;

	return tmp;
}