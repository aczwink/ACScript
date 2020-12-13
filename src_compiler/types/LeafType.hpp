/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#include "Type.hpp"

enum class LeafTypeEnum
{
	Null,
	Float64,
	Any,
};

class LeafType : public ::Type
{
public:
	//Constructor
	inline LeafType(LeafTypeEnum type) : type(type)
	{
	}

	//Public methods
	String ToString() const override
	{
		switch (this->type)
		{
			case LeafTypeEnum::Null:
				return u8"null";
			case LeafTypeEnum::Float64:
				return u8"float64";
			case LeafTypeEnum::Any:
				return u8"any";
		}
		RAISE(ErrorHandling::IllegalCodePathError);
	}

private:
	//Members
	LeafTypeEnum type;
};