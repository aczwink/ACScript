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
#include "AST/ASTVisitor.hpp"
#include "IR/Builder.hpp"

class AST2IRTranslator : public ASTVisitor
{
public:
	//Constructor
	inline AST2IRTranslator(IR::Builder& builder) : builder(builder)
	{
		IR::Procedure* mainProc = builder.CreateMainProcedure();
		builder.Module().AddProcedure(mainProc);
		this->proceduresStack.Push(mainProc);

		IR::BasicBlock* basicBlock = builder.CreateBasicBlock();
		mainProc->entryBlock = basicBlock;
		this->blockStack.Push(basicBlock);
	}

	//Methods
	void OnVisitedBlock(const StatementBlock &statementBlock) override;
	void OnVisitedCall(const CallExpression &callExpression) override;
	void OnVisitedTupleExpression(const TupleExpression &tupleExpression) override;

	void OnVisitingNaturalLiteral(const NaturalLiteralExpression &naturalLiteralExpression) override;

private:
	//Members
	IR::Builder& builder;
	DynamicArray<IR::Value*> valueStack;
	DynamicArray<IR::Procedure*> proceduresStack;
	DynamicArray<IR::BasicBlock*> blockStack;

	//Inline
	inline void AddInstruction(IR::Instruction* instruction)
	{
		this->blockStack.Last()->Add(instruction);
		this->valueStack.Push(instruction);
	}
};