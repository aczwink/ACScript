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
#include "Procedure.hpp"

namespace IR
{
	class Program
	{
	public:
		//Constructor
		inline Program() : nextConstantIndex(0)
		{
			this->procedures.InsertTail({ u8"main", LeafType::Null, *this}); //insert main procedure
		}

		//Methods
		String ToString() const;

		//Inline
		inline Symbol AddConstant(int64 value)
		{
			if (!this->i64Constants.Contains(value))
				this->i64Constants[value] = this->NextConstantIndex();
			return Symbol::Constant(this->i64Constants[value], LeafType::I64);
		}

		inline Symbol AddConstant(uint64 value)
		{
			if (!this->ui64Constants.Contains(value))
				this->ui64Constants[value] = this->NextConstantIndex();
			return Symbol::Constant(this->ui64Constants[value], LeafType::U64);
		}

		inline Symbol AddConstant(const String& value)
		{
			if (!this->stringConstants.Contains(value))
				this->stringConstants[value] = this->NextConstantIndex();
			return Symbol::Constant(this->stringConstants[value], LeafType::String);
		}

		inline Symbol AddGlobal(const String& identifier, const Type& type, bool isMutable)
		{
			this->globals.InsertTail(Symbol::GlobalVariable(identifier, type));
			this->globalMuteable.Insert(identifier, isMutable);
			return this->globals[this->globals.GetNumberOfElements() - 1];
		}

		inline Procedure& AddProcedure(const Type& t)
		{
			this->procedures.InsertTail({ u8"__func_" + String::Number(this->procedures.GetNumberOfElements()), t, *this });
			return this->procedures[this->procedures.GetNumberOfElements() - 1];
		}

		inline void Compile(Compiler& compiler)
		{
			for(Procedure& proc : this->procedures)
				proc.Compile(compiler);
		}

		inline const Map<int64, uint32> GetI64Constants() const
		{
			return this->i64Constants;
		}

		inline Procedure& GetMainProcedure()
		{
			return this->procedures[0];
		}

		inline uint32 GetNumberOfConstants() const
		{
			return this->nextConstantIndex;
		}

		inline uint32 GetNumberOfGlobals() const
		{
			return this->globals.GetNumberOfElements();
		}

		inline const Map<String, uint32> GetStringConstants() const
		{
			return this->stringConstants;
		}

		inline const Map<uint64, uint32> GetUI64Constants() const
		{
			return this->ui64Constants;
		}

		inline Symbol* GetGlobal(const String& identifier)
		{
			for (Symbol& symbol : this->globals)
			{
				if (symbol.GetIdentifier() == identifier)
					return &symbol;
			}
			return nullptr;
		}

		inline Symbol GetProcedureSymbol(const Procedure& procedure)
		{
			auto it = this->procedures.begin();
			while (it != this->procedures.end())
			{
				if(&(*it) == &procedure)
					return Symbol::Procedure(it.GetIndex());
				++it;
			}
			NOT_IMPLEMENTED_ERROR;
		}

		inline bool GlobalIsMutable(const String& globalName) const
		{
			return this->globalMuteable[globalName];
		}

		inline void Verify() const
		{
			for (const Procedure& proc : this->procedures)
				proc.Verify();
		}

	private:
		//Members
		LinkedList<Procedure> procedures;
		uint32 nextConstantIndex;
		Map<int64, uint32> i64Constants;
		Map<uint64, uint32> ui64Constants;
		Map<String, uint32> stringConstants;
		LinkedList<Symbol> globals;
		Map<String, bool> globalMuteable;

		//Inline
		inline uint32 NextConstantIndex()
		{
			return this->nextConstantIndex++;
		}
	};
}