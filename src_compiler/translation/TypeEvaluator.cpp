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
//Class header
#include "TypeEvaluator.hpp"
//Local
#include "../AST/AllNodes.hpp"
//Namespaces
using namespace Translation;

//Public methods
const ::Type *Translation::TypeEvaluator::Evaluate(const AST::TypeSpec &typeSpec)
{
	typeSpec.Visit(*this);

	return this->typeStack.Pop();
}

//Private methods
void Translation::TypeEvaluator::OnVisitedFunctionTypeSpec(const AST::FunctionTypeSpec &functionTypeSpec)
{
	functionTypeSpec.ArgTypeSpec().Visit(*this);
	const ::Type* argType = this->typeStack.Pop();

	functionTypeSpec.ResultTypeSpec().Visit(*this);
	const ::Type* resultType = this->typeStack.Pop();

	this->typeStack.Push(this->typeCatalog.GetFunctionType(resultType, argType));
}

void Translation::TypeEvaluator::OnVisitedIdentifierTypeSpec(const AST::IdentifierTypeSpec &identifierTypeSpec)
{
	if(identifierTypeSpec.Identififer() == u8"any")
		this->typeStack.Push(this->typeCatalog.GetLeafType(LeafTypeEnum::Any));
	else if(identifierTypeSpec.Identififer() == u8"bool")
		this->typeStack.Push(this->typeCatalog.GetLeafType(LeafTypeEnum::Bool));
	else if(identifierTypeSpec.Identififer() == u8"float64")
		this->typeStack.Push(this->typeCatalog.GetLeafType(LeafTypeEnum::Float64));
	else if(identifierTypeSpec.Identififer() == u8"null")
		this->typeStack.Push(this->typeCatalog.GetLeafType(LeafTypeEnum::Null));
	else if(identifierTypeSpec.Identififer() == u8"string")
		this->typeStack.Push(this->typeCatalog.GetLeafType(LeafTypeEnum::String));
	else
		NOT_IMPLEMENTED_ERROR;
}

void Translation::TypeEvaluator::OnVisitedTupleTypeSpec(const AST::TupleTypeSpec &tupleTypeSpec)
{
	DynamicArray<const ::Type*> entries;
	for(const auto& entry : tupleTypeSpec.Entries())
	{
		entry->Visit(*this);
		entries.Push(this->typeStack.Pop());
	}

	this->typeStack.Push(this->typeCatalog.GetTupleType(Move(entries), tupleTypeSpec.RepeatLastEntry()));
}
