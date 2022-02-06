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

RuntimeValue External_Equals(RuntimeValue& arg, const Module&, MarkAndSweepGC&)
{
    if(arg.Type() == RuntimeValueType::Tuple)
    {
        if(
                (arg.ValuesArray().GetNumberOfElements() == 2)
                && (arg.ValuesArray()[0].Type() == RuntimeValueType::Natural)
                && (arg.ValuesArray()[1].Type() == RuntimeValueType::Natural)
                )
        {
            const Math::Natural& lhs = arg.ValuesArray()[0].ValueNatural();
            const Math::Natural& rhs = arg.ValuesArray()[1].ValueNatural();
            return { lhs == rhs };
        }
    }
    NOT_IMPLEMENTED_ERROR;
    return RuntimeValue();
}

RuntimeValue External_LessThanOrEqual(RuntimeValue& arg, const Module&, MarkAndSweepGC&)
{
    if((arg.Type() == RuntimeValueType::Tuple) and (arg.ValuesArray().GetNumberOfElements() == 2))
    {
        if(
                (arg.ValuesArray()[0].Type() == RuntimeValueType::Natural)
                and (arg.ValuesArray()[1].Type() == RuntimeValueType::Natural)
                )
        {
            const Math::Natural& lhs = arg.ValuesArray()[0].ValueNatural();
            const Math::Natural& rhs = arg.ValuesArray()[1].ValueNatural();
            return { lhs <= rhs };
        }
        else if(
                (arg.ValuesArray()[0].Type() == RuntimeValueType::UInt64)
                and (arg.ValuesArray()[1].Type() == RuntimeValueType::UInt64)
                )
        {
            uint64 lhs = arg.ValuesArray()[0].ValueUInt64();
            uint64 rhs = arg.ValuesArray()[1].ValueUInt64();
            return { lhs <= rhs };
        }
    }
    NOT_IMPLEMENTED_ERROR;
    return RuntimeValue();
}