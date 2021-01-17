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

namespace Optimization
{
	class DependencyGraph : private IR::BasicBlockVisitor
	{
	public:
		//Constructor
		inline DependencyGraph(const IR::Procedure& procedure)
		{
			for(const auto& basicBlock : procedure.BasicBlocks())
				basicBlock->Visit(*this);
		}

		//Properties
		inline const Map<IR::Instruction*, BinaryTreeSet<const IR::Value*>>& Dependencies() const
		{
			return this->dependencies;
		}

	private:
		//Members
		Map<IR::Instruction*, BinaryTreeSet<const IR::Value*>> dependencies;

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override
		{
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

		//Inline
		inline void AddDependency(IR::Instruction* target, const IR::Value* referencedValue)
		{
			this->dependencies[target].Insert(referencedValue);
		}
	};
}