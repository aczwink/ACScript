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
#include "../Module.hpp"

RuntimeValue ObjectSet(RuntimeValue& arg, const Module&)
{
	if(arg.Type() == RuntimeValueType::Tuple)
	{
		if(
				(arg.ValuesArray().GetNumberOfElements() == 3)
				&& (arg.ValuesArray()[0].Type() == RuntimeValueType::Dictionary)
				&& (arg.ValuesArray()[1].Type() == RuntimeValueType::String)
		)
		{
			auto& dict = arg.ValuesArray()[0].ValuesDictionary();
			auto& key = arg.ValuesArray()[1].ValueString();
			dict[key] = arg.ValuesArray()[2];
			return arg.ValuesArray()[0];
		}
	}
	NOT_IMPLEMENTED_ERROR;
	return RuntimeValue();
}

RuntimeValue Select(RuntimeValue& arg, const Module&)
{
	if(arg.Type() == RuntimeValueType::Tuple)
	{
		if(
				(arg.ValuesArray().GetNumberOfElements() == 2)
				&& (arg.ValuesArray()[0].Type() == RuntimeValueType::Tuple)
				&& (arg.ValuesArray()[1].Type() == RuntimeValueType::Float64)
				&& (arg.ValuesArray()[0].ValuesArray().GetNumberOfElements() > arg.ValuesArray()[1].ValueF64())
				)
		{
			const DynamicArray<RuntimeValue>& array = arg.ValuesArray()[0].ValuesArray();
			float64 index = arg.ValuesArray()[1].ValueF64();
			return { array[index] };
		}
	}
	NOT_IMPLEMENTED_ERROR;
	return RuntimeValue();
}