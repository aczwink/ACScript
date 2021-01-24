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

#include "ValueEvaluator.hpp"
#include "DependencyGraph.hpp"

namespace Optimization
{
	class ForwardEvaluator : public ValueEvaluator
	{
	public:
		//Methods
		IR::Value *Evaluate(IR::Procedure* proc, IR::Value* parameter)
		{
			Optimization::DependencyGraph dependencyGraph(*proc);
			for(const auto& kv : dependencyGraph.InstructionDependencies())
				this->instructionReferenceCounts[kv.key] = kv.value.GetNumberOfElements();

			this->parameterStack.Push(parameter);
			proc->EntryBlock()->Visit(*this);

			this->instructionReferenceCounts.Release();

			return this->valueStack.Pop();
		}

	private:
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{
			returnInstruction.returnValue->Visit(*this);
			this->parameterStack.Pop();
		}

		void OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		//ValueVisitor
		void OnVisitingConstantString(const IR::ConstantString &constantString) override
		{
			this->valueStack.Push(const_cast<IR::ConstantString *>(&constantString));
		}

		void OnVisitingExternal(const IR::External &external) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingInstructionResultValue(IR::Instruction &instruction) override
		{
			this->PushValue(&instruction);
		}

		void OnVisitingParameter(const IR::Parameter &parameter) override
		{
			this->valueStack.Push(this->parameterStack.Last());
		}

	private:
		//Members
		DynamicArray<IR::Value*> parameterStack;
	};
}