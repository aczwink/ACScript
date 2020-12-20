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
#include <acsb/Opcode.hpp>
#include "Disassembler.hpp"
//Namespaces
using namespace ACSB;

//Public methods
void Disassembler::Disassemble(InputStream &inputStream) const
{
	DataReader dataReader(true, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	dataReader.Skip(4); //skip signature

	this->textWriter << u8".import" << endl;
	uint16 nImports = dataReader.ReadUInt16();
	for(uint16 i = 0; i < nImports; i++)
	{
		this->textWriter.WriteTabs(1);
		this->textWriter << u8"i" << i << u8": " << textReader.ReadZeroTerminatedString() << endl;
	}

	this->textWriter << endl;
	this->textWriter << u8".rodata" << endl;

	uint16 nConstants = dataReader.ReadUInt16();
	for(uint16 i = 0; i < nConstants; i++)
	{
		this->textWriter.WriteTabs(1);
		this->textWriter << u8"c" << i << u8": " << dataReader.ReadFloat64() << endl;
	}

	this->textWriter << endl;
	this->textWriter << u8".code" << endl;
	dataReader.ReadUInt16(); //entry point

	uint16 offset = 0;
	while(!inputStream.IsAtEnd())
	{
		this->textWriter.WriteTabs(1);
		this->textWriter << String::HexNumber(offset, 4);

		this->textWriter.WriteTabs(1);
		this->DisassembleInstruction(dataReader, offset);

		this->textWriter << endl;
	}
}

//Private methods
void Disassembler::DisassembleInstruction(DataReader &dataReader, uint16& offset) const
{
	Opcode op = static_cast<Opcode>(dataReader.ReadByte());
	offset++;

	switch (op)
	{
		case Opcode::Call:
		{
			uint16 callOffset = dataReader.ReadUInt16();
			offset += 2;

			this->OutputMnemonicWithOneArgument(u8"call", callOffset);
		}
		break;
		case Opcode::CallExtern:
		{
			uint16 externalIndex = dataReader.ReadUInt16();
			offset += 2;

			this->OutputMnemonicWithOneArgument(u8"callext", u8"i" + String::Number(externalIndex));
		}
		break;
		case Opcode::JumpOnFalse:
		{
			uint16 jumpOffset = dataReader.ReadUInt16();
			offset += 2;

			this->OutputMnemonicWithOneArgument(u8"jmpf", jumpOffset);
		}
		break;
		case Opcode::LoadConstant:
		{
			uint16 constantIndex = dataReader.ReadUInt16();
			offset += 2;

			this->OutputMnemonicWithOneArgument(u8"ldc", u8"c" + String::Number(constantIndex));
		}
		break;
		case Opcode::NewTuple:
		{
			uint16 count = dataReader.ReadUInt16();
			offset += 2;

			this->OutputMnemonicWithOneArgument(u8"ntp", count);
		}
		break;
		case Opcode::PushParameter:
		{
			this->OutputMnemonic(u8"pusharg");
		}
		break;
		case Opcode::Return:
		{
			this->OutputMnemonic(u8"ret");
		}
		break;
		default:
			this->textWriter << u8"??? Unknown instruction ???: " << String::HexNumber(static_cast<uint64>(op));
	}
}
