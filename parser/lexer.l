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
%option noyywrap

%{
#include "../parser/includes.hpp"
#include "parser.hpp"

extern AST::ParserState* g_parserState;
%}

%%
[ \t\r\n]       ; //ignore whitespaces
"//".*      {} //skip comments

","         { return TOKEN_COMMA; }
":"         { return TOKEN_COLON; }
";"         { return TOKEN_SEMICOLON; }
":="        { return TOKEN_ASSIGN; }
"->"        { return TOKEN_MAP; }
"|"         { return TOKEN_GUARD; }
"("         { return TOKEN_PAREN_OPEN; }
")"         { return TOKEN_PAREN_CLOSE; }
"{"         { return TOKEN_BRACE_OPEN; }
"}"         { return TOKEN_BRACE_CLOSE; }
"..."       { return TOKEN_MORE; }

"+"         { return TOKEN_PLUS; }
"-"         { return TOKEN_MINUS; }

"extern"    { return TOKEN_KEYWORD_EXTERN; }
"let"       { return TOKEN_KEYWORD_LET; }

[0-9]+              { yylval.str = g_parserState->CreateString(yytext); return TOKEN_NATURAL_LITERAL; }
[a-zA-Z0-9\-\*<=]+  { yylval.str = g_parserState->CreateString(yytext); return TOKEN_IDENTIFIER; }
%%