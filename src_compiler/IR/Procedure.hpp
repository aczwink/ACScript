/*
* Copyright (c) 2019-2021 Amir Czwink (amir130@hotmail.de)
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
#include "Symbol.hpp"
#include "../types/TupleType.hpp"
#include "BasicBlock.hpp"
#include "Parameter.hpp"

namespace IR
{
	class Procedure : public Symbol
	{
	public:
		//Members
		const ::Type* returnType;
		const ::Type* argumentType;
		Parameter* parameter;
		bool isMethod;

		//Constructor
		inline Procedure(const ::Type* returnType, const ::Type* argumentType, Parameter* parameter)
			: returnType(returnType), argumentType(argumentType), parameter(parameter)
		{
			this->isMethod = false;
		}

		//Properties
		inline const DynamicArray<BasicBlock*>& BasicBlocks() const
		{
			return this->basicBlocks;
		}

		inline BasicBlock* EntryBlock()
		{
			return this->basicBlocks[0];
		}

		//Methods
		void Visit(ValueVisitor &visitor) override
		{
			visitor.OnVisitingProcedure(*this);
		}

		//Inline
		inline void AddBlock(BasicBlock* basicBlock)
		{
			if(this->basicBlocks.IsEmpty())
			{
				basicBlock->scope.Add(u8"self", this);
			}
			this->basicBlocks.Push(basicBlock);
		}

	private:
		//Members
		DynamicArray<BasicBlock*> basicBlocks;
	};
}