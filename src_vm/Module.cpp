/*
* Copyright (c) 2019-2022 Amir Czwink (amir130@hotmail.de)
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
Module::Module(InputStream &inputStream, ExternalsManager& externalsManager) : externalsManager(externalsManager), gc(constants)
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
                this->entryPoint = dataReader.ReadUInt16();

                uint32 codeSize = chunkSize - 2;
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
                                this->constants[i] = {RuntimeValueType::Natural, this->gc.CreateNatural(textReader.ReadZeroTerminatedString())};
                            }
                            break;
                        case u8's':
                            {
                                this->constants[i] = textReader.ReadZeroTerminatedString();
                            }
                            break;
                        case u8'u':
                            {
                                this->constants[i] = textReader.ReadZeroTerminatedString().ToUInt();
                            }
                            break;
                        default:
                            NOT_IMPLEMENTED_ERROR; //TODO: implement me
                    }
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