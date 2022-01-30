/*
* Copyright (c) 2022 Amir Czwink (amir130@hotmail.de)
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
#include "MarkAndSweepGC.hpp"

//Private methods
MarkAndSweepGC::GCObjectStorage& MarkAndSweepGC::AllocateObject()
{
    if( this->gcObjects.GetNumberOfElements() == 1000) //TODO: do this better
        this->RunGarbageCollection();

    this->gcObjects.InsertTail({});
    GCObjectStorage& gcObject = this->gcObjects.Last();
    gcObject.marked = false;

    return gcObject;
}

void MarkAndSweepGC::MarkObject(const RuntimeValue& value)
{
    switch(value.Type())
    {
        case RuntimeValueType::Natural:
        {
            value.gcObject->marked = true;
        }
        break;
        case RuntimeValueType::Tuple:
        {
            value.gcObject->marked = true;
            for(const RuntimeValue& child : value.ValuesArray())
                this->MarkObject(child);
        }
        break;
    }
}

void MarkAndSweepGC::ReleaseObject(const GCObjectStorage& gcObjectStorage)
{
    switch(gcObjectStorage.type)
    {
        case GCObjectType::Array:
            delete gcObjectStorage.array;
            break;
        case GCObjectType::Natural:
            delete gcObjectStorage.natural;
            break;
    }
}

void MarkAndSweepGC::Sweep()
{
    for(auto it = this->gcObjects.begin(); it != this->gcObjects.end();)
    {
        GCObjectStorage& gcObject = *it;
        if(gcObject.marked)
        {
            gcObject.marked = false;
            ++it;
        }
        else
        {
            this->ReleaseObject(gcObject);
            it.Remove();
        }
    }
}