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
#include "AccessOperation.hpp"
#include "CallOperation.hpp"
#include "CreateArrayOperation.hpp"
#include "StoreOperation.hpp"
#include "WaitOperation.hpp"

namespace IR
{
	//Forward declarations
	class Procedure;
	class Program;

	class Block
	{
	public:
		//Constructors
		inline Block(Procedure& proc) : proc(proc), parent(nullptr)
		{
		}

		inline Block(Block&& src, Procedure& proc) : operations(Move(src.operations)), proc(proc), parent(src.parent)
		{
		}

		//Methods
		const Symbol& AddLocalVariable(const String& identifier, bool isMutable);
		bool CanDeriveIdentifier(const String& identifier) const;
		void Compile(Compiler& compiler);
		Symbol DeriveIdentifierSymbol(const String& identifier);
		Program& GetProgram();
		const Program& GetProgram() const;
		String ToString(uint32 nTabs = 0) const;

		//Inline
		inline const Symbol& AddAccess(Symbol container, Symbol accessor)
		{
			return this->AddValueOperation(new AccessOperation(Move(container), Move(accessor), *this));
		}

		inline const Symbol& AddCall(Symbol&& function, Symbol&& arg)
		{
			return this->AddValueOperation(new CallOperation(Symbol::Null(), Move(function), Move(arg), *this));
		}

		inline const Symbol& AddCall(Symbol context, Symbol function, Symbol arg)
		{
			return this->AddValueOperation(new CallOperation(Move(context), Move(function), Move(arg), *this));
		}

		inline const Symbol& AddCreateArray()
		{
			return this->AddValueOperation(new CreateArrayOperation(*this));
		}

		inline void AddStore(Symbol&& lhs, Symbol&& rhs)
		{
			this->operations.Push( new StoreOperation(Move(lhs), Move(rhs), *this));
		}

		inline const Symbol& AddValueOperation(ValueOperation* operation)
		{
			this->operations.Push(operation);
			return operation->GetValue();
		}

		inline void AddWait(Symbol&& waitSymbol)
		{
			this->operations.Push(new WaitOperation(Move(waitSymbol), *this));
		}

		inline Procedure& GetProcedure()
		{
			return this->proc;
		}

		inline void SetParent(Block* block)
		{
			this->parent = block;
		}

		inline void Verify() const
		{
			for (const auto& op : this->operations)
				op->Verify();
		}

	private:
		//Members
		Procedure& proc;
		Block* parent;
		DynamicArray<Block*> children;
		DynamicArray<UniquePointer<Operation>> operations;
		LinkedList<Symbol> variables;
		Map<String, String> localToGlobalMap;
		Map<String, bool> localMutable;

		//Methods
		Block* GetLocalOwner(const String& identifier);
		void MakeLocalGlobal(const String& identifier);
		void ReplaceSymbol(const Symbol& from, const Symbol& to);
	};
}