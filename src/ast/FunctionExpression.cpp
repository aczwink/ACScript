/*
* Copyright (c) 2018 Amir Czwink (amir130@hotmail.de)
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
#include "FunctionExpression.hpp"
//Local
#include "../ir/Program.hpp"
#include "ReturnExpression.hpp"
#include "../../src_compiler/AST/statements/StatementBlock.hpp"

//Public methods
IR::Symbol FunctionExpression::Compile(IR::Block& block, const ExpressionCompileFlags& flags) const
{
	IR::Procedure& proc = block.GetProgram().AddProcedure(IR::LeafType::Any);
	proc.GetBlock().SetParent(&block);

	//function prolog
	for (const FuncMap& mapping : this->mappings)
	{
		if (mapping.expr)
		{
			NOT_IMPLEMENTED_ERROR;
		}
		else
		{
			mapping.statements->Compile(proc.GetBlock());
		}
	}

	return block.GetProgram().GetProcedureSymbol(proc);
	/*
	uint32 funcOffset = p.GetCurrentOffset();

	for (const FuncMap& mapping : this->mappings)
	{
		uint32 jumpOffsetAddr1, jumpOffsetAddr2 = 0; //if they are 0 means not set as they really can't be zero

		//compile function prolog
		if (mapping.pattern)
		{
			mapping.pattern->CompileAsUnpack(p, true);
			p.AddInstruction(Opcode::JumpOnFalse);
			jumpOffsetAddr1 = p.GetCurrentOffset();
			p.AddInt8(0);
		}
		if (mapping.condition)
		{
			mapping.condition->Compile(p);
			p.AddInstruction(Opcode::JumpOnFalse);
			jumpOffsetAddr2 = p.GetCurrentOffset();
			p.AddInt8(0);
		}

		p.AddInstruction(Opcode::Pop); //pop the function argument... it is not needed anymore

		if (mapping.expr)
		{
			//an expression is returned
			mapping.expr->Compile(p);
			if (!IS_INSTANCE_OF(mapping.expr, ReturnExpression))
				p.AddInstruction(Opcode::Return, 1); //we do not need to write a return but infer it
		}

		//jump after code in case we are in the wrong pattern
		p.WriteInt8(jumpOffsetAddr1, p.GetCurrentOffset() - jumpOffsetAddr1 - 1);
		if(jumpOffsetAddr2)
			p.WriteInt8(jumpOffsetAddr2, p.GetCurrentOffset() - jumpOffsetAddr2 - 1);
	}

	p.WriteInt8(offset, p.GetCurrentOffset() - offset - 1); //skip function code offset

	ScriptFunction* function = new ScriptFunction(funcOffset, p.GetCurrentOffset() - funcOffset);

	RuntimeObject functionObj(function);
	p.AddLoadConstant(functionObj);
	*/
	return IR::Symbol::Temporary(0);
}

void FunctionExpression::CompileAsUnpack(Program & p, bool localAssign) const
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

IR::Type FunctionExpression::GetType() const
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return IR::Type(IR::LeafType::Any);
}
