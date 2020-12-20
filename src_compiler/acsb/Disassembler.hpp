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
#include <StdXX.hpp>
using namespace StdXX;

namespace ACSB
{
	class Disassembler
	{
	public:
		//Constructor
		inline Disassembler(TextWriter &textWriter) : textWriter(textWriter)
		{
		}

		//Methods
		void Disassemble(InputStream &inputStream) const;

	private:
		//Members
		TextWriter &textWriter;

		//Methods
		void DisassembleInstruction(DataReader& dataReader, uint16& offset) const;

		//Inline
		inline void OutputMnemonic(const String& mnemonic) const
		{
			this->textWriter << mnemonic;
			this->textWriter.WriteTabs(1);
			if(mnemonic.GetLength() < 4)
				this->textWriter.WriteTabs(1);
		}

		inline void OutputMnemonicWithOneArgument(const String& mnemonic, uint16 arg) const
		{
			this->OutputMnemonicWithOneArgument(mnemonic, String::HexNumber(arg, 4));
		}

		inline void OutputMnemonicWithOneArgument(const String& mnemonic, const String& arg) const
		{
			this->OutputMnemonic(mnemonic);
			this->textWriter << arg;
		}
	};
}