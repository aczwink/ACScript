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
//Local
#include "../AST/visitors/StatementVisitor.hpp"
#include "../AST/visitors/ExpressionVisitor.hpp"
#include "../AST/AllNodes.hpp"
#include "../IR/Procedure.hpp"
#include "../IR/Builder.hpp"

class ASTFunction2IRTranslator : private AST::StatementVisitor, public AST::ExpressionVisitor
{
public:
	//Constructor
	inline ASTFunction2IRTranslator(IR::Builder& builder, TypeCatalog& typeCatalog, IR::Procedure& procedure)
		: builder(builder), typeCatalog(typeCatalog), procedure(procedure)
	{
	}

	//Methods
	void OnVisitedCall(const AST::CallExpression &callExpression) override;
	void OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression) override;
	void OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression) override;
	void OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression) override;
	void OnVisitingSelectExpression(const AST::SelectExpression &selectExpression) override;
	void OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression) override;

	void TranslateMain(const AST::StatementBlock &statementBlock);

private:
	//Members
	IR::Builder& builder;
	TypeCatalog& typeCatalog;
	IR::Procedure& procedure;
	DynamicArray<IR::BasicBlock*> blockStack;
	DynamicArray<IR::Value*> valueStack;
	DynamicArray<IR::Scope*> scopeStack;

	//Methods
	void Translate(const LinkedList<Tuple<UniquePointer<AST::Expression>, UniquePointer<AST::Expression>, AST::StatementBlock>>& rules);

	//Inline
	inline void AddInstruction(IR::Instruction* instruction)
	{
		this->blockStack.Last()->Add(instruction);
		this->valueStack.Push(instruction);
	}

	inline void PopBlock()
	{
		this->blockStack.Pop();
		this->scopeStack.Pop();
	}

	inline void PushBlock(IR::BasicBlock* block)
	{
		this->blockStack.Push(block);
		this->scopeStack.Push(&block->scope);
	}

	inline IR::Scope* GetCurrentScope()
	{
		return this->scopeStack.Last();
	}

	//Event handlers
	void OnVisitingExternalDeclaration(const AST::ExternalDeclarationStatement &externalDeclaration) override;
	void OnVisitingExpressionStatement(const AST::ExpressionStatement &expressionStatement) override;
	void OnVisitingObjectExpression(const AST::ObjectExpression &objectExpression) override;
	void OnVisitingReturnStatement(const AST::ReturnStatement &returnStatement) override;
	void OnVisitingVariableDefinitionStatement(const AST::VariableDefinitionStatement &variableDefinitionStatement) override;
};