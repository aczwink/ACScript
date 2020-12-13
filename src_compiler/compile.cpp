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
#include "AST/statements/StatementBlock.hpp"
#include "AST2IRTranslator.hpp"
#include "IR/visitors/Printer.hpp"
#include "acsb/Compiler.hpp"
#include "acsb/Disassembler.hpp"

void add_externals(IR::Builder& builder, TypeCatalog& typeCatalog)
{
	DynamicArray<const ::Type*> argTypes;
	argTypes.Push( typeCatalog.GetLeafType(LeafTypeEnum::Any) );
	IR::External* external = builder.CreateExternal(u8"print", typeCatalog.GetLeafType(LeafTypeEnum::Null),
												 typeCatalog.GetTupleType(Move(argTypes), true));
	builder.Module().AddExternal(external);
}

void compile(const StatementBlock& statementBlock)
{
	IR::Module module;
	TypeCatalog typeCatalog;
	IR::Builder builder(module, typeCatalog);

	add_externals(builder, typeCatalog);

	AST2IRTranslator translator(builder);
	statementBlock.Visit(translator);

	IR::Printer printer(stdOut);
	module.Visit(printer);

	ACSB::Compiler compiler;
	module.Visit(compiler);

	FIFOBuffer compiledModule;
	compiler.Write(compiledModule);

	ACSB::Disassembler disassembler(stdOut);
	FIFOBuffer copy = compiledModule;
	disassembler.Disassemble(copy);

	FileOutputStream fileOutputStream(String(u8"a.out.acsb"), true);
	compiledModule.FlushTo(fileOutputStream);
}