/*
* Copyright (c) 2019 Amir Czwink (amir130@hotmail.de)
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
#include "Block.hpp"
//Local
#include "LoadConstantOperation.hpp"
#include "Program.hpp"
//Namespaces
using namespace IR;

//Public methods
const Symbol& Block::AddLocalVariable(const String& identifier, bool isMutable)
{
	this->variables.InsertTail(Move(Symbol::LocalVariable(identifier)));
	this->localMutable.Insert(identifier, isMutable);
	return this->variables.Get(this->variables.GetNumberOfElements() - 1);
}

bool Block::CanDeriveIdentifier(const String& identifier) const
{
	for (const Symbol& s : this->variables)
		if (s.GetIdentifier() == identifier)
			return true;

	if (this->parent)
		return this->parent->CanDeriveIdentifier(identifier);
	
	return false;
}

void Block::Compile(Compiler& compiler)
{
	for (const auto& op : this->operations)
		op->Compile(compiler);
}

Symbol Block::DeriveIdentifierSymbol(const String& identifier)
{
	if (identifier == u8"this")
		return Symbol::This();

	for (const Symbol& s : this->variables)
		if (s.GetIdentifier() == identifier)
			return s;
	if (this->localToGlobalMap.Contains(identifier))
		return *this->GetProgram().GetGlobal(this->localToGlobalMap[identifier]);

	//check parent
	Block* owner = this->GetLocalOwner(identifier);
	if(owner)
	{
		if (this->parent && (&this->parent->proc != &this->proc))
		{
			owner->MakeLocalGlobal(identifier);
		}
		return this->parent->DeriveIdentifierSymbol(identifier);
	}

	//check globals
	IR::Symbol* global = this->GetProgram().GetGlobal(identifier);
	if (global)
		return *global;
	
	//unknown identifier
	NOT_IMPLEMENTED_ERROR;
}

Program& Block::GetProgram()
{
	return this->proc.GetProgram();
}

const Program& Block::GetProgram() const
{
	return this->proc.GetProgram();
}

String Block::ToString(uint32 nTabs) const
{
	String result;
	for (const auto& op : this->operations)
	{
		for (uint32 i = 0; i < nTabs; i++)
			result += u8"\t";
		result += op->ToString() + u8"\r\n";
	}
	return result;
}

//Private methods
Block* Block::GetLocalOwner(const String& identifier)
{
	for (const Symbol& s : this->variables)
		if (s.GetIdentifier() == identifier)
			return this;
	if (this->localToGlobalMap.Contains(identifier))
		return this;

	if (this->parent)
		return this->parent->GetLocalOwner(identifier);

	return nullptr;
}

void Block::MakeLocalGlobal(const String& identifier)
{
	String globalName = this->proc.GetName() + u8"_" + identifier;
	this->localToGlobalMap.Insert(identifier, globalName);

	//delete local
	Symbol* s;
	auto it = this->variables.begin();
	while (it != this->variables.end())
	{
		if ((*it).GetIdentifier() == identifier)
		{
			s = &(*it);
			break;
		}
		++it;
	}

	//add global
	Symbol global = this->GetProgram().AddGlobal(globalName, s->GetType(), this->localMutable[identifier]);

	//replace local with global in operations
	this->ReplaceSymbol(*s, global);

	//remove
	this->variables.Remove(it.GetIndex());
}

void Block::ReplaceSymbol(const Symbol& from, const Symbol& to)
{
	for (auto& op : this->operations)
	{
		op->ReplaceSymbol(from, to);
	}

	for (Block* block : this->children)
	{
		block->ReplaceSymbol(from, to);
	}
}