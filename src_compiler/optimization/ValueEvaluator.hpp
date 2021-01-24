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
#include "../IR/visitors/AllSymbols.hpp"

namespace Optimization
{
	class EvaluatedValue : public virtual IR::Value
	{
	public:
		String ToString() const override
		{
			return String();
		}

		void Visit(IR::ValueVisitor &visitor) override
		{

		}
	};

	class DictValue : public virtual EvaluatedValue, public virtual IR::ObjectValue
	{
	public:
		String ToString() const override
		{
			return String();
		}

		void Visit(IR::ValueVisitor &visitor) override
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

	class ValueEvaluator : protected IR::BasicBlockVisitor, protected IR::ValueVisitor
	{
	public:
		//Inline
		inline void TakeOwnership(ValueEvaluator& valueEvaluator, IR::Value* value)
		{
			for(auto& gen : valueEvaluator.generatedValues)
			{
				if(gen.operator->() == value)
				{
					this->generatedValues.Push(Move(gen));
					break;
				}
			}
		}

	protected:
		//Members
		DynamicArray<IR::Value*> valueStack;
		Map<const IR::Value*, uint32> instructionReferenceCounts;

		//Methods
		void PushValue(IR::Value* value)
		{
			int32 res = this->valueStack.Find(value);
			if(res == -1 && this->instrGenerationMap.Contains(value))
				res = this->valueStack.Find(this->instrGenerationMap[value]);
			ASSERT(res != -1, u8"Value not found on stack");

			if(--this->instructionReferenceCounts[value] > 0)
				this->valueStack.Push(this->valueStack[res]);
		}

	private:
		//Members
		DynamicArray<UniquePointer<IR::Value>> generatedValues;
		Map<IR::Value*, IR::Value*> instrGenerationMap;

		//BasicBlockVisitor
		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override
		{
			//TODO: create bound external call
			//externalCallInstruction.argument->Visit(*this);
			this->valueStack.Pop();
			this->valueStack.Push(const_cast<IR::ExternalCallInstruction *>(&externalCallInstruction));
		}

		void OnVisitingNewObjectInstruction(IR::CreateNewObjectInstruction &createNewObjectInstruction) override
		{
			IR::Value* newDict = new DictValue;
			this->generatedValues.Push(newDict);
			this->valueStack.Push(newDict);

			this->instrGenerationMap.Insert(&createNewObjectInstruction, newDict);
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

			this->instrGenerationMap.Insert(&createNewTupleInstruction, result);
		}

		void OnVisitingStoreInstruction(IR::StoreInstruction &storeInstruction) override
		{
			storeInstruction.objectValue->Visit((IR::ValueVisitor &) *this);
			storeInstruction.keyValue->Visit(*this);
			storeInstruction.value->Visit(*this);

			IR::Value* value = this->valueStack.Pop();
			IR::ConstantString* key = dynamic_cast<IR::ConstantString *>(this->valueStack.Pop());
			DictValue* object = dynamic_cast<DictValue *>(this->valueStack.Pop());

			object->AddMember(key, value);

			this->instrGenerationMap.Insert(&storeInstruction, object);
		}

		//ValueVisitor
		void OnVisitingConstantFloat(const IR::ConstantFloat &constantFloat) override
		{
			this->valueStack.Push(const_cast<IR::ConstantFloat *>(&constantFloat));
		}

		void OnVisitingProcedure(const IR::Procedure &procedure) override
		{
			this->valueStack.Push(const_cast<IR::Procedure *>(&procedure));
		}
	};
}