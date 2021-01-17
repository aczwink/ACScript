/*
* Copyright (c) 2021 Amir Czwink (amir130@hotmail.de)
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

#include <Std++/Containers/Strings/String.hpp>

namespace IR
{
	class Scope
	{
	public:
		//Members
		Scope* parent;

		//Constructor
		inline Scope()
		{
			this->parent = nullptr;
		}

		//Inline
		inline void Add(const String& name, IR::Value* value)
		{
			this->namedValues[name] = value;
		}

		inline IR::Value* Resolve(const String& name)
		{
			if(this->namedValues.Contains(name))
				return this->namedValues[name];
			return this->parent ? this->parent->Resolve(name) : nullptr;
		}

	private:
		//Members
		Map<String, IR::Value*> namedValues;
	};
}