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
	class ProcedureTypeInferer : private ProcedureTransformer, private IR::BasicBlockVisitor
	{
	public:
		//Constructor
		inline ProcedureTypeInferer(IR::Procedure& procedure, TypeCatalog& typeCatalog) : ProcedureTransformer(procedure, typeCatalog)
		{
		}

		void Transform() override
		{
			for(const auto& basicBlock : this->procedure.BasicBlocks())
				basicBlock->Visit(*this);
		}

	private:
		//Methods
		void InferTypesMustBeEqual(const ::Type* type, const ::Type* mustBeType) const;
		void ReplaceType(const ::Type* type, const ::Type* mustBeType) const;

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override;
		void OnVisitingConditionalBranchInstruction(const IR::BranchOnTrueInstruction &branchOnTrueInstruction) override;
		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override;
		void OnVisitingNewObjectInstruction(const IR::CreateNewObjectInstruction &createNewObjectInstruction) override;
		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override;
		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override;
	};
}