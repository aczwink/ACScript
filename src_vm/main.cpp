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
#include "VM.hpp"
#include "ExternalsManager.hpp"
#include "Module.hpp"

using namespace StdXX::FileSystem;

static void RunModule(const Path& path)
{
	ExternalsManager externalsManager;

	FileInputStream inputFile(path);
	Module module(inputFile, externalsManager);

	VM vm(module);
	vm.Run();
}

int32 Main(const String& programName, const FixedArray<String>& args)
{
	CommandLine::Parser commandLineParser(programName);

	commandLineParser.AddHelpOption();

	CommandLine::PathArgument inputPathArgument(u8"inputPath", u8"The compiled script path");
	commandLineParser.AddPositionalArgument(inputPathArgument);

	if(!commandLineParser.Parse(args))
	{
		commandLineParser.PrintHelp();
		return EXIT_FAILURE;
	}

	const CommandLine::MatchResult& cmdParseResult = commandLineParser.ParseResult();
	Path path = inputPathArgument.Value(cmdParseResult);

	RunModule(path);

	return EXIT_SUCCESS;
}