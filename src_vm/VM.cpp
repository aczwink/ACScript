/*
* Copyright (c) 2018-2021 Amir Czwink (amir130@hotmail.de)
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

	executionStack.Push(RuntimeValue()); //arg for main function

	while(true)
	{
		Opcode op = static_cast<Opcode>(*pc++);
		switch (op)
		{
			case Opcode::Call:
			{
				uint16 offset = executionStack.Pop().ValueF64();

				callStack.Push(pc);
				pc = static_cast<const uint8 *>(this->module.GetCodeAtOffset(offset));
			}
			break;
			case Opcode::CallExtern:
			{
				uint16 externIndex = this->ExtractUInt16FromProgramCounter(pc);
				RuntimeValue arg = executionStack.Pop();
				const auto& external = this->module.GetExternal(externIndex);
				auto result = external(arg, this->module);
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
			case Opcode::NewDictionary:
			{
				this->garbageCollector.CleanUp(executionStack);
				Map<String, RuntimeValue>* values = this->garbageCollector.NewDictionary();

				executionStack.Push(RuntimeValue::CreateDictionary(values));
			}
			break;
			case Opcode::NewTuple:
			{
				uint16 nEntries = this->ExtractUInt16FromProgramCounter(pc);

				this->garbageCollector.CleanUp(executionStack);

				DynamicArray<RuntimeValue>* values = this->garbageCollector.NewArray();
				for(uint16 i = 0; i < nEntries; i++)
					values->Push(executionStack.Pop());

				executionStack.Push(RuntimeValue::CreateTuple(values));
			}
			break;
			case Opcode::Pop:
			{
				executionStack.Pop();
			}
			break;
			case Opcode::PopAssign:
			{
				RuntimeValue value = executionStack.Pop();
				executionStack.Last() = value;
			}
			break;
			case Opcode::Push:
			{
				uint16 offset = this->ExtractUInt16FromProgramCounter(pc);
				RuntimeValue value = executionStack[executionStack.GetNumberOfElements() - 1 - offset];
				executionStack.Push(value);
			}
			break;
			case Opcode::Select:
			{
				RuntimeValue selector = executionStack.Pop();
				RuntimeValue object = executionStack.Pop();

				executionStack.Push(object.ValuesDictionary()[selector.ValueString()]);
			}
			break;
			case Opcode::Return:
			{
				if(callStack.IsEmpty())
					return;
				else
					pc = callStack.Pop();
			}
			break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}
}
