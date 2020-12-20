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
%token TOKEN_GUARD
%token TOKEN_MAP
%token TOKEN_MORE
%token TOKEN_PAREN_OPEN
%token TOKEN_PAREN_CLOSE
%token TOKEN_SEMICOLON

%token TOKEN_PLUS
%token TOKEN_MINUS

%token TOKEN_KEYWORD_EXTERN
%token TOKEN_KEYWORD_LET

%token TOKEN_IDENTIFIER
%token TOKEN_NATURAL_LITERAL

%start module


%left TOKEN_PAREN_OPEN



%union
{
    String* str;

    AST::Expression* expr;
    AST::FunctionExpression* funcExpr;
    AST::ExpressionList* exprList;

    AST::LeftValue* lvalue;

    AST::Statement* stmt;

    AST::TypeSpec* type;
    AST::TupleTypeSpec* tupleTypeSpec;
}
%type <str> TOKEN_IDENTIFIER
%type <str> TOKEN_NATURAL_LITERAL
%type <str> function_name

%type <expr> expression
%type <expr> value_expression
%type <expr> function_call
%type <expr> function_expression
%type <funcExpr> functionRules
%type <expr> pattern
%type <expr> tuple_expression
%type <exprList> tuple_expressions
%type <stmt> statement

%type <type> typespec
%type <type> function_type
%type <type> tuple_type
%type <tupleTypeSpec> tuple_type_entries

%type <lvalue> left_value

%parse-param { AST::ParserState* parserState }

%%

module:
    statement                                                                     { parserState->ModuleStatements().AddStatement($1); }
    | statement module                                                              { parserState->ModuleStatements().AddStatement($1); }
;

statement:
    TOKEN_KEYWORD_EXTERN function_name TOKEN_COLON typespec TOKEN_SEMICOLON         { $$ = new AST::ExternalDeclarationStatement(*$2, $4); }
    | function_call TOKEN_SEMICOLON                                                    { $$ = new AST::ExpressionStatement($1); }
    | TOKEN_KEYWORD_LET left_value TOKEN_ASSIGN expression TOKEN_SEMICOLON          { $$ = new AST::VariableDefinitionStatement($2, $4); }
;





typespec:
    TOKEN_IDENTIFIER                                        { $$ = new AST::IdentifierTypeSpec(*$1); }
    | function_type                                         { $$ = $1; }
    | tuple_type                                            { $$ = $1; }
;

function_type:
    typespec TOKEN_MAP typespec                             { $$ = new AST::FunctionTypeSpec($1, $3); }
;

tuple_type_entries:
    typespec                                                { $$ = new AST::TupleTypeSpec($1); }
    | typespec TOKEN_MORE                                   { $$ = new AST::TupleTypeSpec($1, true); }
    | typespec TOKEN_COMMA tuple_type_entries               { $$ = $3; $3->AddTypeSpec($1); }
;

tuple_type:
    TOKEN_PAREN_OPEN tuple_type_entries TOKEN_PAREN_CLOSE     { $$ = $2; }
;





function_name:
    TOKEN_IDENTIFIER                                                { $$ = $1; }
    | TOKEN_PLUS                                                    { $$ = parserState->CreateString(u8"+"); }
    | TOKEN_MINUS                                                   { $$ = parserState->CreateString(u8"-"); }
;



function_call:
    TOKEN_IDENTIFIER TOKEN_PAREN_OPEN expression TOKEN_PAREN_CLOSE                       { $$ = new AST::CallExpression(*$1, $3); }
;




expression:
    value_expression                                                    { $$ = $1; }
    | TOKEN_IDENTIFIER                                                  { $$ = new AST::IdentifierExpression(*$1); }
    | function_call                                                     { $$ = $1; }
    | expression function_name expression /*infix notation call*/       { $$ = new AST::CallExpression(*$2, new AST::TupleExpression(new AST::ExpressionList($1, $3))); }
;

value_expression:
    TOKEN_NATURAL_LITERAL                                           { $$ = new AST::NaturalLiteralExpression(*$1); }
    | function_expression                                           { $$ = $1; }
    | tuple_expression                                              { $$ = $1; }
;


function_expression:
    pattern TOKEN_MAP expression                                    { $$ = new AST::FunctionExpression($1, nullptr, $3); }
    | TOKEN_BRACE_OPEN functionRules TOKEN_BRACE_CLOSE              { $$ = $2; }
;

functionRules:
    pattern TOKEN_MAP expression                                                        { $$ = new AST::FunctionExpression($1, nullptr, $3); }
    | pattern TOKEN_MAP expression TOKEN_COMMA functionRules                            { $$ = $5; $5->AddRule($1, nullptr, $3); }
    | pattern TOKEN_GUARD expression TOKEN_MAP expression TOKEN_COMMA functionRules     { $$ = $7; $7->AddRule($1, $3, $5); }
;


tuple_expressions:
    expression                                              { $$ = new AST::ExpressionList($1); }
    | expression TOKEN_COMMA tuple_expressions              { $3->InsertAtBeginning($1); $$ = $3; }
;

tuple_expression:
    TOKEN_PAREN_OPEN tuple_expressions TOKEN_PAREN_CLOSE            { $$ = new AST::TupleExpression($2); }
;




left_value:
    TOKEN_IDENTIFIER                                                { $$ = new AST::IdentifierLeftValue(*$1); }
;

pattern:
    TOKEN_NATURAL_LITERAL                                           { $$ = new AST::NaturalLiteralExpression(*$1); }
    | TOKEN_IDENTIFIER                                              { $$ = new AST::IdentifierExpression(*$1); }
;

%%