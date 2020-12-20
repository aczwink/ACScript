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
//Local
#include "../RuntimeValue.hpp"

RuntimeValue Add(const RuntimeValue& arg)
{
	if(arg.Type() == RuntimeValueType::Tuple)
	{
		if(
				(arg.ValuesArray().GetNumberOfElements() == 2)
				&& (arg.ValuesArray()[0].Type() == RuntimeValueType::Float64)
				&& (arg.ValuesArray()[1].Type() == RuntimeValueType::Float64)
				)
		{
			float64 lhs = arg.ValuesArray()[0].ValueF64();
			float64 rhs = arg.ValuesArray()[1].ValueF64();
			return { lhs + rhs };
		}
	}
	return RuntimeValue();
}

RuntimeValue Multiply(const RuntimeValue& arg)
{
	if(arg.Type() == RuntimeValueType::Tuple)
	{
		if(
			(arg.ValuesArray().GetNumberOfElements() == 2)
			&& (arg.ValuesArray()[0].Type() == RuntimeValueType::Float64)
			&& (arg.ValuesArray()[1].Type() == RuntimeValueType::Float64)
		)
		{
			float64 lhs = arg.ValuesArray()[0].ValueF64();
			float64 rhs = arg.ValuesArray()[1].ValueF64();
			return { lhs * rhs };
		}
	}
	return RuntimeValue();
}

RuntimeValue Subtract(const RuntimeValue& arg)
{
	if(arg.Type() == RuntimeValueType::Tuple)
	{
		if(
				(arg.ValuesArray().GetNumberOfElements() == 2)
				&& (arg.ValuesArray()[0].Type() == RuntimeValueType::Float64)
				&& (arg.ValuesArray()[1].Type() == RuntimeValueType::Float64)
				)
		{
			float64 lhs = arg.ValuesArray()[0].ValueF64();
			float64 rhs = arg.ValuesArray()[1].ValueF64();
			return { lhs - rhs };
		}
	}
	return RuntimeValue();
}