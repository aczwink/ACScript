/*
* Copyright (c) 2018-2019 Amir Czwink (amir130@hotmail.de)
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
//Namespaces
using namespace ACSB;

//Destructor
VM::~VM()
{
}

//Private methods
void VM::ExecuteTask(RuntimeCall* task)
{
	if(task->running.TestAndSet())
		return;

	auto& pc = task->pc;
	auto& executionStack = task->executionStack;

	while (true)
	{
		switch (*pc++)
		{
		case Opcode::Access:
		{
			RuntimeObject accessor = executionStack.Pop();
			RuntimeObject container = executionStack.Pop();

			executionStack.Push(container[accessor]);
		}
		break;
		case Opcode::AssignGlobal:
		{
			uint16 globalIndex = *(uint16*)pc;
			pc += sizeof(globalIndex);

			RuntimeObject value = executionStack.Pop();

			task->module->globals[globalIndex] = value;
		}
		break;
		case Opcode::AssignLocal:
		{
			uint16 localIndex = *(uint16*)pc;
			pc += sizeof(localIndex);

			RuntimeObject value = executionStack.Pop();

			task->locals[localIndex] = value;
		}
		break;
		case Opcode::Call:
		{
			RuntimeObject thisObjCall = executionStack.Pop();
			RuntimeObject called = executionStack.Pop();
			RuntimeObject arg = executionStack.Pop();
			switch (called.GetType())
			{
			case RuntimeObjectType::NativeFunction:
				called.GetValue().func(arg);
				break;
			case RuntimeObjectType::Function:
			{
				RuntimeCall* call = new RuntimeCall(called.GetValue().proc, task->module, thisObjCall, arg);
				RuntimeObject ref = call->CreateRef(task);
				executionStack.Push(ref);
				this->AddTask(call);
			}
			break;
			default:
				NOT_IMPLEMENTED_ERROR;
			}
		}
		break;
		case Opcode::CreateArray:
		{
			executionStack.Push(RuntimeObject::Array());
		}
		break;
		case Opcode::LoadConstant:
		{
			uint32 constantIndex = *(uint32*)pc;
			pc += sizeof(constantIndex);
			executionStack.Push(task->module->module->GetConstant(constantIndex));
		}
		break;
		case Opcode::Push:
		{
			uint32 stackIndex = *(uint32*)pc;
			pc += sizeof(stackIndex);

			RuntimeObject obj = executionStack[stackIndex];
			executionStack.Push( obj );
		}
		break;
		case Opcode::PushGlobal:
		{
			uint16 globalIndex = *(uint16*)pc;
			pc += sizeof(globalIndex);

			executionStack.Push(task->module->globals[globalIndex]);
		}
		break;
		case Opcode::PushLocal:
		{
			uint16 localIndex = *(uint16*)pc;
			pc += sizeof(localIndex);

			executionStack.Push(task->locals[localIndex]);
		}
		break;
		case Opcode::PushNull:
		{
			executionStack.Push({});
		}
		break;
		case Opcode::PushProc:
		{
			uint32 procIndex = *(uint32*)pc;
			pc += sizeof(procIndex);
			executionStack.Push(&task->module->module->GetProcedure(procIndex));
		}
		break;
		case Opcode::PushThis:
		{
			executionStack.Push(task->thisObj);
		}
		break;
		case Opcode::Return:
		{
			RuntimeObject ret = executionStack.Pop();
			if (task->ref != RuntimeObject())
			{
				*task->ref.GetTaskRefData().returnValue = ret;
				task->ref.GetTaskRefData().finished = true;
				if (!task->ref.GetTaskRefData().caller->running.TestAndSet())
				{
					//task isn't running
					task->ref.GetTaskRefData().caller->running.Clear();
					this->ExecuteTask(task->ref.GetTaskRefData().caller);//i can continue on the task
				}
			}
			return;
		}
		break;
		case Opcode::Wait:
		{
			RuntimeObject obj = executionStack.Last();
			if(!obj.IsTaskReady())
			{
				pc--; //point back to wait instruction
				task->running.Clear();
				return; //pause execution
			}
		}
		break;
		default:
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}
}

/*
//Destructor
VM::~VM()
{
	for (const auto& kv : this->modules)
		delete kv.value.program;
}

//Private methods
void VM::Run()
{
	while (!this->callStack.IsEmpty())
	{
			{
			case Opcode::Add:
			{
				RuntimeObjectValue right = this->valueStack.Pop().GetValue();
				RuntimeObjectValue left = this->valueStack.Pop().GetValue();

				this->valueStack.Push({ left.u64 + right.u64 });
			}
			break;
			case Opcode::Equals:
			{
				this->valueStack.Push(this->valueStack.Pop() == this->valueStack.Pop());
			}
			break;
			case Opcode::JumpOnFalse:
			{
				int8 delta = *pc++;
				if(this->valueStack.Pop().GetValue().b == false)
					pc += delta;
			}
			break;
			case Opcode::JumpRelative:
			{
				int8 delta = *pc++;
				pc += delta;
			}
			break;
			case Opcode::LessOrEqual:
			{
				RuntimeObjectValue right = this->valueStack.Pop().GetValue();
				RuntimeObjectValue left = this->valueStack.Pop().GetValue();
				
				this->valueStack.Push({ left.u64 <= right.u64 });
			}
			break;
			case Opcode::Minus:
			{
				RuntimeObjectValue right = this->valueStack.Pop().GetValue();
				RuntimeObjectValue left = this->valueStack.Pop().GetValue();

				this->valueStack.Push({left.u64 - right.u64});
			}
			break;
			case Opcode::Multiply:
			{
				RuntimeObjectValue right = this->valueStack.Pop().GetValue();
				RuntimeObjectValue left = this->valueStack.Pop().GetValue();

				this->valueStack.Push({ left.u64 * right.u64 });
			}
			break;
			case Opcode::Pop:
			{
				this->valueStack.Pop();
			}
			break;
			case Opcode::PushBool:
			{
				bool b = (*pc++) != 0;
				this->valueStack.Push({b});
			}
			break;
			case Opcode::PushSelf:
			{
				this->valueStack.Push(call.function);
			}
			break;
			}
		}
	}
}*/