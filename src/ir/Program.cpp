/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "Program.hpp"
//Namespaces
using namespace IR;

//Public methods
String Program::ToString() const
{
	String tmp;

	//constants
	for (const auto& kv : this->i64Constants)
		tmp += Symbol::Constant(kv.value, LeafType::I64).ToString() + u8" := " + String::Number(kv.key) + u8"\r\n";
	for (const auto& kv : this->ui64Constants)
		tmp += Symbol::Constant(kv.value, LeafType::U64).ToString() + u8" := " + String::Number(kv.key) + u8"\r\n";
	for (const auto& kv : this->stringConstants)
		tmp += Symbol::Constant(kv.value, LeafType::String).ToString() + u8" := \"" + kv.key + u8"\"\r\n";
	tmp += u8"\r\n";

	//globals
	for (const Symbol& global : this->globals)
		tmp += global.ToString() + u8"\r\n";
	tmp += u8"\r\n";

	//procedures
	for (const Procedure& procedure : this->procedures)
		tmp += procedure.ToString() + u8"\r\n";

	return tmp;
}