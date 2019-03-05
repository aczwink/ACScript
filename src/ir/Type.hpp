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
#pragma once
#include <Std++.hpp>
using namespace StdXX;

namespace IR
{
	enum class LeafType
	{
		Null,
		Any,
		I64,
		String,
		U64,
	};

	class Type
	{
	public:
		//Constructors
		inline Type(LeafType leafType) : isSimple(true), leafType(leafType), isConst(false)
		{
		}

		//Methods
		String ToString() const;

		//Inline
		inline LeafType GetLeafType() const
		{
			return this->leafType;
		}

		inline bool IsLeaf() const
		{
			return this->isSimple;
		}

		inline bool& Const()
		{
			return this->isConst;
		}

	private:
		//Members
		bool isConst;
		bool isSimple;
		LeafType leafType;
	};
}