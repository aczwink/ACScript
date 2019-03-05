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
#include "Type.hpp"

namespace IR
{
	enum class SymbolType
	{
		Constant,
		GlobalVariable,
		LocalVariable,
		Null,
		Procedure,
		Temporary,
		This,
	};

	class Symbol
	{
	public:
		//Operators
		bool operator==(const Symbol& rhs) const;

		//Methods
		String ToString() const;
		String ToStringWithoutType() const;

		//inline
		inline const String& GetIdentifier() const
		{
			return this->identifier;
		}

		inline uint32 GetNumber() const
		{
			return this->number;
		}

		inline SymbolType GetSymbolType() const
		{
			return this->symbolType;
		}

		inline const Type& GetType() const
		{
			return this->type;
		}

		//Functions
		static inline Symbol Constant(uint32 number, LeafType type)
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::Constant;
			symbol.type = Type(type);
			symbol.type.Const() = true;
			symbol.number = number;
			return symbol;
		}

		static inline Symbol GlobalVariable(const String& identifier, const Type& type)
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::GlobalVariable;
			symbol.type = type;
			symbol.identifier = identifier;
			return symbol;
		}

		static inline Symbol LocalVariable(const String& identifier)
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::LocalVariable;
			symbol.identifier = identifier;
			return symbol;
		}

		static inline Symbol Null()
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::Null;
			symbol.type = LeafType::Null;
			return symbol;
		}

		static inline Symbol Procedure(uint32 procNumber)
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::Procedure;
			symbol.number = procNumber;
			return symbol;
		}

		static inline Symbol Temporary(uint32 number)
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::Temporary;
			symbol.number = number;
			return symbol;
		}

		static inline Symbol This()
		{
			Symbol symbol;
			symbol.symbolType = SymbolType::This;
			return symbol;
		}

	private:
		//Members
		SymbolType symbolType;
		Type type;
		uint32 number;
		String identifier;

		//Constructor
		inline Symbol() : type(LeafType::Any)
		{
		}
	};
}