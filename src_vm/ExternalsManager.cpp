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
	extern RuntimeValue Add(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"+", Add);

	extern RuntimeValue Multiply(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"*", Multiply);

	extern RuntimeValue Subtract(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"-", Subtract);

	//logical
	extern RuntimeValue And(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"and", And);

	extern RuntimeValue Equals(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"=", Equals);

	extern RuntimeValue LessThan(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"<", LessThan);

	extern RuntimeValue LessThanOrEqual(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"<=", LessThanOrEqual);

	extern RuntimeValue Not(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"not", Not);

	extern RuntimeValue Or(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"or", Or);

	//other
	extern RuntimeValue Select(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"[]", Select);

	extern RuntimeValue ObjectSet(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"__set", ObjectSet);

	extern RuntimeValue Print(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"print", Print);
}
