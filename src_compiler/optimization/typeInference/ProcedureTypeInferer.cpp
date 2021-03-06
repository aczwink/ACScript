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
#include "ProcedureTypeInferer.hpp"
//Local
#include "../../IR/visitors/AllSymbols.hpp"
#include "ProcedureTypeReplacer.hpp"
#include "../BackwardsEvaluator.hpp"
//Namespaces
using namespace Optimization;

//Private methods
void ProcedureTypeInferer::InferTypeMustHaveMember(const ::Type* type, const String &memberName) const
{
	const ObjectType* objectType = dynamic_cast<const ObjectType *>(type);
	if(objectType)
	{
		ASSERT_EQUALS(true, objectType->Members().Contains(memberName));
		return;
	}

	GenericType* leftGenericType = const_cast<GenericType *>(dynamic_cast<const GenericType *>(type));
	if(leftGenericType)
	{
		leftGenericType->AddMemberConstraint(memberName, this->typeCatalog.CreateGenericType());
		return;
	}

	NOT_IMPLEMENTED_ERROR;
}

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
		BackwardsEvaluator valueEvaluator;
		const IR::Procedure* procedure = dynamic_cast<const IR::Procedure *>(valueEvaluator.Evaluate(callInstruction.function));
		callInstruction.type = procedure->returnType;
	}
}

void ProcedureTypeInferer::OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction)
{
}

void ProcedureTypeInferer::OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction)
{
	this->InferTypesMustBeEqual(externalCallInstruction.argument->type, externalCallInstruction.external->argumentType);
}

void ProcedureTypeInferer::OnVisitingNewObjectInstruction(IR::CreateNewObjectInstruction &createNewObjectInstruction)
{
	createNewObjectInstruction.UpdateType(this->typeCatalog);
}

void ProcedureTypeInferer::OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction)
{
	DynamicArray<const ::Type*> types;
	for(const IR::Value*const& value : createNewTupleInstruction.Values())
		types.Push(value->type);
	createNewTupleInstruction.type = this->typeCatalog.GetTupleType(Move(types));
}

void ProcedureTypeInferer::OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction)
{
	returnInstruction.type = returnInstruction.returnValue->type;

	if(this->procedure.returnType == nullptr)
	{
		this->procedure.returnType = returnInstruction.type;
		this->procedure.type = nullptr;
	}
	else if(returnInstruction.type != nullptr)
	{
		this->InferTypesMustBeEqual(this->procedure.returnType, returnInstruction.type);
		this->procedure.type = nullptr;
	}
	else
	{
		//returnInstruction.type should be nullptr only if we come from recursion
	}

	if(this->procedure.type == nullptr)
	{
		if( this->procedure.argumentType && this->procedure.returnType)
		{
			this->procedure.type = this->typeCatalog.GetFunctionType(this->procedure.returnType, this->procedure.argumentType);
		}
	}
}

void ProcedureTypeInferer::OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction)
{
	const IR::ConstantString* constantString = dynamic_cast<const IR::ConstantString *>(selectInstruction.selector);
	this->InferTypeMustHaveMember(selectInstruction.inner->type, constantString->Value());

	const ObjectType* objectType = dynamic_cast<const ObjectType *>(selectInstruction.inner->type);
	if(objectType)
		selectInstruction.type = objectType->Members()[constantString->Value()];
	else
	{
		const GenericType* genericType = dynamic_cast<const GenericType *>(selectInstruction.inner->type);
		selectInstruction.type = genericType->GetMemberConstraint(constantString->Value());
	}
}

void ProcedureTypeInferer::OnVisitingStoreInstruction(IR::StoreInstruction &storeInstruction)
{
}
