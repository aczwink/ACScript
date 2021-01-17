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
#include "../Instruction.hpp"
#include "../BasicBlock.hpp"

namespace IR
{
	class BranchOnTrueInstruction : public Instruction
	{
	public:
		//Constructor
		inline BranchOnTrueInstruction(Value* condition, BasicBlock* thenBlock, BasicBlock* elseBlock)
			: condition(condition), thenBlock(thenBlock), elseBlock(elseBlock)
		{
		}

		//Properties
		inline Value* Condition()
		{
			return this->condition;
		}

		inline const BasicBlock* ElseBlock() const
		{
			return this->elseBlock;
		}

		//Methods
		void Visit(BasicBlockVisitor &visitor) override
		{
			visitor.OnVisitingConditionalBranchInstruction(*this);
		}

		String ToString() const override
		{
			return u8"br " + this->condition->ToReferenceString() + u8", " + this->thenBlock->Name() + u8", " + this->elseBlock->Name();
		}

	private:
		//Members
		Value* condition;
		BasicBlock* thenBlock;
		BasicBlock* elseBlock;
	};
}