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
#include <StdXX.hpp>
using namespace StdXX;
//Local
#include "../IR/visitors/AllSymbols.hpp"
#include "../IR/Procedure.hpp"

namespace Optimization
{
	class CallGraph : private IR::BasicBlockVisitor
	{
	public:
		//Constructor
		inline CallGraph(const LinkedList<IR::Procedure*>& procedures)
		{
			for(IR::Procedure*const& procedure : procedures)
			{
				this->callLinks[procedure] = {};

				this->currentProc = procedure;
				this->AnalyzeCalls(procedure);
			}
			this->currentProc = nullptr;
		}

		//Methods
		DynamicArray<IR::Procedure*> QueryProceduresOrdered()
		{
			DynamicArray<IR::Procedure*> result;
			BinaryTreeSet<const IR::Procedure*> marked;

			for(auto& kv : this->callLinks)
				Collect(result, marked, kv.key);

			return result;
		}

	private:
		//Members
		IR::Procedure* currentProc;
		Map<IR::Procedure*, BinaryTreeSet<IR::Procedure*>> callLinks;

		//Inline
		inline void AnalyzeCalls(IR::Procedure*const& procedure)
		{
			for(const auto& basicBlock : procedure->BasicBlocks())
				basicBlock->Visit(*this);
		}

		inline void Collect(DynamicArray<IR::Procedure*>& result, BinaryTreeSet<const IR::Procedure*>& marked, IR::Procedure*const& procedure)
		{
			if(marked.Contains(procedure))
				return;
			marked.Insert(procedure); //mark now because of self-recursive functions

			BinaryTreeSet<IR::Procedure*>& dependencies = this->callLinks[procedure];
			for(auto& dependency : dependencies)
				Collect(result, marked, dependency);

			result.Push(procedure);
		}

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
			IR::Procedure* called = const_cast<IR::Procedure *>(dynamic_cast<const IR::Procedure *>(callInstruction.function));

			this->callLinks[this->currentProc].Insert(called);
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
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{
		}
	};
}