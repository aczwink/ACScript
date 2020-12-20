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
	void OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression) override;

	void Translate(const AST::StatementBlock &statementBlock);

private:
	//Members
	IR::Builder& builder;
	TypeCatalog& typeCatalog;
	IR::Procedure& procedure;
	DynamicArray<IR::BasicBlock*> blockStack;
	DynamicArray<IR::Value*> valueStack;

	//Methods
	void Translate(const LinkedList<Tuple<UniquePointer<AST::Expression>, UniquePointer<AST::Expression>, AST::StatementBlock>>& rules);

	//Inline
	inline void AddInstruction(IR::Instruction* instruction)
	{
		this->GetCurrentBlock()->Add(instruction);
		this->valueStack.Push(instruction);
	}

	inline IR::BasicBlock* GetCurrentBlock()
	{
		return this->blockStack.Last();
	}

	//Event handlers
	void OnVisitingExternalDeclaration(const AST::ExternalDeclarationStatement &externalDeclaration) override;
	void OnVisitingExpressionStatement(const AST::ExpressionStatement &expressionStatement) override;
	void OnVisitingReturnStatement(const AST::ReturnStatement &returnStatement) override;
	void OnVisitingVariableDefinitionStatement(const AST::VariableDefinitionStatement &variableDefinitionStatement) override;
};