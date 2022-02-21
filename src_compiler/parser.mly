%token END_OF_STREAM
%token KEYWORD_EXTERN
%token KEYWORD_LET
%token KEYWORD_TYPE
%token KEYWORD_USE
%token SYMBOL_ASSIGNMENT
%token SYMBOL_COLON
%token SYMBOL_COMMA
%token SYMBOL_DOT
%token SYMBOL_LEFT_BRACE
%token SYMBOL_LEFT_PARENTHESIS
%token SYMBOL_MAP
%token SYMBOL_PIPE
%token SYMBOL_RIGHT_BRACE
%token SYMBOL_RIGHT_PARENTHESIS
%token SYMBOL_SEMICOLON

%token <string> IDENTIFIER
%token <string> NATURAL_LITERAL
%token <string> STRING_LITERAL
%token <string> UNSIGNED_LITERAL

%type <string * Ast.expression> dict_entry
%type <Ast.statement list> program_module
%type <Ast.typedefinition> typedef

%start program_module

%%


program_module:
	| toplevels=list(toplevels); END_OF_STREAM { toplevels }

toplevel:
	| KEYWORD_LET id=IDENTIFIER SYMBOL_ASSIGNMENT expr=expression { Ast.LetBindingStatement(id, None, expr) }
	| KEYWORD_LET id=IDENTIFIER SYMBOL_COLON td=typedef SYMBOL_ASSIGNMENT expr=expression { Ast.LetBindingStatement(id, Some td, expr) }
	| KEYWORD_TYPE id=IDENTIFIER SYMBOL_ASSIGNMENT td=typedef { Ast.TypeDefStatement(id, td) }
	| KEYWORD_USE moduleName=STRING_LITERAL { Ast.UseStatement(moduleName) }
	| expr=expression { Ast.ExpressionStatement(expr) }
	
toplevels:
	| tl=toplevel SYMBOL_SEMICOLON { tl }
	
expression:
	| pt=pattern SYMBOL_MAP expr=expression																{ Ast.Function( [(pt, None, expr)] ) }
	| pt=pattern																						{ Ast.pattern_to_expr pt }
	| expr=value_expression																				{ expr }
	| SYMBOL_LEFT_PARENTHESIS expr=expression SYMBOL_RIGHT_PARENTHESIS									{ expr }
	| KEYWORD_EXTERN name=STRING_LITERAL																{ Ast.External name }
	| SYMBOL_LEFT_BRACE res=dict_or_func SYMBOL_RIGHT_BRACE												{ res }
	| func=expression SYMBOL_LEFT_PARENTHESIS arg=expression SYMBOL_RIGHT_PARENTHESIS					{ Ast.Call(func, arg) }
	| lhs=expression id=IDENTIFIER rhs=expression														{ Ast.BinaryInfixCall( lhs, id, rhs ) }
	| expr=expression SYMBOL_DOT id=IDENTIFIER															{ Ast.Select(expr, id) }
	
value_expression:
	| literal=NATURAL_LITERAL																			{ Ast.NaturalLiteral literal }
	| literal=UNSIGNED_LITERAL																			{ Ast.UnsignedLiteral literal }
	| literal=STRING_LITERAL																			{ Ast.StringLiteral literal }
	
pattern:
	| id=IDENTIFIER																															{ Ast.IdentifierPattern (id, None) }
	| SYMBOL_LEFT_PARENTHESIS firstEntry=pattern_tuple_entry SYMBOL_COMMA entries=separated_list(SYMBOL_COMMA, pattern_tuple_entry) SYMBOL_RIGHT_PARENTHESIS	{ Ast.TuplePattern (firstEntry::entries) }
	
pattern_tuple_entry:
	| id=IDENTIFIER																															{ Ast.IdentifierPattern (id, None) }
	| id=IDENTIFIER SYMBOL_COLON td=typedef																									{ Ast.IdentifierPattern (id, Some td) }
	| expr=value_expression																													{ Ast.ExprPattern expr }
	
	
dict_or_func:
	| firstEntry=dict_entry SYMBOL_COMMA dictEntries=separated_list(SYMBOL_COMMA, dict_entry)			{ Ast.Dictionary (firstEntry::dictEntries) }
	| rules=separated_list(SYMBOL_COMMA, function_rule)													{ Ast.Function( rules ) }
	
	
dict_entry:
	| id=IDENTIFIER SYMBOL_COLON expr=expression														{ (id, expr) }


function_rule:
	| pt=pattern SYMBOL_MAP expr=expression																{ (pt, None, expr) }
	| pt=pattern SYMBOL_PIPE cond=expression SYMBOL_MAP expr=expression									{ (pt, Some cond, expr) }
	
inner_typedef:
	| id=IDENTIFIER { Ast.NamedType id }
	| SYMBOL_LEFT_BRACE decls=list(object_type_decl) SYMBOL_RIGHT_BRACE { Ast.ObjectType(decls) }
	| SYMBOL_LEFT_PARENTHESIS entries=separated_list(SYMBOL_COMMA, typedef) SYMBOL_RIGHT_PARENTHESIS { Ast.TupleType(entries) }
	
typedef:
	| argType=inner_typedef SYMBOL_MAP returnType=inner_typedef { Ast.FunctionType(argType, returnType) }
	| td=inner_typedef { td }
	
object_type_decl:
	| id=IDENTIFIER SYMBOL_COLON td=typedef SYMBOL_SEMICOLON { Ast.create_decl id td }
