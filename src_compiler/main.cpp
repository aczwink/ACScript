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
#include <StdXX.hpp>
using namespace StdXX;
using namespace StdXX::FileSystem;
#include "AST/Parser.hpp"

void compile(const StatementBlock&);
void llvm_main(const StatementBlock&);

static bool Parse(const Path& path, AST::ParserState& parserState)
{
	AST::Parser parser(parserState);

	//load lib
	//Path libPath = String(u8"/usr/local/share/acs_lib");
	Path libPath = String(u8"../acs_lib");
	AutoPointer<Directory> libDir = OSFileSystem::GetInstance().GetDirectory(libPath);
	for(const String& childName : *libDir)
	{
		if(!parser.Parse(libPath / childName))
		{
			stdErr << u8"Failed parsing input." << endl;
			return false;
		}
	}
	parser.Parse(path);

	return true;
}

int32 Main(const String& programName, const FixedArray<String>& args)
{
	CommandLine::Parser commandLineParser(programName);

	commandLineParser.AddHelpOption();

	CommandLine::Option llvmOption(u8'l', u8"llvm", u8"Compile using llvm");
	commandLineParser.AddOption(llvmOption);

	CommandLine::PathArgument inputPathArgument(u8"inputPath", u8"The path to the source file to be compiled");
	commandLineParser.AddPositionalArgument(inputPathArgument);

	if(!commandLineParser.Parse(args))
	{
		commandLineParser.PrintHelp();
		return EXIT_FAILURE;
	}

	const CommandLine::MatchResult& cmdParseResult = commandLineParser.ParseResult();
	Path path = inputPathArgument.Value(cmdParseResult);

	AST::ParserState parserState;
	if(!Parse(path, parserState))
	{
		return EXIT_FAILURE;
	}

	if(cmdParseResult.IsActivated(llvmOption))
	{
		llvm_main(parserState.ModuleStatements());
	}
	else
	{
		compile(parserState);
	}

	return EXIT_SUCCESS;
}