/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#pragma once
#include <StdXX.hpp>
//Local
#include "../AST/visitors/StatementVisitor.hpp"
#include "../AST/AllNodes.hpp"
//Libs
#undef new
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

class llvmCompiler : public AST::StatementVisitor, public AST::ExpressionVisitor, public AST::TypeSpecVisitor
{
public:
	//Constructor
	inline llvmCompiler()
	{
		this->context = new llvm::LLVMContext();
		this->module = new llvm::Module(u8"acsc-llvm", *this->context);
		this->irBuilder = new llvm::IRBuilder<>(*this->context);
		this->Initialize();
	}

	//Properties
	inline llvm::Module& Module()
	{
		return *this->module;
	}

	//Methods
	void OnVisitedBlock(const AST::StatementBlock &statementBlock);
	void OnVisitedCall(const AST::CallExpression &callExpression) override;
	void OnVisitedFunctionTypeSpec(const AST::FunctionTypeSpec &functionTypeSpec) override;
	void OnVisitedIdentifierTypeSpec(const AST::IdentifierTypeSpec &identifierTypeSpec) override;
	void OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression) override;
	void OnVisitedTupleTypeSpec(const AST::TupleTypeSpec &tupleTypeSpec) override;

	void OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression) override;
	void OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression) override;
	void OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression& naturalLiteralExpression) override;

private:
	//Members
	StdXX::UniquePointer<llvm::LLVMContext> context;
	StdXX::UniquePointer<llvm::Module> module;
	StdXX::UniquePointer<llvm::IRBuilder<>> irBuilder;
	StdXX::DynamicArray<llvm::Value*> valueStack;

	//Methods
	void CompilePrintCall(const AST::CallExpression &callExpression);
	void Initialize();

	//Event handlers
	void OnVisitingExternalDeclaration(const AST::ExternalDeclarationStatement &externalDeclaration) override;

	void OnVisitingExpressionStatement(const AST::ExpressionStatement &expressionStatement) override;
};