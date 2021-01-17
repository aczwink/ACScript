/*
* Copyright (c) 2019-2021 Amir Czwink (amir130@hotmail.de)
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
#include <acsb/Opcode.hpp>
#include "../IR/visitors/BasicBlockVisitor.hpp"
#include "../IR/External.hpp"
#include "../IR/Module.hpp"
#include "../optimization/DependencyGraph.hpp"

namespace ACSB
{
	class Compiler : private IR::BasicBlockVisitor, private IR::ValueVisitor
	{
		struct Constant
		{
			bool isString;
			float64 f64;
			String string;
		};
	public:
		//Methods
		void Compile(IR::Module& module);
		void Write(OutputStream& outputStream);

	private:
		//Members
		DynamicArray<String> externals;
		Map<const IR::External*, uint16> externalMap;
		Map<String, uint16> procedureOffsetMap;
		DynamicArray<Constant> constants;
		FIFOBuffer codeSegment;
		Map<const IR::BasicBlock*, uint16> blockOffsets;
		Map<const IR::BasicBlock*, DynamicArray<uint16>> missingBlockOffsets;
		DynamicArray<const IR::Value*> valueStack;
		IR::Module* module;
		Map<const IR::Value*, uint32> instructionReferenceCounts;

		//Methods
		void PushValue(const IR::Value* value);

		//Inline
		inline uint16 AddConstant(float64 value)
		{
			return this->constants.Push({ .isString = false, .f64 = value });
		}

		inline uint16 AddConstant(const String& value)
		{
			return this->constants.Push({ .isString = true, .string = value });
		}

		inline void AddInstruction(Opcode op)
		{
			this->codeSegment.WriteBytes(&op, 1);
		}

		inline void AddInstruction(Opcode op, uint16 arg0)
		{
			DataWriter dataWriter(true, this->codeSegment);
			dataWriter.WriteByte(static_cast<byte>(op));
			dataWriter.WriteUInt16(arg0);
		}

		inline void AddPopAssignInstruction()
		{
			this->AddInstruction(Opcode::PopAssign);
			const IR::Value* top = this->valueStack.Pop();
			this->valueStack.Last() = top;
		}

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override;
		void OnVisitingConditionalBranchInstruction(IR::BranchOnTrueInstruction &branchOnTrueInstruction) override;
		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override;
		void OnVisitingNewObjectInstruction(IR::CreateNewObjectInstruction &createNewObjectInstruction) override;
		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override;
		void OnVisitingReturnInstruction(IR::ReturnInstruction &returnInstruction) override;
		void OnVisitingSelectInstruction(IR::SelectInstruction &selectInstruction) override;

		void OnVisitingConstantFloat(const IR::ConstantFloat &constantFloat) override;
		void OnVisitingConstantString(const IR::ConstantString &constantString) override;
		void OnVisitingExternal(const IR::External &external) override;
		void OnVisitingInstructionResultValue(IR::Instruction &instruction) override;
		void OnVisitingParameter(const IR::Parameter &parameter) override;
		void OnVisitingProcedure(const IR::Procedure &procedure) override;
	};
}