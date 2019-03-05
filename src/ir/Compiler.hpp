/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
//Local
#include "AccessOperation.hpp"
#include "CallOperation.hpp"
#include "CreateArrayOperation.hpp"
#include "StoreOperation.hpp"
#include "WaitOperation.hpp"

namespace IR
{
	class Compiler
	{
	public:
		//Abstract
		virtual void BeginProcedure() = 0;
		virtual void CompileAccess(const AccessOperation& op) = 0;
		virtual void CompileCall(const CallOperation& op) = 0;
		virtual void CompileCreateArray(const CreateArrayOperation& op) = 0;
		virtual void CompileStore(const StoreOperation& op) = 0;
		virtual void CompileWait(const WaitOperation& op) = 0;
		virtual void EndProcedure() = 0;
	};
}