/*
* Copyright (c) 2018-2020 Amir Czwink (amir130@hotmail.de)
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

//Private methods
void ExternalsManager::RegisterAllExternals()
{
	//arithmetical
	extern RuntimeValue Add(const RuntimeValue&);
	this->RegisterExternal(u8"+", Add);

	extern RuntimeValue Multiply(const RuntimeValue&);
	this->RegisterExternal(u8"*", Multiply);

	extern RuntimeValue Subtract(const RuntimeValue&);
	this->RegisterExternal(u8"-", Subtract);

	//logical
	extern RuntimeValue And(const RuntimeValue&);
	this->RegisterExternal(u8"and", And);

	extern RuntimeValue LessThan(const RuntimeValue&);
	this->RegisterExternal(u8"<", LessThan);

	extern RuntimeValue LessThanOrEqual(const RuntimeValue&);
	this->RegisterExternal(u8"<=", LessThanOrEqual);

	extern RuntimeValue Not(const RuntimeValue&);
	this->RegisterExternal(u8"not", Not);

	extern RuntimeValue Or(const RuntimeValue&);
	this->RegisterExternal(u8"or", Or);

	//other
	extern RuntimeValue Print(const RuntimeValue&);
	this->RegisterExternal(u8"print", Print);
}
