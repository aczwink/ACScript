/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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
#include "../optimization/BackwardsEvaluator.hpp"

namespace Optimization
{
	class DependencyGraph : private IR::BasicBlockVisitor
	{
	public:

		//Constructors
		inline DependencyGraph(const IR::Procedure& procedure)
		{
			this->ProcessProcedure(const_cast<IR::Procedure &>(procedure));
		}

		inline DependencyGraph(const LinkedList<IR::Procedure*>& procedures)
		{
			for(const auto& procedure : procedures)
				this->ProcessProcedure(*procedure);
		}

		//Properties
		inline const Map<IR::Instruction*, BinaryTreeSet<const IR::Value*>>& InstructionDependencies() const
		{
			return this->instructionDependencies;
		}

		//Methods
		DynamicArray<IR::Procedure*> QueryProceduresOrdered()
		{
			DynamicArray<IR::Procedure*> result;
			BinaryTreeSet<const IR::Procedure*> marked;

			for(auto& kv : this->procedureDependencies)
				Collect(result, marked, kv.key);

			return result;
		}

	private:
		//Members
		Map<IR::Instruction*, BinaryTreeSet<const IR::Value*>> instructionDependencies;
		Map<IR::Procedure*, BinaryTreeSet<IR::Procedure*>> procedureDependencies;
		IR::Procedure* currentProcedure;

		//Methods
		inline void Collect(DynamicArray<IR::Procedure*>& result, BinaryTreeSet<const IR::Procedure*>& marked, IR::Procedure*const& procedure)
		{
			if(marked.Contains(procedure))
				return;
			marked.Insert(procedure); //mark now because of self-recursive functions

			BinaryTreeSet<IR::Procedure*>& dependencies = this->procedureDependencies[procedure];
			for(auto& dependency : dependencies)
				Collect(result, marked, dependency);

			result.Push(procedure);
		}

		//Inline
		inline void AddDependency(IR::Instruction* target, const IR::Value* referencedValue)
		{
			this->instructionDependencies[target].Insert(referencedValue);
		}

		inline void ProcessProcedure(IR::Procedure &procedure)
		{
			this->currentProcedure = &procedure;
			for(const auto& basicBlock : procedure.BasicBlocks())
				basicBlock->Visit(*this);
		}

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
			Optimization::BackwardsEvaluator evaluator;
			IR::Procedure* procedure = dynamic_cast<IR::Procedure *>(evaluator.Evaluate(callInstruction.function));

			this->procedureDependencies[this->currentProcedure].Insert(procedure);
		}

		void OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction) override
		{

		}

		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override
		{

		}

		void OnVisitingNewObjectInstruction(IR::CreateNewObjectInstruction &createNewObjectInstruction) override
		{
		}

		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override
		{
			for(const IR::Value*const& value : createNewTupleInstruction.Values())
				this->AddDependency(&createNewTupleInstruction, value);
		}

		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override
		{

		}

		void OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction) override
		{
		}

		void OnVisitingStoreInstruction(IR::StoreInstruction &storeInstruction) override
		{
		}
	};
}