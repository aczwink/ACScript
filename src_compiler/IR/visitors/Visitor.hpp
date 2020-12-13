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
#pragma once
#include <StdXX.hpp>
using namespace StdXX;

namespace IR
{
	//Forward declarations
	class BasicBlock;
	class ConstantFloat;
	class CreateNewTupleInstruction;
	class External;
	class ExternalCallInstruction;
	class Procedure;
	class ReturnInstruction;

	class Visitor
	{
	public:
		//Methods
		virtual void OnVisitedNewTupleInstruction(const CreateNewTupleInstruction& createNewTupleInstruction) = 0;

		virtual void OnVisitingConstant(const ConstantFloat& constantFloat) = 0;
		virtual void OnVisitingExternal(const External& external) = 0;
		virtual void OnVisitingExternalCallInstruction(const ExternalCallInstruction& externalCallInstruction) = 0;
		virtual void OnVisitingNewTupleInstruction(const CreateNewTupleInstruction& createNewTupleInstruction) = 0;
		virtual void OnVisitingProcedure(const Procedure& procedure) = 0;
		virtual void OnVisitingReturnInstruction(const ReturnInstruction& returnInstruction) = 0;
	};
}