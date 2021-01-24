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
#pragma once
#include "ValueEvaluator.hpp"

namespace Optimization
{
	class BackwardsEvaluator : private ValueEvaluator
	{
	public:
		//Methods
		IR::Value* Evaluate(IR::Value* value)
		{
			value->Visit(*this);

			return this->valueStack.Pop();
		}

	private:
		//Methods
		void OnVisitingConstantString(const IR::ConstantString &constantString) override
		{
			this->valueStack.Push((IR::Value*)&constantString);
		}

		void OnVisitingExternal(const IR::External &external) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingInstructionResultValue(IR::Instruction &instruction) override
		{
			instruction.Visit((IR::BasicBlockVisitor&)*this);
		}

		void OnVisitingParameter(const IR::Parameter &parameter) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override;

		void OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{
			returnInstruction.returnValue->Visit(*this);
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction) override
		{
			selectInstruction.inner->Visit(*this);
			selectInstruction.selector->Visit(*this);

			IR::ConstantString* selector = dynamic_cast<IR::ConstantString*>(this->valueStack.Pop());
			IR::ObjectValue* inner = dynamic_cast<IR::ObjectValue*>(this->valueStack.Pop());

			this->valueStack.Push(inner->Members()[selector->Value()]);
		}
	};
}