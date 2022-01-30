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
#include "GCObject.hpp"

enum class RuntimeValueType
{
    Bool,
    Dictionary,
    External,
    Natural,
    String,
    Tuple,
    UInt64,
    Unit
};

//Forward declarations
class MarkAndSweepGC;
class Module;
class RuntimeValue;

typedef RuntimeValue(*External)(RuntimeValue&, const Module&, MarkAndSweepGC&);

class RuntimeValue
{
    friend class MarkAndSweepGC;
public:
    //Constructors
    inline RuntimeValue() : type(RuntimeValueType::Unit)
    {
    }

    inline RuntimeValue(bool b) : type(RuntimeValueType::Bool), b(b)
    {
    }

    inline RuntimeValue(External external) : type(RuntimeValueType::External), external(external)
    {
    }

    inline RuntimeValue(String&& string) : type(RuntimeValueType::String)
    {
        this->string = new String(Move(string));
    }

    inline RuntimeValue(uint64 v) : type(RuntimeValueType::UInt64), u64(v)
    {
    }

    inline RuntimeValue(RuntimeValueType type, GCObject* gcObject) : type(type)
    {
        this->gcObject = gcObject;
    }

    //Properties
    inline RuntimeValueType Type() const
    {
        return this->type;
    }

    inline DynamicArray<RuntimeValue>& ValuesArray()
    {
        return *this->gcObject->array;
    }

    inline const DynamicArray<RuntimeValue>& ValuesArray() const
    {
        return *this->gcObject->array;
    }

    inline bool ValueBool() const
    {
        return this->b;
    }

    inline External ValueExternal() const
    {
        ASSERT_EQUALS(RuntimeValueType::External, this->type);
        return this->external;
    }

    inline BinaryTreeMap<String, RuntimeValue>& ValuesDictionary()
    {
        return *this->dictionary;
    }

    inline const BinaryTreeMap<String, RuntimeValue>& ValuesDictionary() const
    {
        return *this->dictionary;
    }

    inline const Math::Natural& ValueNatural() const
    {
        ASSERT_EQUALS(RuntimeValueType::Natural, this->type);
        return *this->gcObject->natural;
    }

    inline const String& ValueString() const
    {
        ASSERT_EQUALS(RuntimeValueType::String, this->type);
        return *this->string;
    }

    inline uint64 ValueUInt64() const
    {
        ASSERT_EQUALS(RuntimeValueType::UInt64, this->type);
        return this->u64;
    }

    //Functions
    inline static RuntimeValue CreateDictionary()
    {
        RuntimeValue v;
        v.type = RuntimeValueType::Dictionary;
        v.dictionary = new BinaryTreeMap<String, RuntimeValue>;
        return v;
    }

private:
    //Members
    RuntimeValueType type;
    union
    {
        bool b;
        BinaryTreeMap<String, RuntimeValue>* dictionary;
        External external;
        String* string;
        uint64 u64;
        GCObject* gcObject;
    };
};