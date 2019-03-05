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
//Local
#include "ValueOperation.hpp"

namespace IR
{
	class AccessOperation : public ValueOperation
	{
	public:
		//Constructor
		inline AccessOperation(Symbol&& container, Symbol&& accessor, Block& block) : ValueOperation(block),
			container(Move(container)), accessor(Move(accessor))
		{
		}

		//Methods
		void Compile(Compiler & compiler) const override;
		void ReplaceSymbol(const Symbol & from, const Symbol & to) override;
		String ToString() const override;
		void Verify() const override;

		//Inline
		inline const Symbol& GetAccessor() const
		{
			return this->accessor;
		}

		inline const Symbol& GetContainer() const
		{
			return this->container;
		}

	private:
		//Members
		Symbol container;
		Symbol accessor;
	};
}