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
%{
#include "../parser/includes.hpp"

extern int yylex();
extern int yyparse();
void yyerror(AST::ParserState* parserState, const char* s);
%}

%token TOKEN_ASSIGN
%token TOKEN_BRACE_OPEN
%token TOKEN_BRACE_CLOSE
%token TOKEN_COLON
%token TOKEN_COMMA
%token TOKEN_MAP
%token TOKEN_MORE
%token TOKEN_PAREN_OPEN
%token TOKEN_PAREN_CLOSE
%token TOKEN_SEMICOLON

%token TOKEN_KEYWORD_EXTERN
%token TOKEN_KEYWORD_NULL

%token TOKEN_IDENTIFIER
%token TOKEN_NATURAL_LITERAL
%token TOKEN_OPERATOR

%start module


%left TOKEN_OPERATOR
%left TOKEN_PAREN_OPEN



%union
{
    String* str;

    Expression* expr;
    ExpressionList* exprList;
    TupleExpression* tupleExpr;

    AST::External* external;

    Statement* stmt;
}
%type <str> TOKEN_IDENTIFIER
%type <str> TOKEN_NATURAL_LITERAL

%type <expr> expression
%type <expr> value_expression
%type <expr> tuple_call
%type <tupleExpr> tuple_expression
%type <exprList> tuple_expressions
%type <external> external
%type <stmt> statement

%parse-param { AST::ParserState* parserState }

%%

module:
    external                                                { parserState->AddExternal($1); }
    | statement                                             { parserState->ModuleStatements().AddStatement($1); }
    | statement module                                      { parserState->ModuleStatements().AddStatement($1); }
;

external:
    TOKEN_KEYWORD_EXTERN TOKEN_IDENTIFIER TOKEN_COLON typespec TOKEN_SEMICOLON
;

statement:
    tuple_call TOKEN_SEMICOLON                              { $$ = new ExpressionStatement($1); }
;





typespec:
    TOKEN_KEYWORD_NULL
    | TOKEN_IDENTIFIER
    | function_type
    | tuple_type
;

function_type:
    typespec TOKEN_MAP typespec
;

tuple_type_entry:
    typespec
    | typespec TOKEN_MORE
;

tuple_type:
    TOKEN_PAREN_OPEN tuple_type_entry TOKEN_PAREN_CLOSE
;






tuple_call:
    TOKEN_IDENTIFIER tuple_expression                       { $$ = new CallExpression(*$1, $2); }
;

tuple_expressions:
    expression                                              { $$ = new ExpressionList($1); }
    | expression TOKEN_COMMA tuple_expressions              { $3->InsertAtBeginning($1); $$ = $3; }
;

tuple_expression:
    TOKEN_PAREN_OPEN tuple_expressions TOKEN_PAREN_CLOSE    { $$ = new TupleExpression($2); }
;

expression:
    value_expression                                        { $$ = $1; }
;

value_expression:
    TOKEN_NATURAL_LITERAL                                   { $$ = new NaturalLiteralExpression(*$1); }
;








/*
statement:
    call TOKEN_SEMICOLON                                  { $$ = new ExpressionStatement($1); }
    //| TOKEN_IDENTIFIER TOKEN_ASSIGN expression TOKEN_SEMICOLON  { $$ = new AssignmentStatement($1, $3); }
;

expression:
    value_expression
    | expression TOKEN_OPERATOR expression //binary operator call
    | call
;

value_expression:
    TOKEN_IDENTIFIER
    | TOKEN_NATURAL_LITERAL
    | TOKEN_KEYWORD_FUNCTION TOKEN_BRACE_OPEN functionRules TOKEN_BRACE_CLOSE
;

functionRules:
    expression TOKEN_MAP expression
    | expression TOKEN_MAP expression TOKEN_COMMA functionRules
;*/

%%