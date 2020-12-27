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
#include "PatternMatching2IRTranslator.hpp"

//Event handlers
void PatternMatching2IRTranslator::OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression)
{
	IR::Value* argument = this->valueStack.Pop();

	const IR::External* lessThanExternal = this->builder.Module().FindExternal(u8"<");
	const IR::External* orExternal = this->builder.Module().FindExternal(u8"or");
	const IR::External* notExternal = this->builder.Module().FindExternal(u8"not");

	DynamicArray<IR::Value*> values;
	values.Push(this->builder.CreateConstant(naturalLiteralExpression.Value()));
	values.Push(argument);
	IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
	this->AddInstruction(argInstruction);

	IR::Instruction* cmpInstruction1 = this->builder.CreateExternalCall(lessThanExternal, argInstruction);
	this->AddInstruction(cmpInstruction1);

	values.Push(argument);
	values.Push(this->builder.CreateConstant(naturalLiteralExpression.Value()));
	argInstruction = this->builder.CreateNewTuple(Move(values));
	this->AddInstruction(argInstruction);

	IR::Instruction* cmpInstruction2 = this->builder.CreateExternalCall(lessThanExternal, argInstruction);
	this->AddInstruction(cmpInstruction2);

	values.Push(cmpInstruction1);
	values.Push(cmpInstruction2);
	argInstruction = this->builder.CreateNewTuple(Move(values));
	this->AddInstruction(argInstruction);

	IR::Instruction* andInstruction = this->builder.CreateExternalCall(orExternal, argInstruction);
	this->AddInstruction(andInstruction);

	IR::Instruction* notInstruction = this->builder.CreateExternalCall(notExternal, andInstruction);
	this->AddInstruction(notInstruction);

	this->valueStack.Push(notInstruction);
}

void PatternMatching2IRTranslator::OnVisitingObjectExpression(const AST::ObjectExpression &objectExpression)
{
	const IR::External* subscript = this->builder.Module().FindExternal(u8"[]");

	IR::Value* dictValue = this->valueStack.Pop();

	for(const auto& kv : objectExpression.Members())
	{
		ASSERT_EQUALS(true, kv.value.IsNull()); //TODO: implement value conditions for objects

		DynamicArray<IR::Value*> values;
		values.Push(dictValue);
		values.Push(this->builder.CreateConstant(kv.key));
		IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
		this->AddInstruction(argInstruction);

		IR::Instruction* selectInstruction = this->builder.CreateExternalCall(subscript, argInstruction);
		this->AddInstruction(selectInstruction);

		this->valueStack.Push(selectInstruction);
		this->basicBlock->namedValues[kv.key] = selectInstruction;
	}
}

void PatternMatching2IRTranslator::OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression)
{
	const IR::External* subscript = this->builder.Module().FindExternal(u8"[]");

	IR::Value* tupleValue = this->valueStack.Pop();

	for(uint32 i = 0; i < tupleExpression.Expressions().GetNumberOfElements(); i++)
	{
		const auto& expression = tupleExpression.Expressions()[i];

		DynamicArray<IR::Value*> values;
		values.Push(tupleValue);
		values.Push(this->builder.CreateConstant(i));
		IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
		this->AddInstruction(argInstruction);

		IR::Instruction* selectInstruction = this->builder.CreateExternalCall(subscript, argInstruction);
		this->AddInstruction(selectInstruction);

		this->valueStack.Push(selectInstruction);
		expression->Visit(*this);
	}
}