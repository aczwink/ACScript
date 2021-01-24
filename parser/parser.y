/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
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
%token TOKEN_DOT
%token TOKEN_GUARD
%token TOKEN_MAP
%token TOKEN_MORE
%token TOKEN_MULTIPLY
%token TOKEN_PAREN_OPEN
%token TOKEN_PAREN_CLOSE
%token TOKEN_SEMICOLON

%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_EQUALS

%token TOKEN_KEYWORD_EXTERN
%token TOKEN_KEYWORD_LET
%token TOKEN_KEYWORD_TYPE

%token TOKEN_IDENTIFIER
%token TOKEN_NATURAL_LITERAL

%start module



%union
{
    String* str;

    AST::Expression* expr;
    AST::FunctionExpression* funcExpr;
    AST::ObjectExpression* objExpr;
    AST::ExpressionList* exprList;

    AST::LeftValue* lvalue;

    AST::Statement* stmt;
    AST::StatementBlock* stmtBlock;

    AST::TypeSpec* type;
    AST::TupleTypeSpec* tupleTypeSpec;
}
%type <str> TOKEN_IDENTIFIER
%type <str> TOKEN_NATURAL_LITERAL
%type <str> function_name

%type <expr> expression
%type <expr> expression_without_function_value
%type <expr> value_expression

%type <funcExpr> function_rule_leaf
%type <funcExpr> function_rules

%type <objExpr> object_entry
%type <objExpr> object_entries

%type <exprList> tuple_entries

%type <stmt> statement
%type <stmt> declaration_statement
%type <stmtBlock> declaration_statements

%type <type> typespec
%type <type> function_type
%type <tupleTypeSpec> tuple_type
%type <tupleTypeSpec> tuple_type_entries

%type <lvalue> left_value

%parse-param { AST::ParserState* parserState }

%left TOKEN_PLUS
%left TOKEN_MULTIPLY

%left TOKEN_IDENTIFIER
%left TOKEN_MAP

%%
module:
    statement                                                                                                           { parserState->ModuleStatements().AddStatement($1); }
    | statement module                                                                                                  { parserState->ModuleStatements().AddStatement($1); }
;

statement:
    declaration_statement                                                                                               { $$ = $1; }
    | TOKEN_KEYWORD_EXTERN TOKEN_KEYWORD_TYPE TOKEN_IDENTIFIER TOKEN_ASSIGN
        TOKEN_BRACE_OPEN declaration_statements TOKEN_BRACE_CLOSE TOKEN_SEMICOLON                                       { $$ = new AST::TypeDeclarationStatement($6);  }
    | TOKEN_KEYWORD_LET left_value TOKEN_ASSIGN expression TOKEN_SEMICOLON                                              { $$ = new AST::VariableDefinitionStatement($2, $4); }
    | expression TOKEN_SEMICOLON                                                                                        { $$ = new AST::ExpressionStatement($1); }
;



declaration_statement:
    TOKEN_KEYWORD_EXTERN function_name TOKEN_COLON typespec TOKEN_SEMICOLON                                             { $$ = new AST::ExternalDeclarationStatement(*$2, $4); }
;

declaration_statements:
    declaration_statement                                                                                               { $$ = new AST::StatementBlock(); $$->AddStatement($1); }
    | declaration_statement declaration_statements                                                                      { $$ = $2; $2->AddStatement($1); }
;


function_name:
    TOKEN_IDENTIFIER                                                { $$ = $1; }
    | TOKEN_PLUS                                                    { $$ = parserState->CreateString(u8"+"); }
    | TOKEN_MINUS                                                   { $$ = parserState->CreateString(u8"-"); }
    | TOKEN_MULTIPLY                                                { $$ = parserState->CreateString(u8"*"); }
    | TOKEN_EQUALS                                                  { $$ = parserState->CreateString(u8"="); }
;

left_value:
    TOKEN_IDENTIFIER                                                { $$ = new AST::IdentifierLeftValue(*$1); }
;



expression:
    function_rule_leaf                                                                                                  { $$ = $1; }
    | TOKEN_BRACE_OPEN function_rules TOKEN_BRACE_CLOSE                                                                 { $$ = $2; }
    | expression_without_function_value                                                                                 { $$ = $1; }
;

expression_without_function_value:
    expression_without_function_value TOKEN_PAREN_OPEN TOKEN_PAREN_CLOSE                                                { $$ = new AST::CallExpression($1); }
    | expression_without_function_value TOKEN_PAREN_OPEN expression TOKEN_PAREN_CLOSE                                   { $$ = new AST::CallExpression($1, $3); }
    | expression_without_function_value TOKEN_DOT TOKEN_IDENTIFIER                                                      { $$ = new AST::SelectExpression($1, *$3); }
    | value_expression                                                                                                  { $$ = $1; }

    //infix
    | expression_without_function_value TOKEN_IDENTIFIER expression_without_function_value                              { $$ = new AST::CallExpression(new AST::IdentifierExpression(*$2), new AST::TupleExpression(new AST::ExpressionList($1, $3))); }
    | expression_without_function_value TOKEN_MULTIPLY expression_without_function_value                                { $$ = new AST::CallExpression(new AST::IdentifierExpression(u8"*"), new AST::TupleExpression(new AST::ExpressionList($1, $3))); }
    | expression_without_function_value TOKEN_PLUS expression_without_function_value                                    { $$ = new AST::CallExpression(new AST::IdentifierExpression(u8"+"), new AST::TupleExpression(new AST::ExpressionList($1, $3))); }
    | expression_without_function_value TOKEN_EQUALS expression_without_function_value                                  { $$ = new AST::CallExpression(new AST::IdentifierExpression(u8"="), new AST::TupleExpression(new AST::ExpressionList($1, $3))); }
;


value_expression:
    TOKEN_PAREN_OPEN tuple_entries TOKEN_PAREN_CLOSE                                                                    { $$ = new AST::TupleExpression($2); }
    | TOKEN_BRACE_OPEN object_entries TOKEN_BRACE_CLOSE                                                                 { $$ = $2; }
    | TOKEN_NATURAL_LITERAL                                                                                             { $$ = new AST::NaturalLiteralExpression(*$1); }
    | TOKEN_IDENTIFIER                                                                                                  { $$ = new AST::IdentifierExpression(*$1); }
;


function_rule_leaf:
    value_expression TOKEN_MAP expression_without_function_value                                                        { $$ = new AST::FunctionExpression($1, nullptr, $3); }
    | value_expression TOKEN_GUARD expression_without_function_value TOKEN_MAP expression_without_function_value        { $$ = new AST::FunctionExpression($1, $3, $5); }
;

function_rules:
    function_rule_leaf                                                                  { $$ = $1; }
    | function_rule_leaf TOKEN_COMMA function_rules                                      { $$ = $3; $3->CombineRulesAndPrepend($1); }
;


tuple_entries:
    expression                                              { $$ = new AST::ExpressionList($1); }
    | expression TOKEN_COMMA tuple_entries              { $3->InsertAtBeginning($1); $$ = $3; }
;


object_entry:
    function_name                                                                                                       { $$ = new AST::ObjectExpression(*$1); }
    | function_name TOKEN_COLON expression                                                                              { $$ = new AST::ObjectExpression(*$1, $3); }
;

object_entries:
    object_entry                                                                                                        { $$ = $1; }
    | object_entry TOKEN_COMMA object_entries                                                                           { $$ = $3; $3->AddMember($1); }
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

%%