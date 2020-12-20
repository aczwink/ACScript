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
#pragma once
//Local
#include "Type.hpp"

enum class LeafTypeEnum
{
	Any,
	Bool,
	Float64,
	Null,
};

class LeafType : public ::Type
{
public:
	//Constructor
	inline LeafType(LeafTypeEnum type) : type(type)
	{
	}

	//Public methods
	bool IsTriviallyAssignableTo(const Type &other) const override
	{
		const LeafType* otherLeafType = dynamic_cast<const LeafType *>(&other);
		if(otherLeafType)
		{
			if(this->type == otherLeafType->type)
				return true;

			switch(this->type)
			{
				case LeafTypeEnum::Any:
					NOT_IMPLEMENTED_ERROR;
					break;
				case LeafTypeEnum::Bool:
					NOT_IMPLEMENTED_ERROR;
					break;
				case LeafTypeEnum::Float64:
				{
					switch(otherLeafType->type)
					{
						case LeafTypeEnum::Any:
							return true;
						case LeafTypeEnum::Bool:
							NOT_IMPLEMENTED_ERROR;
							break;
						case LeafTypeEnum::Null:
							NOT_IMPLEMENTED_ERROR;
							break;
					}
				}
					break;
				case LeafTypeEnum::Null:
					NOT_IMPLEMENTED_ERROR;
					break;
			}
		}

		NOT_IMPLEMENTED_ERROR;
		return false;
	}

	String ToString() const override
	{
		switch (this->type)
		{
			case LeafTypeEnum::Any:
				return u8"any";
			case LeafTypeEnum::Bool:
				return u8"bool";
			case LeafTypeEnum::Float64:
				return u8"float64";
			case LeafTypeEnum::Null:
				return u8"null";
		}
		RAISE(ErrorHandling::IllegalCodePathError);
	}

	void Visit(TypeVisitor &visitor) const override
	{
		visitor.OnVisitingLeafType(*this);
	}

private:
	//Members
	LeafTypeEnum type;
};