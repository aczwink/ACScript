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
//Class header
#include "Parser.hpp"
//Global
#include <cstdio>
//Namespaces
using namespace AST;
ParserState* g_parserState = nullptr;

extern FILE * yyin;
extern int yylex();
extern int yyparse(ParserState* parserState);

void yyerror(ParserState* parserState, const char* s)
{
	stdOut << u8"ERROR: " << s << endl;
}

//Public methods
bool Parser::Parse(const FileSystem::Path& inputPath)
{
	yyin = fopen(reinterpret_cast<const char *>(inputPath.String().ToUTF8().GetRawZeroTerminatedData()), u8"rb");

	g_parserState = &this->parserState;

	/*int result = 1;
	while(result)
	{
		result = yylex();
		stdOut << result << endl;
	}*/

	int result = yyparse(g_parserState);
	fclose(yyin);

	return result == 0;
}