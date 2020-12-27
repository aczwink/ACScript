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
//Local
#include "../AST/expressions/Expression.hpp"
#include "../IR/Builder.hpp"
#include "../AST/AllNodes.hpp"

class PatternMatching2IRTranslator : private AST::ExpressionVisitor
{
public:
	//Constructor
	inline PatternMatching2IRTranslator(IR::Builder& builder, IR::Procedure& procedure, IR::BasicBlock* basicBlock)
		: builder(builder), procedure(procedure), basicBlock(basicBlock)
	{
		this->valueStack.Push(procedure.parameter);
	}

	//Inline
	inline IR::Value* Translate(const AST::Expression& expression)
	{
		expression.Visit(*this);
		if(this->valueStack.IsEmpty())
			return nullptr;
		return this->valueStack.Pop();
	}

private:
	//Members
	IR::Builder& builder;
	IR::Procedure& procedure;
	IR::BasicBlock* basicBlock;
	DynamicArray<IR::Value*> valueStack;

	//Event handlers
	void OnVisitedCall(const AST::CallExpression &callExpression) override
	{
		NOT_IMPLEMENTED_ERROR;
	}

	void OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression) override
	{
		NOT_IMPLEMENTED_ERROR;
	}

	void OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression) override
	{
		String varName = identifierExpression.Identifier();
		this->basicBlock->namedValues[varName] = this->valueStack.Pop();
	}

	void OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression) override;
	void OnVisitingObjectExpression(const AST::ObjectExpression &objectExpression) override;
	void OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression) override;

	//Inline
	inline void AddInstruction(IR::Instruction* instruction)
	{
		this->basicBlock->Add(instruction);
	}
};