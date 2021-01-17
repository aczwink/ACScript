/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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
#include "llvmCompiler.hpp"
//Local
#include "../CompileException.hpp"

//Public methods
void llvmCompiler::OnVisitedBlock(const AST::StatementBlock &statementBlock)
{
	//return value for main
	this->irBuilder->CreateRet(llvm::ConstantInt::get(*this->context, llvm::APInt(32, 0)));
}

void llvmCompiler::OnVisitedCall(const AST::CallExpression &callExpression)
{
	NOT_IMPLEMENTED_ERROR;
	/*
	llvm::Function* function = this->module->getFunction(
			reinterpret_cast<const char *>(callExpression.FunctionName().ToUTF8().GetRawZeroTerminatedData()));

	if (!function)
	{
		if(callExpression.FunctionName() == u8"print")
		{
			this->CompilePrintCall(callExpression);
			return;
		}
		throw CompileException(u8"Unknown function: " + callExpression.FunctionName());
	}

	//if (function->arg_size() != callExpression.Argument().Expressions().GetNumberOfElements())
		throw CompileException(u8"Incorrect number of arguments passed");

	std::vector<llvm::Value *> ArgsV;
	//for(const auto& expr : callExpression.Argument().Expressions())
		ArgsV.push_back(this->valueStack.Pop());

	llvm::Value* call = this->irBuilder->CreateCall(function, ArgsV, u8"calltmp");
	this->valueStack.Push(call);*/
}

void llvmCompiler::OnVisitingExternalDeclaration(const AST::ExternalDeclarationStatement &externalDeclaration)
{
}

void llvmCompiler::OnVisitedFunctionTypeSpec(const AST::FunctionTypeSpec &functionTypeSpec)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitedIdentifierTypeSpec(const AST::IdentifierTypeSpec &identifierTypeSpec)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitedTupleExpression(const AST::TupleExpression &tupleExpression)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitedTupleTypeSpec(const AST::TupleTypeSpec &tupleTypeSpec)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitingFunctionExpression(const AST::FunctionExpression &functionExpression)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitingIdentifier(const AST::IdentifierExpression &identifierExpression)
{
	//NOT IMPLEMENTED
}

void llvmCompiler::OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression)
{
	llvm::Value* value = llvm::ConstantFP::get(*this->context, llvm::APFloat((float64)naturalLiteralExpression.Value()));
	this->valueStack.Push(value);
}

//Private methods
void llvmCompiler::CompilePrintCall(const AST::CallExpression &callExpression)
{
	std::vector<llvm::Value *> ArgsV;
	llvm::Value* formatStr = this->irBuilder->CreateGlobalStringPtr(u8"%f\n");
	ArgsV.push_back(formatStr);

	//for(uint32 i = 0; i < callExpression.Argument().Expressions().GetNumberOfElements(); i++)
	NOT_IMPLEMENTED_ERROR;
	{
		ArgsV.push_back(this->valueStack.Pop());
	}

	llvm::Value* call = this->irBuilder->CreateCall(this->module->getFunction(u8"printf"), ArgsV, "printfCall");

	this->valueStack.Push(call);
}

void llvmCompiler::Initialize()
{
	//declare printf
	llvm::FunctionType* printfType = llvm::FunctionType::get(this->irBuilder->getInt32Ty(), llvm::Type::getInt8PtrTy(*this->context), true);
	llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, u8"printf", *this->module);

	//define main
	llvm::FunctionType *mainType = llvm::FunctionType::get(this->irBuilder->getInt32Ty(), false);
	llvm::Function *main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, u8"main", *this->module);

	llvm::BasicBlock *entry = llvm::BasicBlock::Create(*this->context, u8"entry", main);
	this->irBuilder->SetInsertPoint(entry);
}

//Event handlers
void llvmCompiler::OnVisitingExpressionStatement(const AST::ExpressionStatement &expressionStatement)
{

}
