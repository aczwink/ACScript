/*
* Copyright (c) 2019-2020 Amir Czwink (amir130@hotmail.de)
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

namespace ACSB
{
	class Compiler : private IR::BasicBlockVisitor, private IR::ValueVisitor
	{
	public:
		//Methods
		void Compile(const IR::Module& module);
		void Write(OutputStream& outputStream);

	private:
		//Members
		DynamicArray<String> externals;
		Map<const IR::External*, uint16> externalMap;
		Map<String, uint16> procedureOffsetMap;
		DynamicArray<float64> constants;
		FIFOBuffer codeSegment;
		Map<const IR::BasicBlock*, uint16> blockOffsets;
		Map<const IR::BasicBlock*, DynamicArray<uint16>> missingBlockOffsets;

		//Inline
		inline uint16 AddConstant(float64 value)
		{
			return this->constants.Push(value);
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

		//Event handlers
		void OnVisitingCallInstruction(IR::CallInstruction &callInstruction) override;
		void OnVisitingConditionalBranchInstruction(const IR::BranchOnTrueInstruction &branchOnTrueInstruction) override;
		void OnVisitingExternalCallInstruction(const IR::ExternalCallInstruction &externalCallInstruction) override;
		void OnVisitingNewTupleInstruction(IR::CreateNewTupleInstruction &createNewTupleInstruction) override;
		void OnVisitingReturnInstruction(const IR::ReturnInstruction &returnInstruction) override;

		void OnVisitingConstantFloat(const IR::ConstantFloat &constantFloat) override;
		void OnVisitingExternal(const IR::External &external) override;
		void OnVisitingParameter(const IR::Parameter &parameter) override;
	};
}