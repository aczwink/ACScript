/*
* Copyright (c) 2018-2021 Amir Czwink (amir130@hotmail.de)
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
#include "RuntimeValue.hpp"

class GarbageCollector
{
public:
	~GarbageCollector()
	{
		BinaryTreeSet<DynamicArray<RuntimeValue>*> toDelete = this->allocatedArrays;
		for (DynamicArray<RuntimeValue>*const& toDeleteElement : toDelete)
			this->Release(toDeleteElement);

		BinaryTreeSet<BinaryTreeMap<String, RuntimeValue>*> toDelete2 = this->allocatedDictionaries;
		for(BinaryTreeMap<String, RuntimeValue>*const& toDeleteElement : toDelete2)
			this->Release(toDeleteElement);
	}

	//Inline
	inline void CleanUp(const DynamicArray<RuntimeValue>& referencedValues)
	{
		if(this->allocatedArrays.GetNumberOfElements() < 5000000)
			return;

		BinaryTreeSet<DynamicArray<RuntimeValue>*> toDelete = this->allocatedArrays;

		for (const RuntimeValue& referenced : referencedValues)
		{
			if(referenced.Type() == RuntimeValueType::Tuple)
				toDelete.Remove((DynamicArray<RuntimeValue>*)&referenced.ValuesArray());
		}

		for (DynamicArray<RuntimeValue>*const& toDeleteElement : toDelete)
			this->Release(toDeleteElement);
	}

	inline DynamicArray<RuntimeValue>* NewArray()
	{
		DynamicArray<RuntimeValue>* ptr = new DynamicArray<RuntimeValue>;
		this->allocatedArrays.Insert(ptr);
		return ptr;
	}

	inline BinaryTreeMap<String, RuntimeValue>* NewDictionary()
	{
		BinaryTreeMap<String, RuntimeValue>* ptr = new BinaryTreeMap<String, RuntimeValue>;
		this->allocatedDictionaries.Insert(ptr);
		return ptr;
	}

private:
	//Members
	BinaryTreeSet<DynamicArray<RuntimeValue>*> allocatedArrays;
	BinaryTreeSet<BinaryTreeMap<String, RuntimeValue>*> allocatedDictionaries;

	//Inline
	inline void Release(DynamicArray<RuntimeValue>* ptr)
	{
		this->allocatedArrays.Remove(ptr);
		delete ptr;
	}

	inline void Release(BinaryTreeMap<String, RuntimeValue>* ptr)
	{
		this->allocatedDictionaries.Remove(ptr);
		delete ptr;
	}
};