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
//Class header
#include "Module.hpp"

//Constructor
Module::Module(SeekableInputStream &inputStream, ExternalsManager& externalsManager)
{
	DataReader fourccReader(false, inputStream);
	DataReader dataReader(true, inputStream);
	TextReader textReader(inputStream, TextCodecType::UTF8);

	while(!inputStream.IsAtEnd())
	{
		uint32 chunkId = fourccReader.ReadUInt32();
		uint32 chunkSize = dataReader.ReadUInt32();

		switch(chunkId)
		{
			case FOURCC(u8"ACSB"):
				ASSERT_EQUALS(0, chunkSize);
				break;
			case FOURCC(u8"code"):
			{
				//this->entryPoint = dataReader.ReadUInt16();
				this->entryPoint = 0;

				uint32 codeSize = chunkSize;
				this->code = MemAlloc(codeSize);
				inputStream.ReadBytes(this->code, codeSize);
			}
			break;
			case FOURCC(u8"data"):
			{
				uint16 nConstants = dataReader.ReadUInt16();
				this->constants.Resize(nConstants);

				for(uint16 i = 0; i < nConstants; i++)
				{
					uint8 type = dataReader.ReadByte();
					switch(type)
					{
						case u8'n':
						{
							uint32 index = this->constantNaturals.Push(Math::Natural(textReader.ReadZeroTerminatedString()));
							this->constants[i] = &this->constantNaturals[index];
						}
						break;
						case u8's':
						{
							uint32 index = this->constantStrings.Push(textReader.ReadZeroTerminatedString());
							this->constants[i] = &this->constantStrings[index];
						}
						break;
						default:
							NOT_IMPLEMENTED_ERROR; //TODO: implement me
					}
				}
			}
			break;
			case FOURCC(u8"impt"):
			{
				uint16 nImports = dataReader.ReadUInt16();
				this->moduleExternals.Resize(nImports);
				for(uint16 i = 0; i < nImports; i++)
				{
					this->moduleExternals[i] = externalsManager.GetExternal(textReader.ReadZeroTerminatedString());
				}
			}
			break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}
}

//Destructor
Module::~Module()
{
	MemFree(this->code);
}
