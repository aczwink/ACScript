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
//Local
#include "ConstantFloat.hpp"
#include "visitors/Visitor.hpp"
#include "External.hpp"
#include "Procedure.hpp"

namespace IR
{
	class Module
	{
	public:
		//Inline
		inline void AddExternal(External* external)
		{
			this->externals.Insert(external->name, external);
		}

		inline void AddProcedure(Procedure* procedure)
		{
			this->procedures.Push(procedure);
		}

		inline const External* FindExternal(const String& externalName) const
		{
			auto it = this->externals.Find(externalName);
			if(it == this->externals.end())
				return nullptr;
			return (*it).value;
		}

		inline void Visit(Visitor& visitor) const
		{
			for(const auto& kv : this->externals)
				kv.value->Visit(visitor);

			for(const Procedure*const& procedure : this->procedures)
				procedure->Visit(visitor);
		}

	private:
		//Members
		Map<String, External*> externals;
		DynamicArray<Procedure*> procedures;
	};
}