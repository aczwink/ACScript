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
#pragma once
//Local
#include "RuntimeValue.hpp"

class MarkAndSweepGC
{
    enum class GCObjectType : uint8
    {
        Array,
        Natural
    };

    struct GCObjectStorage : GCObject
    {
        GCObjectType type;
    };
public:
    //Constructor
    inline MarkAndSweepGC(const DynamicArray<RuntimeValue>& reachableObjects) : reachableObjects(reachableObjects)
    {
    }

    //Destructor
    inline ~MarkAndSweepGC()
    {
        this->RunGarbageCollection();
    }

    //Inline
    inline GCObject* CreateArray()
    {
        GCObjectStorage& gcObject = this->AllocateObject();
        gcObject.type = GCObjectType::Array;
        gcObject.array = new DynamicArray<RuntimeValue>;

        return &gcObject;
    }

    inline GCObject* CreateNatural(Math::Natural&& natural)
    {
        GCObjectStorage& gcObject = this->AllocateObject();
        gcObject.type = GCObjectType::Natural;
        gcObject.natural = new Math::Natural(Move(natural));

        return &gcObject;
    }

    inline void RunGarbageCollection()
    {
        this->Mark();
        this->Sweep();
    }

private:
    //Members
    const DynamicArray<RuntimeValue>& reachableObjects;
    LinkedList<GCObjectStorage> gcObjects;

    //Methods
    GCObjectStorage& AllocateObject();
    void MarkObject(const RuntimeValue& runtimeValue);
    void ReleaseObject(const GCObjectStorage& gcObjectStorage);
    void Sweep();

    //Inline
    inline void Mark()
    {
        for(const RuntimeValue& value : this->reachableObjects)
            this->MarkObject(value);
    }
};