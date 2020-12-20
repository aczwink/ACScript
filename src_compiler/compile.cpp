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
#include "translation/AST2IRTranslator.hpp"
#include "IR/visitors/Printer.hpp"
#include "acsb/Compiler.hpp"
#include "acsb/Disassembler.hpp"
#include "AST/ParserState.hpp"
#include "optimization/PassManager.hpp"
#include "optimization/typeInference/ProcedureTypeInferer.hpp"

void compile(const AST::ParserState& parserState)
{
	IR::Module module;
	TypeCatalog typeCatalog;
	IR::Builder builder(module, typeCatalog);

	AST2IRTranslator translator(builder, typeCatalog);
	translator.Translate(parserState.ModuleStatements());

	Optimization::PassManager passManager(module, typeCatalog);
	passManager.PrintState();
	passManager.AddProcedurePass<Optimization::ProcedureTypeInferer>(u8"procedure_type_inferer");
	//passManager.AddProcedurePass<Optimization::ProcedureTypeInferer>(u8"procedure_type_inferer_round2"); //must be applied twice because of "self" keyword

	ACSB::Compiler compiler;
	compiler.Compile(module);

	FIFOBuffer compiledModule;
	compiler.Write(compiledModule);

	ACSB::Disassembler disassembler(stdOut);
	FIFOBuffer copy = compiledModule;
	disassembler.Disassemble(copy);

	FileOutputStream fileOutputStream(String(u8"a.out.acsb"), true);
	compiledModule.FlushTo(fileOutputStream);
}