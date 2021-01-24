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
#include "ASTFunction2IRTranslator.hpp"
//Local
#include "TypeEvaluator.hpp"
#include "../CompileException.hpp"
#include "PatternMatching2IRTranslator.hpp"

//Public methods
void ASTFunction2IRTranslator::OnVisitedCall(const AST::CallExpression &callExpression)
{
	IR::Value* argument = this->builder.GetNull();
	if(callExpression.Argument())
	{
		callExpression.Argument()->Visit(*this);
		argument = this->valueStack.Pop();
	}

	callExpression.Called().Visit(*this);
	IR::Value* called = this->valueStack.Pop();

	IR::Instruction* instruction;
	IR::External* external = dynamic_cast<IR::External *>(called);
	if(external)
		instruction = this->builder.CreateExternalCall(external, argument);
	else
		instruction = this->builder.CreateCall(called, argument);

	this->AddInstruction(instruction);
}

void ASTFunction2IRTranslator::OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression)
{
	IR::Procedure* proc = this->builder.CreateProcedure(nullptr, nullptr);
	this->builder.Module().AddProcedure(proc, &this->procedure);

	IR::BasicBlock* basicBlock = this->builder.CreateBasicBlock(u8"entry");
	basicBlock->scope.parent = this->GetCurrentScope();
	proc->AddBlock(basicBlock);

	ASTFunction2IRTranslator functionTranslator(this->builder, this->typeCatalog, *proc);
	functionTranslator.Translate(functionExpression.Rules());

	this->valueStack.Push(proc);
}

void ASTFunction2IRTranslator::OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression)
{
	if(identifierExpression.Identifier() == u8"null")
	{
		this->valueStack.Push(this->builder.GetNull());
	}
	else if(this->GetCurrentScope()->IsInScope(identifierExpression.Identifier(), &this->procedure.EntryBlock()->scope))
	{
		this->valueStack.Push(this->GetCurrentScope()->Resolve(identifierExpression.Identifier()));
	}
	else if(this->GetCurrentScope()->Resolve(identifierExpression.Identifier()))
	{
		IR::Value* value = this->procedure.Environment().CaptureByReference(
				this->GetCurrentScope()->Resolve(identifierExpression.Identifier()), this->builder);
		this->valueStack.Push(value);
	}
	else
	{
		IR::External* external = const_cast<IR::External *>(this->builder.Module().FindExternal(
				identifierExpression.Identifier()));

		if(external)
			this->valueStack.Push(external);
		else
			throw CompileException(u8"Can't resolve identifier: " + identifierExpression.Identifier());
	}
}

void ASTFunction2IRTranslator::OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression)
{
	IR::Value* value = this->builder.CreateConstant(naturalLiteralExpression.Value());
	this->valueStack.Push(value);
}

void ASTFunction2IRTranslator::OnVisitingSelectExpression(const AST::SelectExpression &selectExpression)
{
	selectExpression.Expression().Visit(*this);
	IR::Value* innerValue = this->valueStack.Pop();

	IR::Value* value = this->builder.CreateConstant(selectExpression.MemberName());

	IR::Instruction* instruction = this->builder.CreateSelectInstruction(innerValue, value);
	this->AddInstruction(instruction);
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
	this->PushBlock(this->procedure.EntryBlock());
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
			this->PushBlock(lastBlock);
			rule.Get<1>()->Visit(*this);
			this->PopBlock();

			IR::Value* guardCondition = this->valueStack.Pop();
			if(condition)
			{
				this->PushBlock(lastBlock);

				IR::External* andExternal = this->builder.Module().FindExternal(u8"and");

				DynamicArray<IR::Value*> values;
				values.Push(condition);
				values.Push(guardCondition);
				IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
				this->AddInstruction(argInstruction);

				IR::Instruction* andInstruction = this->builder.CreateExternalCall(andExternal, argInstruction);
				this->AddInstruction(andInstruction);

				this->PopBlock();

				condition = andInstruction;
			}
			else
				condition = guardCondition;
		}

		if(condition)
		{
			IR::BasicBlock *thenBlock = this->builder.CreateBasicBlock(u8"rule" + String::Number(ruleNumber++));
			IR::BasicBlock *elseBlock = this->builder.CreateBasicBlock(u8"rule" + String::Number(ruleNumber++));

			thenBlock->scope = lastBlock->scope;
			elseBlock->scope = lastBlock->scope;

			this->PushBlock(lastBlock);

			IR::Instruction* branchInstruction = this->builder.CreateConditionalBranch(condition, thenBlock, elseBlock);
			this->AddInstruction(branchInstruction);

			this->procedure.AddBlock(thenBlock);
			this->procedure.AddBlock(elseBlock);
			this->PushBlock(thenBlock);

			lastBlock = elseBlock;
		}
		else
			this->PushBlock(lastBlock);

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
	IR::Scope objectScope;
	objectScope.parent = this->scopeStack.Last();
	this->scopeStack.Push(&objectScope);

	IR::CreateNewObjectInstruction* instruction = this->builder.CreateNewObject();
	instruction->UpdateType(this->typeCatalog);
	this->AddInstruction(instruction);
	objectScope.Add(u8"this", instruction);

	IR::ObjectValue* lastObjectInstruction = instruction;
	for(const auto& kv : objectExpression.Members())
	{
		kv.value->Visit(*this);
		IR::Value* value = this->valueStack.Pop();

		IR::Value* key = this->builder.CreateConstant(kv.key);

		IR::StoreInstruction* store = this->builder.CreateStoreInstruction(lastObjectInstruction, key, value);
		this->AddInstruction(store);

		lastObjectInstruction = store;
		lastObjectInstruction->UpdateType(this->typeCatalog);
	}

	this->scopeStack.Pop();
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
	this->GetCurrentScope()->Add(varName, this->valueStack.Pop());
}