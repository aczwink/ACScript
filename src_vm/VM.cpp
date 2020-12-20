/*
* Copyright (c) 2018-2020 Amir Czwink (amir130@hotmail.de)
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
//Class header
#include "VM.hpp"
//Local
#include <acsb/Opcode.hpp>

//Public methods
void VM::Run()
{
	const uint8* pc = static_cast<const uint8 *>(this->module.EntryPoint());
	DynamicArray<RuntimeValue> executionStack;
	DynamicArray<const uint8*> callStack;
	DynamicArray<RuntimeValue> argumentStack;

	while(true)
	{
		Opcode op = static_cast<Opcode>(*pc++);
		switch (op)
		{
			case Opcode::Call:
			{
				uint16 offset = this->ExtractUInt16FromProgramCounter(pc);

				callStack.Push(pc);
				pc = static_cast<const uint8 *>(this->module.GetCodeAtOffset(offset));

				argumentStack.Push(executionStack.Pop());
			}
			break;
			case Opcode::CallExtern:
			{
				uint16 externIndex = this->ExtractUInt16FromProgramCounter(pc);
				const auto& arg = executionStack.Pop();
				auto result = this->module.GetExternal(externIndex)(arg);
				executionStack.Push(result);
			}
			break;
			case Opcode::JumpOnFalse:
			{
				uint16 offset = this->ExtractUInt16FromProgramCounter(pc);

				if(!executionStack.Pop().ValueBool())
					pc = static_cast<const uint8 *>(this->module.GetCodeAtOffset(offset));
			}
			break;
			case Opcode::LoadConstant:
			{
				uint16 constantIndex = this->ExtractUInt16FromProgramCounter(pc);
				executionStack.Push(this->module.GetConstant(constantIndex));
			}
			break;
			case Opcode::NewTuple:
			{
				uint16 nEntries = this->ExtractUInt16FromProgramCounter(pc);

				this->garbageCollector.CleanUp(argumentStack);

				DynamicArray<RuntimeValue>* values = this->garbageCollector.NewArray();
				for(uint16 i = 0; i < nEntries; i++)
					values->Push(executionStack.Pop());

				executionStack.Push(RuntimeValue::CreateTuple(values));
			}
			break;
			case Opcode::PushParameter:
			{
				executionStack.Push(argumentStack.Last());
			}
			break;
			case Opcode::Return:
			{
				if(callStack.IsEmpty())
					return;
				else
				{
					pc = callStack.Pop();
					argumentStack.Pop();
				}
			}
			break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}
}
