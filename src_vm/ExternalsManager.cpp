/*
* Copyright (c) 2018-2022 Amir Czwink (amir130@hotmail.de)
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
#include "ExternalsManager.hpp"
#include "MarkAndSweepGC.hpp"

//Private methods
void ExternalsManager::RegisterAllExternals()
{
    extern RuntimeValue External_Add(RuntimeValue&, const Module&, MarkAndSweepGC&);
    this->RegisterExternal(u8"+", External_Add);

    extern RuntimeValue External_Subtract(RuntimeValue&, const Module&, MarkAndSweepGC&);
    this->RegisterExternal(u8"-", External_Subtract);

    extern RuntimeValue External_Equals(RuntimeValue&, const Module&, MarkAndSweepGC&);
    this->RegisterExternal(u8"=", External_Equals);

    extern RuntimeValue External_LessThanOrEqual(RuntimeValue&, const Module&, MarkAndSweepGC&);
    this->RegisterExternal(u8"<=", External_LessThanOrEqual);

    extern RuntimeValue External_Multiply(RuntimeValue&, const Module&, MarkAndSweepGC&);
	this->RegisterExternal(u8"*", External_Multiply);

    extern RuntimeValue External_Print(RuntimeValue&, const Module&, MarkAndSweepGC&);
    this->RegisterExternal(u8"print", External_Print);
}
