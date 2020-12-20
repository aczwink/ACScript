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

extern const char* yytext;
extern int yylineno;
extern int yylex();
extern int yyparse(ParserState* parserState);

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_buffer ( char *base, size_t size  );
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

void yyerror(ParserState* parserState, const char* s)
{
	stdOut << u8"ERROR: " << s << u8" token: " << yytext << u8" in line: " << yylineno << endl;
}

//Public methods
bool Parser::Finish()
{
	DataWriter dataWriter(true, this->buffer);
	dataWriter.WriteByte(0);
	dataWriter.WriteByte(0);

	g_parserState = &this->parserState;

	FixedSizeBuffer buffer(this->buffer.GetRemainingBytes());
	this->buffer.ReadBytes(buffer.Data(), buffer.Size());

	YY_BUFFER_STATE state = yy_scan_buffer(reinterpret_cast<char *>(buffer.Data()), buffer.Size());
	int result = yyparse(g_parserState);
	/*int result = 1;
	while(result)
	{
		result = yylex();
		stdOut << result << endl;
	}*/
	yy_delete_buffer(state);

	return result == 0;
}

bool Parser::Parse(const FileSystem::Path& inputPath)
{
	FileInputStream fileInputStream(inputPath);
	fileInputStream.FlushTo(this->buffer);

	return true;
}