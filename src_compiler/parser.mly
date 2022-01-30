%token END_OF_STREAM
%token KEYWORD_EXTERN
%token KEYWORD_LET
%token KEYWORD_TYPE
%token KEYWORD_USE
%token SYMBOL_ASSIGNMENT
%token SYMBOL_COLON
%token SYMBOL_COMMA
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
	| SYMBOL_LEFT_BRACE rules=separated_list(SYMBOL_COMMA, function_rule) SYMBOL_RIGHT_BRACE			{ Ast.Function( rules ) }
	| id=IDENTIFIER																						{ Ast.Identifier(id) }
	| literal=NATURAL_LITERAL																			{ Ast.NaturalLiteral literal }
	| literal=STRING_LITERAL																			{ Ast.StringLiteral literal }
	| KEYWORD_EXTERN name=STRING_LITERAL																{ Ast.External name }
	| func=expression SYMBOL_LEFT_PARENTHESIS arg=expression SYMBOL_RIGHT_PARENTHESIS					{ Ast.Call(func, arg) }
	| lhs=expression id=IDENTIFIER rhs=expression														{ Ast.BinaryInfixCall( lhs, id, rhs ) }
	| SYMBOL_LEFT_PARENTHESIS entries=separated_list(SYMBOL_COMMA, expression) SYMBOL_RIGHT_PARENTHESIS	{ Ast.Tuple entries }
	
pattern:
	| expr=expression																					{ expr }


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
