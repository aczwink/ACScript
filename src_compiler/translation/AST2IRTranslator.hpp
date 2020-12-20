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
#include "../IR/Builder.hpp"
#include "../AST/statements/StatementBlock.hpp"

class AST2IRTranslator
{
public:
	//Constructor
	inline AST2IRTranslator(IR::Builder& builder, TypeCatalog& typeCatalog) : builder(builder), typeCatalog(typeCatalog)
	{
	}

	//Methods
	//void OnVisitedBlock(const AST::StatementBlock &statementBlock) override;
	//void OnVisitedFunctionTypeSpec(const AST::FunctionTypeSpec &functionTypeSpec) override;
	//void OnVisitedIdentifierTypeSpec(const AST::IdentifierTypeSpec &identifierTypeSpec) override;
	//void OnVisitedTupleTypeSpec(const AST::TupleTypeSpec &tupleTypeSpec) override;
	//void OnVisitingNaturalLiteral(const AST::NaturalLiteralExpression &naturalLiteralExpression) override;

	void Translate(const AST::StatementBlock& statementBlock);

private:
	//Members
	IR::Builder& builder;
	TypeCatalog& typeCatalog;
	DynamicArray<IR::Procedure*> proceduresStack;
};