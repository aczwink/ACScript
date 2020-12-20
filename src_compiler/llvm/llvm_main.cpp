/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#include "../AST/statements/StatementBlock.hpp"
#include "llvmCompiler.hpp"

//Libs
#undef PI
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>

void OutputObjectCode(llvm::Module& module)
{
	module.print(llvm::errs(), nullptr);

	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmParser();
	llvm::InitializeNativeTargetAsmPrinter();

	std::string TargetTriple = llvm::sys::getDefaultTargetTriple();
	module.setTargetTriple(TargetTriple);

	std::string Error;
	const llvm::Target* Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

	if (!Target) {
		llvm::errs() << Error;
		return;
	}

	auto CPU = "generic";
	auto Features = "";

	llvm::TargetOptions opt;
	auto RM = llvm::Optional<llvm::Reloc::Model>();
	auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	module.setDataLayout(TheTargetMachine->createDataLayout());

	auto Filename = "llvm-module-output.s";
	auto Filename2 = "llvm-module-output.o";
	std::error_code EC;
	llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);
	llvm::raw_fd_ostream dest2(Filename2, EC, llvm::sys::fs::OF_None);

	if (EC) {
		llvm::errs() << "Could not open file: " << EC.message();
		return;
	}

	llvm::legacy::PassManager pass;
	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_AssemblyFile)) {
		llvm::errs() << "TheTargetMachine can't emit a file of this type";
		return;
	}
	pass.run(module);

	llvm::legacy::PassManager pass2;
	if (TheTargetMachine->addPassesToEmitFile(pass2, dest2, nullptr, llvm::CGFT_ObjectFile)) {
		llvm::errs() << "TheTargetMachine can't emit a file of this type";
		return;
	}
	pass2.run(module);


	dest.flush();
	dest2.flush();

	llvm::outs() << "Wrote " << Filename << "\n";
}

void llvm_main(const AST::StatementBlock& statementBlock)
{
	/*
	llvmCompiler compiler;
	statementBlock.Visit(compiler);
	compiler.OnVisitedBlock(statementBlock);

	compiler.Module().print(llvm::outs(), nullptr);
	OutputObjectCode(compiler.Module());*/
}