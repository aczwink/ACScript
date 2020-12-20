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
//Local
#include "Module.hpp"
#include "../types/TypeCatalog.hpp"
#include "Symbol.hpp"
#include "External.hpp"
#include "Procedure.hpp"
#include "instructions/ExternalCallInstruction.hpp"
#include "instructions/CreateNewTupleInstruction.hpp"
#include "instructions/ReturnInstruction.hpp"
#include "NullValue.hpp"
#include "instructions/CallInstruction.hpp"
#include "instructions/BranchOnTrueInstruction.hpp"

namespace IR
{
	class Builder
	{
	public:
		//Constructor
		inline Builder(IR::Module& module, TypeCatalog& typeCatalog) : module(module), typeCatalog(typeCatalog)
		{
			this->null.type = typeCatalog.GetLeafType(LeafTypeEnum::Null);
		}

		//Properties
		inline IR::Module& Module()
		{
			return this->module;
		}

		//Inline
		BasicBlock *CreateBasicBlock(const String& name)
		{
			BasicBlock* basicBlock = new BasicBlock(name);
			this->basicBlocks.Push(basicBlock);
			return basicBlock;
		}

		inline Instruction* CreateCall(const Value* function, const Value* argument)
		{
			return this->InsertInstruction(new CallInstruction(function, argument));
		}

		inline Instruction* CreateConditionalBranch(Value* condition, BasicBlock* thenBlock, BasicBlock* elseBlock)
		{
			return this->InsertInstruction(new BranchOnTrueInstruction(condition, thenBlock, elseBlock));
		}

		inline Value* CreateConstant(float64 value)
		{
			Value* v = new ConstantFloat(value);
			v->type = this->typeCatalog.GetLeafType(LeafTypeEnum::Float64);

			return this->Register(v);
		}

		inline External* CreateExternal(const String& externalName, const ::Type* returnType, const ::Type* argumentType)
		{
			External* external = new External(externalName, returnType, argumentType);
			external->type = this->typeCatalog.GetFunctionType(returnType, argumentType);

			return this->Register(external);
		}

		inline Instruction* CreateExternalCall(const External* external, const Value* argument)
		{
			return this->InsertInstruction(new ExternalCallInstruction(external, argument));
		}

		inline Procedure* CreateMainProcedure()
		{
			Procedure* proc = new Procedure(this->typeCatalog.GetLeafType(LeafTypeEnum::Null), this->typeCatalog.GetEmptyTupleType(), nullptr);
			proc->type = this->typeCatalog.GetFunctionType(proc->returnType, proc->argumentType);
			proc->name = u8"main";

			return this->Register( proc );
		}

		inline Instruction *CreateNewTuple(DynamicArray<Value *>&& values)
		{
			CreateNewTupleInstruction* instruction = new CreateNewTupleInstruction(Move(values));

			DynamicArray<const ::Type*> types;
			for(const Value*const& value : instruction->Values())
				types.Push(value->type);
			instruction->type = this->typeCatalog.GetTupleType(Move(types));

			return this->InsertInstruction(instruction);
		}

		inline Procedure* CreateProcedure(const ::Type* returnType, const ::Type* argumentType)
		{
			if(argumentType == nullptr)
				argumentType = this->typeCatalog.CreateGenericType();

			Parameter* parameter = this->CreateUniqueNameAndRegisterSymbol(new Parameter, u8"$p");
			parameter->type = argumentType;

			Procedure* proc = new Procedure(returnType, argumentType, parameter);
			return this->CreateUniqueNameAndRegisterSymbol(proc, u8"__func");
		}

		inline Instruction* CreateReturn(const Value* returnValue)
		{
			return this->InsertInstruction(new ReturnInstruction(returnValue));
		}

		inline Value* GetNull()
		{
			return &this->null;
		}

	private:
		//Members
		IR::Module& module;
		TypeCatalog& typeCatalog;
		Map<String, uint32> prefixes;
		DynamicArray<UniquePointer<Value>> values;
		DynamicArray<UniquePointer<BasicBlock>> basicBlocks;
		NullValue null;

		//Inline
		template<typename T>
		inline T* CreateUniqueNameAndRegisterSymbol(T* symbol, const String& prefix)
		{
			symbol->name = this->CreateUniqueName(prefix);

			return this->Register(symbol);
		}

		inline String CreateUniqueName(const String& prefix)
		{
			uint32 counter = this->prefixes[prefix]++;
			return prefix + String::Number(counter);
		}

		inline Instruction* InsertInstruction(Instruction* instruction)
		{
			return this->CreateUniqueNameAndRegisterSymbol(instruction, u8"$t");
		}

		template<typename T>
		inline T* Register(T* value)
		{
			this->values.Push(value);
			return value;
		}
	};
}