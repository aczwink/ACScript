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
#include "TypeSpec.hpp"

namespace AST
{
	class TupleTypeSpec : public TypeSpec
	{
	public:
		//Constructor
		TupleTypeSpec(UniquePointer<TypeSpec>&& entry, bool repeatLastEntry = false)
			: repeatLastEntry(repeatLastEntry)
		{
			this->entries.InsertTail(Move(entry));
		}

		//Properties
		inline const LinkedList<UniquePointer<TypeSpec>>& Entries() const
		{
			return this->entries;
		}

		inline bool RepeatLastEntry() const
		{
			return this->repeatLastEntry;
		}

		//Methods
		void Visit(TypeSpecVisitor &visitor) const override
		{
			visitor.OnVisitedTupleTypeSpec(*this);
		}

		//Inline
		inline void AddTypeSpec(UniquePointer<TypeSpec>&& typeSpec)
		{
			this->entries.InsertFront(Move(typeSpec));
		}

	private:
		//Members
		LinkedList<UniquePointer<TypeSpec>> entries;
		bool repeatLastEntry;
	};
}