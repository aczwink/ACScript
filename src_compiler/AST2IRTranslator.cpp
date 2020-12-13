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
#include "AST2IRTranslator.hpp"
//Local
#include "./AST/AllNodes.hpp"
#include "CompileException.hpp"

//Public methods
void AST2IRTranslator::OnVisitedBlock(const StatementBlock &statementBlock)
{
	this->AddInstruction(this->builder.CreateReturn(this->builder.GetNull()));
}

void AST2IRTranslator::OnVisitedCall(const CallExpression &callExpression)
{
	const IR::External* external = this->builder.Module().FindExternal(callExpression.FunctionName());
	if(external)
	{
		IR::Instruction* instruction = this->builder.CreateExternalCall(external, this->valueStack.Pop());
		this->AddInstruction(instruction);
		return;
	}

	throw CompileException(u8"Can't resolve function referred to by: " + callExpression.FunctionName());
}

void AST2IRTranslator::OnVisitedTupleExpression(const TupleExpression &tupleExpression)
{
	DynamicArray<IR::Value*> values;
	for(uint32 i = 0; i < tupleExpression.Expressions().GetNumberOfElements(); i++)
		values.Push(this->valueStack.Pop());

	IR::Instruction* instruction = this->builder.CreateNewTuple(Move(values));
	this->AddInstruction(instruction);
}

void AST2IRTranslator::OnVisitingNaturalLiteral(const NaturalLiteralExpression &naturalLiteralExpression)
{
	IR::Value* value = this->builder.CreateConstant(naturalLiteralExpression.Value());
	this->valueStack.Push(value);
}