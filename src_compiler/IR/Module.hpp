/*
* Copyright (c) 2019-2020 Amir Czwink (amir130@hotmail.de)
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
#include "ConstantFloat.hpp"
#include "External.hpp"
#include "Procedure.hpp"

namespace IR
{
	class Module
	{
	public:
		//Properties
		inline const Map<String, External*>& Externals() const
		{
			return this->externals;
		}

		inline LinkedList<Procedure*>& Procedures()
		{
			return this->procedures;
		}

		inline const LinkedList<Procedure*>& Procedures() const
		{
			return this->procedures;
		}

		template<typename Iterable>
		inline void Procedures(const Iterable& iterable)
		{
			this->procedures.Release();
			for(const auto& current : iterable)
				this->procedures.InsertTail(current);
		}

		//Inline
		inline void AddExternal(External* external)
		{
			this->externals.Insert(external->name, external);
		}

		inline void AddProcedure(Procedure* procedure, Procedure* before)
		{
			this->procedures.InsertBefore(this->procedures.Find(before), Move(procedure));
		}

		inline const External* FindExternal(const String& externalName) const
		{
			auto it = this->externals.Find(externalName);
			if(it == this->externals.end())
				return nullptr;
			return (*it).value;
		}

		inline Procedure* GetMainProcedure()
		{
			ASSERT_EQUALS(u8"main", this->procedures.Last()->name);
			return this->procedures.Last();
		}

	private:
		//Members
		Map<String, External*> externals;
		LinkedList<Procedure*> procedures;
	};
}