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
#include "ASTFunction2IRTranslator.hpp"
//Local
#include "TypeEvaluator.hpp"
#include "../CompileException.hpp"
#include "PatternMatching2IRTranslator.hpp"

//Public methods
void ASTFunction2IRTranslator::OnVisitedCall(const AST::CallExpression &callExpression)
{
	callExpression.Argument().Visit(*this);
	IR::Value* argument = this->valueStack.Pop();

	if(this->GetCurrentBlock()->namedValues.Contains(callExpression.FunctionName()))
	{
		IR::Instruction* instruction = this->builder.CreateCall(this->GetCurrentBlock()->namedValues[callExpression.FunctionName()], argument);
		this->AddInstruction(instruction);
		return;
	}

	const IR::External* external = this->builder.Module().FindExternal(callExpression.FunctionName());
	if(external)
	{
		IR::Instruction* instruction = this->builder.CreateExternalCall(external, argument);
		this->AddInstruction(instruction);
		return;
	}

	throw CompileException(u8"Can't resolve function referred to by: " + callExpression.FunctionName());
}

void ASTFunction2IRTranslator::OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression)
{
	IR::Procedure* proc = this->builder.CreateProcedure(nullptr, nullptr);
	this->builder.Module().AddProcedure(proc, &this->procedure);

	IR::BasicBlock* basicBlock = this->builder.CreateBasicBlock(u8"entry");
	if(this->procedure.name == u8"main")
	{
		//the function to compile is defined within the module, derive variables
		basicBlock->namedValues = this->GetCurrentBlock()->namedValues;
	}
	proc->AddBlock(basicBlock);

	ASTFunction2IRTranslator functionTranslator(this->builder, this->typeCatalog, *proc);
	functionTranslator.Translate(functionExpression.Rules());

	this->valueStack.Push(proc);
}

void ASTFunction2IRTranslator::OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression)
{
	if(this->GetCurrentBlock()->namedValues.Contains(identifierExpression.Identifier()))
	{
		this->valueStack.Push(this->GetCurrentBlock()->namedValues[identifierExpression.Identifier()]);
		return;
	}

	throw CompileException(u8"Can't resolve identifier: " + identifierExpression.Identifier());
}

void ASTFunction2IRTranslator::OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression)
{
	IR::Value* value = this->builder.CreateConstant(naturalLiteralExpression.Value());
	this->valueStack.Push(value);
}

void ASTFunction2IRTranslator::OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression)
{
	DynamicArray<IR::Value*> values;
	for(const UniquePointer<AST::Expression>& expr : tupleExpression.Expressions())
	{
		expr->Visit(*this);
		values.Push(this->valueStack.Pop());
	}

	IR::Instruction* instruction = this->builder.CreateNewTuple(Move(values));
	this->AddInstruction(instruction);
}

void ASTFunction2IRTranslator::TranslateMain(const AST::StatementBlock &statementBlock)
{
	this->blockStack.Push(this->procedure.EntryBlock());
	statementBlock.Visit(*this);
	this->AddInstruction(this->builder.CreateReturn(this->builder.GetNull()));
}

//Private methods
void ASTFunction2IRTranslator::Translate(const LinkedList<Tuple<UniquePointer<AST::Expression>, UniquePointer<AST::Expression>, AST::StatementBlock>> &rules)
{
	uint8 ruleNumber = 0;

	IR::BasicBlock* lastBlock = this->procedure.EntryBlock();
	for(const auto& rule : rules)
	{
		PatternMatching2IRTranslator patternMatchingTranslator(this->builder, this->procedure, lastBlock);
		IR::Value* condition = patternMatchingTranslator.Translate(*rule.Get<0>());

		if(!rule.Get<1>().IsNull())
		{
			this->blockStack.Push(lastBlock);
			rule.Get<1>()->Visit(*this);
			this->blockStack.Pop();

			IR::Value* guardCondition = this->valueStack.Pop();
			if(condition)
			{
				this->blockStack.Push(lastBlock);

				const IR::External* andExternal = this->builder.Module().FindExternal(u8"and");

				DynamicArray<IR::Value*> values;
				values.Push(condition);
				values.Push(guardCondition);
				IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
				this->AddInstruction(argInstruction);

				IR::Instruction* andInstruction = this->builder.CreateExternalCall(andExternal, argInstruction);
				this->AddInstruction(andInstruction);

				this->blockStack.Pop();

				condition = andInstruction;
			}
			else
				condition = guardCondition;
		}

		if(condition)
		{
			IR::BasicBlock *thenBlock = this->builder.CreateBasicBlock(u8"rule" + String::Number(ruleNumber++));
			IR::BasicBlock *elseBlock = this->builder.CreateBasicBlock(u8"rule" + String::Number(ruleNumber++));

			thenBlock->namedValues = lastBlock->namedValues;
			elseBlock->namedValues = lastBlock->namedValues;

			this->blockStack.Push(lastBlock);

			IR::Instruction* branchInstruction = this->builder.CreateConditionalBranch(condition, thenBlock, elseBlock);
			this->AddInstruction(branchInstruction);

			this->procedure.AddBlock(thenBlock);
			this->procedure.AddBlock(elseBlock);
			this->blockStack.Push(thenBlock);

			lastBlock = elseBlock;
		}
		else
			this->blockStack.Push(lastBlock);

		rule.Get<2>().Visit(*this);
	}
}

//Event handlers
void ASTFunction2IRTranslator::OnVisitingExpressionStatement(const AST::ExpressionStatement &expressionStatement)
{
	expressionStatement.EmbeddedExpression().Visit(*this);
}

void ASTFunction2IRTranslator::OnVisitingExternalDeclaration(const AST::ExternalDeclarationStatement &externalDeclaration)
{
	Translation::TypeEvaluator typeEvaluator(this->typeCatalog);
	const ::FunctionType* functionType = dynamic_cast<const FunctionType *>(typeEvaluator.Evaluate(externalDeclaration.TypeSpec()));

	IR::External* external = builder.CreateExternal(externalDeclaration.Name(), functionType->ReturnType(), functionType->ArgumentType());
	builder.Module().AddExternal(external);
}

void ASTFunction2IRTranslator::OnVisitingObjectExpression(const AST::ObjectExpression &objectExpression)
{
	Map<String, IR::Value*> values;
	for(const auto& kv : objectExpression.Members())
	{
		kv.value->Visit(*this);
		values.Insert(kv.key, this->valueStack.Pop());
	}

	IR::Instruction* instruction = this->builder.CreateNewObject(Move(values));
	this->AddInstruction(instruction);
}

void ASTFunction2IRTranslator::OnVisitingReturnStatement(const AST::ReturnStatement &returnStatement)
{
	returnStatement.EmbeddedExpression().Visit(*this);

	IR::Instruction* instruction = this->builder.CreateReturn(this->valueStack.Pop());
	this->AddInstruction(instruction);
}

void ASTFunction2IRTranslator::OnVisitingVariableDefinitionStatement(const AST::VariableDefinitionStatement &variableDefinitionStatement)
{
	variableDefinitionStatement.RightHandSide().Visit(*this);

	String varName = dynamic_cast<const AST::IdentifierLeftValue&>(variableDefinitionStatement.LeftHandSide()).Identifier();
	this->GetCurrentBlock()->namedValues[varName] = this->valueStack.Pop();
}