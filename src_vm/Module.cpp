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
//Class header
#include "Module.hpp"

//Constructor
Module::Module(SeekableInputStream &inputStream, ExternalsManager& externalsManager)
{
	DataReader dataReader(true, inputStream);
	TextReader textReader(inputStream, TextCodecType::UTF8);

	dataReader.Skip(4); //skip signature

	uint16 nImports = dataReader.ReadUInt16();
	this->moduleExternals.Resize(nImports);
	for(uint16 i = 0; i < nImports; i++)
	{
		this->moduleExternals[i] = externalsManager.GetExternal(textReader.ReadZeroTerminatedString());
	}

	uint16 nConstants = dataReader.ReadUInt16();
	this->constants.Resize(nConstants);
	this->constantStrings.EnsureCapacity(nConstants);
	for(uint16 i = 0; i < nConstants; i++)
	{
		if(dataReader.ReadByte())
		{
			uint32 index = this->constantStrings.Push(textReader.ReadZeroTerminatedString());
			this->constants[i] = &this->constantStrings[index];
		}
		else
			this->constants[i] = dataReader.ReadFloat64();
	}

	this->entryPoint = dataReader.ReadUInt16();

	uint64 codeSize = inputStream.QueryRemainingBytes();
	this->code = MemAlloc(codeSize);
	inputStream.ReadBytes(this->code, codeSize);
}

//Destructor
Module::~Module()
{
	MemFree(this->code);
}
