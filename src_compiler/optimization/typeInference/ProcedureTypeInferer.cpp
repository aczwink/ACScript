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
#include "ProcedureTypeInferer.hpp"
//Local
#include "../../IR/visitors/AllSymbols.hpp"
#include "ProcedureTypeReplacer.hpp"
//Namespaces
using namespace Optimization;

//Private methods
void ProcedureTypeInferer::InferTypesMustBeEqual(const ::Type *type, const ::Type *mustBeType) const
{
	if(type->IsTriviallyAssignableTo(*mustBeType))
		return;

	const TupleType* leftTupleType = dynamic_cast<const TupleType *>(type);
	const TupleType* rightTupleType = dynamic_cast<const TupleType *>(mustBeType);

	if(leftTupleType && rightTupleType)
	{
		ASSERT_EQUALS(leftTupleType->Types().GetNumberOfElements(), rightTupleType->Types().GetNumberOfElements());
		for(uint32 i = 0; i < leftTupleType->Types().GetNumberOfElements(); i++)
			this->InferTypesMustBeEqual(leftTupleType->Types()[i], rightTupleType->Types()[i]);
	}

	const GenericType* leftGenericType = dynamic_cast<const GenericType *>(type);
	if(leftGenericType)
	{
		this->ReplaceType(type, mustBeType);
	}
}

void ProcedureTypeInferer::ReplaceType(const ::Type *type, const ::Type *mustBeType) const
{
	ProcedureTypeReplacer typeReplacer(this->procedure, this->typeCatalog, type, mustBeType);
	typeReplacer.Transform();
}

//Event handlers
void ProcedureTypeInferer::OnVisitingCallInstruction(IR::CallInstruction &callInstruction)
{
	if(callInstruction.type == nullptr)
	{
		const IR::Procedure* procedure = dynamic_cast<const IR::Procedure *>(callInstruction.function);
		callInstruction.type = procedure->returnType;
	}
}

void ProcedureTypeInferer::OnVisitingConditionalBranchInstruction(const IR::BranchOnTrueInstruction &branchOnTrueInstruction)
{
}

void ProcedureTypeInferer::OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction)
{
	this->InferTypesMustBeEqual(externalCallInstruction.argument->type, externalCallInstruction.external->argumentType);
}

void ProcedureTypeInferer::OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction)
{
	DynamicArray<const ::Type*> types;
	for(const IR::Value*const& value : createNewTupleInstruction.Values())
		types.Push(value->type);
	createNewTupleInstruction.type = this->typeCatalog.GetTupleType(Move(types));
}

void ProcedureTypeInferer::OnVisitingReturnInstruction(const IR::ReturnInstruction &returnInstruction)
{
	if(this->procedure.returnType == nullptr)
		this->procedure.returnType = returnInstruction.type;
	else
		this->InferTypesMustBeEqual(this->procedure.returnType, returnInstruction.type);
	this->procedure.type = nullptr;

	if(this->procedure.type == nullptr)
	{
		if( this->procedure.argumentType && this->procedure.returnType)
		{
			this->procedure.type = this->typeCatalog.GetFunctionType(this->procedure.returnType, this->procedure.argumentType);
		}
	}
}