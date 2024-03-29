/*
* Copyright (c) 2018-2022 Amir Czwink (amir130@hotmail.de)
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
#include "Opcode.hpp"
#include "MarkAndSweepGC.hpp"

//Public methods
void VM::Run()
{
    DynamicArray<const uint8*> callStack;
    DynamicArray<RuntimeValue> executionStack;
    MarkAndSweepGC gc(executionStack);

    executionStack.Push(RuntimeValue()); //arg for main function

    const uint8* pc = static_cast<const uint8 *>(this->module.EntryPoint());
    while(true)
    {
        Opcode op = static_cast<Opcode>(*pc++);
        switch(op)
        {
            case Opcode::Call:
            {
                RuntimeValue arg = executionStack.Pop();
                RuntimeValue func = executionStack.Pop();

                if(func.Type() == RuntimeValueType::External)
                {
                    RuntimeValue result = func.ValueExternal()(arg, this->module, gc);
                    executionStack.Push(result);
                }
                else
                {
                    callStack.Push(pc);
                    pc = static_cast<const uint8 *>(this->module.GetCodeAtOffset(func.ValueNatural().ClampTo64Bit()));
                    executionStack.Push(arg);
                }
            }
            break;
            case Opcode::JumpOnFalse:
            {
                uint64 offset = executionStack.Pop().ValueUInt64();

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
            case Opcode::LoadExternal:
            {
                RuntimeValue externalName = executionStack.Pop();
                const auto& external = this->module.GetExternal(externalName.ValueString());
                executionStack.Push(external);
            }
            break;
            case Opcode::NewDictionary:
            {
                executionStack.Push(RuntimeValue::CreateDictionary());
            }
            break;
            case Opcode::NewTuple:
            {
                uint16 nEntries = this->ExtractUInt16FromProgramCounter(pc);

                GCObject* gcObject = gc.CreateArray();
                gcObject->array->Resize(nEntries);

                executionStack.Push(RuntimeValue(RuntimeValueType::Tuple, gcObject));
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
            case Opcode::PushUInt16:
            {
                uint16 value = this->ExtractUInt16FromProgramCounter(pc);
                executionStack.Push((uint64)value);
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
            case Opcode::Select:
            {
                RuntimeValue selector = executionStack.Pop();
                RuntimeValue source = executionStack.Pop();

                if(source.Type() == RuntimeValueType::Dictionary)
                    executionStack.Push(source.ValuesDictionary()[selector.ValueString()]);
                else if(source.Type() == RuntimeValueType::Tuple)
                    executionStack.Push(source.ValuesArray()[selector.ValueNatural().ClampTo64Bit()]);
                else
                    executionStack.Push(this->module.GetExternal(selector.ValueString()));
            }
            break;
            case Opcode::Set:
            {
                RuntimeValue value = executionStack.Pop();
                RuntimeValue key = executionStack.Pop();
                RuntimeValue target = executionStack.Pop();

                if(target.Type() == RuntimeValueType::Tuple)
                    target.ValuesArray()[key.ValueNatural().ClampTo64Bit()] = value;
                else
                    target.ValuesDictionary().Insert(key.ValueString(), value);
            }
            break;
            default:
                NOT_IMPLEMENTED_ERROR; //TODO: implement me
        }
    }
}

//Private methods
void VM::DumpStack(const DynamicArray<RuntimeValue> &stack) const
{
    DynamicArray<RuntimeValue> executionStack;
    MarkAndSweepGC gc(executionStack);

    External external = this->module.GetExternal(u8"print");

    stdOut << u8"Stack:" << endl;

    auto it = stack.end();
    for(--it; it != stack.begin(); --it)
    {
        external((RuntimeValue&)*it, this->module, gc);
    }
}