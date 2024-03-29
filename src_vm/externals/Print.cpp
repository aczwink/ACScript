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
//Local
#include "../RuntimeValue.hpp"

static String ToString(const RuntimeValue& value)
{
    switch(value.Type())
    {
        case RuntimeValueType::Bool:
            return value.ValueBool() ? u8"true" : u8"false";
        case RuntimeValueType::Dictionary:
        {
            DynamicArray<String> strings;
            for(const auto& kv : value.ValuesDictionary())
                strings.Push(kv.key + u8": " + ToString(kv.value));
            return u8"{ " + String::Join(strings, u8", ") + u8" }";
        }
        case RuntimeValueType::External:
            return u8"__external__";
        case RuntimeValueType::Natural:
            return value.ValueNatural().ToString();
        case RuntimeValueType::String:
            return value.ValueString();
        case RuntimeValueType::Tuple:
        {
            DynamicArray<String> strings;
            for(const RuntimeValue& subValue : value.ValuesArray())
                strings.Push(ToString(subValue));
            return u8"(" + String::Join(strings, u8", ") + u8")";
        }
        case RuntimeValueType::UInt64:
            return String::Number(value.ValueUInt64());
        case RuntimeValueType::Unit:
            return u8"unit";
    }
    return String();
}

RuntimeValue External_Print(RuntimeValue& arg, const Module&, MarkAndSweepGC&)
{
    stdOut << ToString(arg) << endl;
    return RuntimeValue();
}