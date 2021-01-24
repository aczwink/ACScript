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
#pragma once
#include <StdXX.hpp>
using namespace StdXX;

namespace IR
{
	//Forward declarations
	class BranchOnTrueInstruction;
	class CallInstruction;
	class CreateNewObjectInstruction;
	class CreateNewTupleInstruction;
	class ExternalCallInstruction;
	class ReturnInstruction;
	class SelectInstruction;
	class StoreInstruction;

	class BasicBlockVisitor
	{
	public:
		//Abstract
		virtual void OnVisitingCallInstruction(CallInstruction& callInstruction) = 0;
		virtual void OnVisitingConditionalBranchInstruction(BranchOnTrueInstruction& branchOnTrueInstruction) = 0;
		virtual void OnVisitingExternalCallInstruction(const ExternalCallInstruction& externalCallInstruction) = 0;
		virtual void OnVisitingNewObjectInstruction(CreateNewObjectInstruction& createNewObjectInstruction) = 0;
		virtual void OnVisitingNewTupleInstruction(CreateNewTupleInstruction& createNewTupleInstruction) = 0;
		virtual void OnVisitingReturnInstruction(ReturnInstruction& returnInstruction) = 0;
		virtual void OnVisitingSelectInstruction(SelectInstruction& selectInstruction) = 0;
		virtual void OnVisitingStoreInstruction(StoreInstruction& storeInstruction) = 0;
	};
}