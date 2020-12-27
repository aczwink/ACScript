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
void Disassembler::Disassemble(FIFOBuffer &inputStream)
{
	DataReader dataReader(true, inputStream);
	TextReader textReader(inputStream, TextCodecType::UTF8);

	dataReader.Skip(4); //skip signature

	this->textWriter << u8".import" << endl;
	uint16 nImports = dataReader.ReadUInt16();
	this->externals.Resize(nImports);
	for(uint16 i = 0; i < nImports; i++)
	{
		this->textWriter.WriteTabs(1);
		this->externals[i] = textReader.ReadZeroTerminatedString();
		this->textWriter << u8"i" << i << u8": " << this->externals[i] << endl;
	}

	this->textWriter << endl;
	this->textWriter << u8".rodata" << endl;

	uint16 nConstants = dataReader.ReadUInt16();
	this->constantsAsStrings.Resize(nConstants);
	for(uint16 i = 0; i < nConstants; i++)
	{
		this->textWriter.WriteTabs(1);
		this->textWriter << u8"c" << i << u8": ";

		if(dataReader.ReadByte())
			this->constantsAsStrings[i] = u8"\"" + textReader.ReadZeroTerminatedString() + u8"\"";
		else
			this->constantsAsStrings[i] = String::Number(dataReader.ReadFloat64());

		this->textWriter << this->constantsAsStrings[i] << endl;
	}

	this->textWriter << endl;
	this->textWriter << u8".code" << endl;
	uint16 entryPoint = dataReader.ReadUInt16();

	FixedSizeBuffer buffer(inputStream.GetRemainingBytes());
	inputStream.PeekBytes(buffer.Data(), 0, buffer.Size());
	BufferInputStream bufferInputStream(buffer.Data(), buffer.Size());
	DataReader dataReaderBuffer(true, bufferInputStream);

	this->jumpOffsets.Insert(entryPoint, Unsigned<uint32>::Max());
	this->FindJumpOffsets(dataReaderBuffer);

	uint16 offset = 0;
	uint16 jumpLabelCounter = 0;
	while(!inputStream.IsAtEnd())
	{
		if(this->jumpOffsets.Contains(offset))
		{
			this->jumpOffsets[offset] = jumpLabelCounter;
			this->textWriter << u8"__lbl" << jumpLabelCounter << u8":" << endl;
			jumpLabelCounter++;
		}

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
	offset += this->GetArgumentsSize(op);

	switch (op)
	{
		case Opcode::Call:
		{
			uint16 callOffset = dataReader.ReadUInt16();

			this->OutputMnemonicWithOneArgument(u8"call", u8"__lbl" + String::Number(this->jumpOffsets[callOffset]));
		}
		break;
		case Opcode::CallExtern:
		{
			uint16 externalIndex = dataReader.ReadUInt16();

			this->OutputMnemonicWithOneArgumentAndComment(u8"callext", u8"i" + String::Number(externalIndex), this->externals[externalIndex]);
		}
		break;
		case Opcode::JumpOnFalse:
		{
			uint16 jumpOffset = dataReader.ReadUInt16();

			this->OutputMnemonicWithOneArgument(u8"jmpf", jumpOffset);
		}
		break;
		case Opcode::LoadConstant:
		{
			uint16 constantIndex = dataReader.ReadUInt16();

			this->OutputMnemonicWithOneArgumentAndComment(u8"ldc", u8"c" + String::Number(constantIndex), this->constantsAsStrings[constantIndex]);
		}
		break;
		case Opcode::NewDictionary:
		{
			this->OutputMnemonic(u8"ndict");
		}
		break;
		case Opcode::NewTuple:
		{
			uint16 count = dataReader.ReadUInt16();

			this->OutputMnemonicWithOneArgument(u8"ntp", count);
		}
		break;
		case Opcode::Pop:
		{
			this->OutputMnemonic(u8"pop");
		}
		break;
		case Opcode::PopAssign:
		{
			this->OutputMnemonic(u8"popa");
		}
		break;
		case Opcode::Push:
		{
			uint16 backIndex = dataReader.ReadUInt16();
			this->OutputMnemonicWithOneArgument(u8"push", backIndex);
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

void Disassembler::FindJumpOffsets(DataReader &dataReader)
{
	while(!dataReader.InputStream().IsAtEnd())
	{
		Opcode op = static_cast<Opcode>(dataReader.ReadByte());

		switch(op)
		{
			case Opcode::Call:
			{
				uint16 callOffset = dataReader.ReadUInt16();
				this->jumpOffsets.Insert(callOffset, Unsigned<uint32>::Max());
			}
			break;
			default:
				dataReader.Skip(this->GetArgumentsSize(op));
		}
	}
}

uint8 Disassembler::GetArgumentsSize(Opcode opcode) const
{
	switch(opcode)
	{
		case Opcode::Call:
		case Opcode::CallExtern:
		case Opcode::JumpOnFalse:
		case Opcode::LoadConstant:
		case Opcode::NewTuple:
		case Opcode::Push:
			return 2;
	}
	return 0;
}
