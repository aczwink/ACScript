/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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
#include "../instructions/BranchOnTrueInstruction.hpp"
#include "../instructions/CallInstruction.hpp"
#include "../instructions/CreateNewObjectInstruction.hpp"
#include "../instructions/CreateNewTupleInstruction.hpp"
#include "../instructions/ExternalCallInstruction.hpp"
#include "../instructions/ReturnInstruction.hpp"
#include "../instructions/SelectInstruction.hpp"
#include "../instructions/StoreInstruction.hpp"

#include "../ConstantFloat.hpp"
#include "../ConstantString.hpp"
#include "../External.hpp"
#include "../Procedure.hpp"