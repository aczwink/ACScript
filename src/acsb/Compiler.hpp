/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "../ir/Compiler.hpp"
#include "Module.hpp"
#include "Opcode.hpp"
#include "Procedure.hpp"

namespace ACSB
{
	class Compiler : public IR::Compiler
	{
	public:
		//Constructor
		inline Compiler(const IR::Program& program) : program(program), lastProcOffset(0)
		{
		}

		//Methods
		void BeginProcedure() override;
		void CompileAccess(const IR::AccessOperation & op) override;
		void CompileCall(const IR::CallOperation & op) override;
		void CompileCreateArray(const IR::CreateArrayOperation & op) override;
		void CompileStore(const IR::StoreOperation& op) override;
		void CompileWait(const IR::WaitOperation & op) override;
		void EndProcedure() override;

		//Inline
		inline Module GetCompiledModule()
		{
			return Module(&this->opcodes[0], this->opcodes.GetSize(), Move(procedures), this->program);
		}

	private:
		//Members
		const IR::Program& program;
		DynamicArray<Procedure> procedures;
		DynamicArray<byte> opcodes;
		Map<String, uint32> localMap;
		Map<String, uint32> globalMap;
		uint16 nextLocalIdx;
		DynamicArray<const IR::Symbol*> executionStack;
		uint32 lastProcOffset;

		//Methods
		void CompileSymbol(const IR::Symbol& symbol);

		//Inline
		inline void AddInstruction(Opcode op)
		{
			this->opcodes.Push(op);
		}

		inline void AddInstruction(Opcode op, uint16 arg0)
		{
			byte buffer[2];
			BufferOutputStream outputStream(buffer, sizeof(buffer));
#ifdef XPC_ENDIANNESS_LITTLE
			bool bigEndian = false;
#else
			bool bigEndian = true;
#endif
			DataWriter dataWriter(bigEndian, outputStream);
			dataWriter.WriteUInt16(arg0);

			this->opcodes.Push(op);
			this->opcodes.Push(buffer[0]);
			this->opcodes.Push(buffer[1]);
		}

		inline uint16 GetGlobalIndex(const String& global)
		{
			if (!this->globalMap.Contains(global))
				this->globalMap.Insert(global, this->globalMap.GetNumberOfElements());
			return this->globalMap[global];
		}

		inline uint16 GetLocalIndex(const String& local)
		{
			if (!this->localMap.Contains(local))
				this->localMap.Insert(local, this->nextLocalIdx++);
			return this->localMap[local];
		}
	};
}