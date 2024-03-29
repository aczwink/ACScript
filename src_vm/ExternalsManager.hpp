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
#pragma once
//Local
#include "RuntimeValue.hpp"

class ExternalsManager
{
public:
    //Constructor
    inline ExternalsManager()
    {
        this->RegisterAllExternals();
    }

    //Inline
    inline External GetExternal(const String& name)
    {
        ASSERT(this->externals.Contains(name), u8"Missing external: " + name);
        return this->externals.Get(name);
    }

    inline void RegisterExternal(const String& name, External external)
    {
        this->externals[name] = external;
    }

private:
    //Members
    BinaryTreeMap<String, External> externals;

    //Methods
    void RegisterAllExternals();
};