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
#include "../IR/visitors/AllSymbols.hpp"

namespace Optimization
{
	class ValueEvaluator : private IR::ValueVisitor, private IR::BasicBlockVisitor
	{
		class EvaluatedValue : public IR::Value
		{
		public:
			String ToString() const override
			{
				return String();
			}

			void Visit(ValueVisitor &visitor) override
			{

			}
		};

		class Float64Value : public EvaluatedValue
		{
		public:
			float64 value;

			inline Float64Value(float64 value) : value(value)
			{
			}
		};

		class TupleValue : public EvaluatedValue
		{
		public:
			DynamicArray<IR::Value*> entries;

			inline TupleValue(DynamicArray<IR::Value*>&& entries) : entries(Move(entries))
			{
			}
		};
	public:
		//Methods
		IR::Value* Evaluate(IR::Value* value)
		{
			//TODO: store instruction
			NOT_IMPLEMENTED_ERROR;
			return nullptr;

			value->Visit(*this);

			return this->valueStack.Pop();
		}

	private:
		//Members
		DynamicArray<IR::Value*> valueStack;
		DynamicArray<IR::Value*> parameterStack;
		DynamicArray<UniquePointer<IR::Value>> generatedValues;

		//Methods
		void OnVisitingConstantFloat(const IR::ConstantFloat &constantFloat) override
		{
			IR::Value* value = new Float64Value(constantFloat.Value());
			this->generatedValues.Push(value);
			this->valueStack.Push(value);
		}

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
			this->valueStack.Push(this->parameterStack.Last());
		}

		void OnVisitingProcedure(const IR::Procedure &procedure) override
		{
			this->valueStack.Push(const_cast<IR::Procedure *>(&procedure));
		}

		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
			callInstruction.function->Visit(*this);
			IR::Procedure* proc = dynamic_cast<IR::Procedure *>(this->valueStack.Pop());

			callInstruction.argument->Visit(*this);
			this->parameterStack.Push(this->valueStack.Pop());
			proc->EntryBlock()->Visit(*this);
		}

		void OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction) override
		{
			NOT_IMPLEMENTED_ERROR;
		}

		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override
		{
			externalCallInstruction.argument->Visit(*this);
		}

		void OnVisitingNewObjectInstruction(IR::CreateNewObjectInstruction &createNewObjectInstruction) override
		{
			this->valueStack.Push(&createNewObjectInstruction);
		}

		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override
		{
			DynamicArray<IR::Value*> entries;
			for(auto& entry : createNewTupleInstruction.Values())
			{
				entry->Visit(*this);
				entries.Push(this->valueStack.Pop());
			}
			IR::Value* result = new TupleValue(Move(entries));
			this->generatedValues.Push(result);
			this->valueStack.Push(result);
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{
			returnInstruction.returnValue->Visit(*this);
			this->parameterStack.Pop();
		}

		void OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction) override
		{
			selectInstruction.inner->Visit(*this);
			selectInstruction.selector->Visit(*this);

			IR::ConstantString* selector = dynamic_cast<IR::ConstantString*>(this->valueStack.Pop());
			IR::CreateNewObjectInstruction* inner = dynamic_cast<IR::CreateNewObjectInstruction*>(this->valueStack.Pop());

			this->valueStack.Push(inner->Members()[selector->Value()]);
		}
	};
}