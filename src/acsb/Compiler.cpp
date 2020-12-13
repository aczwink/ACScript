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
//Class header
#include "Compiler.hpp"
//Namespaces
using namespace ACSB;

//Public methods
void ACSB::Compiler::BeginProcedure()
{
	this->nextLocalIdx = 0;
}

void ACSB::Compiler::CompileAccess(const IR::AccessOperation & op)
{
	this->CompileSymbol(op.GetContainer());
	this->CompileSymbol(op.GetAccessor());
	this->opcodes.Push(Opcode::Access);

	this->executionStack.Pop();
	this->executionStack.Pop();
	this->executionStack.Push(&op.GetValue());
}

void ACSB::Compiler::CompileCall(const IR::CallOperation & op)
{
	this->CompileSymbol(op.GetArg());
	this->CompileSymbol(op.GetFunction());
	this->CompileSymbol(op.GetContext());
	this->opcodes.Push(Opcode::Call);

	this->executionStack.Pop();
	this->executionStack.Pop();
	this->executionStack.Pop();
	this->executionStack.Push(&op.GetValue());
}

void Compiler::CompileCreateArray(const IR::CreateArrayOperation & op)
{
	this->AddInstruction(Opcode::CreateArray);
	this->executionStack.Push(&op.GetValue());
}

void Compiler::CompileStore(const IR::StoreOperation& op)
{
	this->CompileSymbol(op.GetRHS());

	switch (op.GetLHS().GetSymbolType())
	{
	case IR::SymbolType::GlobalVariable:
	{
		this->AddInstruction(Opcode::AssignGlobal, this->GetGlobalIndex(op.GetLHS().GetIdentifier()));
	}
	break;
	case IR::SymbolType::LocalVariable:
	{
		this->AddInstruction(Opcode::AssignLocal, this->GetLocalIndex(op.GetLHS().GetIdentifier()));
	}
	break;
	default:
		NOT_IMPLEMENTED_ERROR;
	}

	this->executionStack.Pop();
}

void Compiler::CompileWait(const IR::WaitOperation & op)
{
	this->AddInstruction(Opcode::Wait);
}

void ACSB::Compiler::EndProcedure()
{
	//emergency bail out
	this->AddInstruction(Opcode::PushNull);
	this->AddInstruction(Opcode::Return);

	this->procedures.Push({ this->nextLocalIdx, this->lastProcOffset, this->opcodes.GetNumberOfElements() - this->lastProcOffset });
	this->lastProcOffset = this->opcodes.GetNumberOfElements();
}

//Private methods
void ACSB::Compiler::CompileSymbol(const IR::Symbol & symbol)
{
	switch (symbol.GetSymbolType())
	{
	case IR::SymbolType::GlobalVariable:
	{
		this->AddInstruction(Opcode::PushGlobal, this->GetGlobalIndex(symbol.GetIdentifier()));
	}
	break;
	case IR::SymbolType::LocalVariable:
	{
		this->AddInstruction(Opcode::PushLocal, this->GetLocalIndex(symbol.GetIdentifier()));
	}
	break;
	case IR::SymbolType::Null:
	{
		this->AddInstruction(Opcode::PushNull);
	}
	break;
	case IR::SymbolType::Procedure:
	{
		this->AddInstruction(Opcode::PushProc, symbol.GetNumber());
	}
	break;
	case IR::SymbolType::Temporary:
	{
		uint32 index = Unsigned<uint32>::Max();
		for (int32 idx = this->executionStack.GetNumberOfElements() - 1; idx >= 0; idx--)
		{
			if (*this->executionStack[idx] == symbol)
			{
				index = idx;
				break;
			}
		}

		this->AddInstruction(Opcode::Push, index);
	}
	break;
	case IR::SymbolType::This:
		this->opcodes.Push(Opcode::PushThis);
		break;
	default:
		NOT_IMPLEMENTED_ERROR;
	}
	this->executionStack.Push(&symbol);
}