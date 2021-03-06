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
	class ConstantFloat;
	class ConstantString;
	class External;
	class Instruction;
	class Parameter;
	class Procedure;

	class ValueVisitor
	{
	public:
		//Abstract
		virtual void OnVisitingConstantFloat(const ConstantFloat& constantFloat) = 0;
		virtual void OnVisitingConstantString(const ConstantString& constantString) = 0;
		virtual void OnVisitingExternal(const External& external) = 0;
		virtual void OnVisitingInstructionResultValue(Instruction& instruction) = 0;
		virtual void OnVisitingParameter(const Parameter& parameter) = 0;
		virtual void OnVisitingProcedure(const Procedure& procedure) = 0;
	};
}