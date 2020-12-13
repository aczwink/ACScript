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
//Local
#include "Visitor.hpp"
#include "../Instruction.hpp"

namespace IR
{
	class Printer : public Visitor
	{
	public:
		//Constructor
		inline Printer(TextWriter& textWriter) : textWriter(textWriter)
		{
		}

		//Methods
		void OnVisitedNewTupleInstruction(const CreateNewTupleInstruction &createNewTupleInstruction) override;

		void OnVisitingConstant(const ConstantFloat &constantFloat) override;
		void OnVisitingExternal(const External& external) override;
		void OnVisitingExternalCallInstruction(const ExternalCallInstruction &externalCallInstruction) override;
		void OnVisitingNewTupleInstruction(const CreateNewTupleInstruction &createNewTupleInstruction) override;
		void OnVisitingProcedure(const Procedure &procedure) override;
		void OnVisitingReturnInstruction(const ReturnInstruction &returnInstruction) override;

	private:
		//Members
		TextWriter& textWriter;

		//Inline
		inline void PrintInstruction(const Instruction& instruction)
		{
			this->textWriter.WriteTabs(1);
			this->textWriter << instruction.ToString() << endl;
		}
	};
}