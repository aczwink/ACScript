/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#include "../ProcedureTransformer.hpp"

namespace Optimization
{
	class ProcedureTypeReplacer : public ProcedureTransformer, private IR::BasicBlockVisitor, private TypeVisitor
	{
	public:
		//Constructor
		inline ProcedureTypeReplacer(IR::Procedure& procedure, TypeCatalog& typeCatalog, const ::Type* typeToReplace, const ::Type* newType)
			: ProcedureTransformer(procedure, typeCatalog), typeToReplace(typeToReplace), newType(newType)
		{
		}

		void Transform() override
		{
			this->ReplaceType(this->procedure.argumentType);
			this->ReplaceType(this->procedure.returnType);

			for(const auto& basicBlock : this->procedure.BasicBlocks())
				basicBlock->Visit(*this);
		}

	private:
		//Members
		const ::Type* typeToReplace;
		const ::Type* newType;
		DynamicArray<const ::Type*> typeStack;

		//Methods
		void ReplaceType(const ::Type*& type)
		{
			if(type == nullptr)
				return;

			type->Visit(*this);
			type = this->typeStack.Pop();
		}

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
		}

		void OnVisitingConditionalBranchInstruction(const IR::BranchOnTrueInstruction &branchOnTrueInstruction) override
		{
		}

		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override
		{
		}

		void OnVisitingNewObjectInstruction(const IR::CreateNewObjectInstruction &createNewObjectInstruction) override
		{
		}

		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override
		{
			this->ReplaceType(createNewTupleInstruction.type);
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{
		}

		void OnVisitingGenericType(const GenericType &genericType) override
		{
			if(&genericType == this->typeToReplace)
				this->typeStack.Push(this->newType);
			else
				this->typeStack.Push(&genericType);
		}

		void OnVisitingLeafType(const LeafType &leafType) override
		{
			if(&leafType == this->typeToReplace)
				this->typeStack.Push(this->newType);
			else
				this->typeStack.Push(&leafType);
		}

		void OnVisitingTupleType(const TupleType &tupleType) override
		{
			DynamicArray<const ::Type*> newTypes;
			for(const auto& nested : tupleType.Types())
			{
				if(nested == nullptr)
				{
					newTypes.Push(nullptr);
					continue;
				}
				nested->Visit(*this);
				newTypes.Push(this->typeStack.Pop());
			}
			this->typeStack.Push(this->typeCatalog.GetTupleType(Move(newTypes)));
		}
	};
}