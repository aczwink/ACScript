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
#include "acsb/Compiler.hpp"
#include "acsb/VM.hpp"
#include "ast/Parser.hpp"

static String FormatTime(uint64 time)
{
	if(time < 1000)
		return String::Number(time) + u8" ns";

	if(time < 1000000)
		return String::Number(time / 1000) + u8" µs";

	if (time < 1000000000)
		return String::Number(time / 1000000) + u8" ms";

	return String::Number(time / 1e9, 3) + u8" s";
}

int32 Main(const String& programName, const FixedArray<String>& args)
{
	//Read
	FileSystem::Path inputFilePath = String(u8"../examples/containers.acs");
	FileInputStream inputFile(inputFilePath);

	Clock c;

	//Parse
	c.Start();
	Parser parser(inputFile);
	UniquePointer<StatementBlock> parsedModule = parser.Parse();
	stdOut << u8"Parsing time: " << FormatTime(c.GetElapsedNanoseconds()) << endl;

	//compile to IR
	c.Start();
	IR::Program program;
	parsedModule->Compile(program.GetMainProcedure().GetBlock());
	stdOut << u8"Compile to IR time: " << FormatTime(c.GetElapsedNanoseconds()) << endl;
	stdOut << u8"IR code: " << endl
		<< program.ToString() << endl;

	//Verify IR
	c.Start();
	try
	{
		program.Verify();
	}
	catch (const String& string)
	{
		stdErr << string << endl;
		return EXIT_FAILURE;
	}
	stdOut << u8"IR verifying time: " << FormatTime(c.GetElapsedNanoseconds()) << endl;

	/*
	//Optimize
	//TODO
	*/

	//Compile to acs bytecode
	c.Start();
	ACSB::Compiler compiler(program);
	program.Compile(compiler);
	ACSB::Module mod = compiler.GetCompiledModule();
	stdOut << u8"Compile to acs bytecode time: " << FormatTime(c.GetElapsedNanoseconds()) << endl;

	//execute bytecode
	c.Start();
	vm.WaitForTaskCompletion();
	uint64 t_exec = c.GetElapsedNanoseconds();
	
	stdOut << u8"Script execution ended." << endl;
		//<< u8"Execution time: " << FormatTime(t_exec) << endl;
	
	return EXIT_SUCCESS;
}