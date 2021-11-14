type expression =
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	| Call of expression * expression
	| Select of expression * string
	| Tuple of expression list
	| Function of expression * expression
	
type declaration = {
	name: string;
	typedef: typedefinition
}
	
and typedefinition =
	| NamedType of string
	| FunctionType of typedefinition * typedefinition
	| ObjectType of declaration list
	| TupleType of typedefinition list
	
type statement = 
	| ExpressionStatement of expression
	| LetBindingStatement of string * expression
	| TypeDefStatement of string * typedefinition


let create_decl name typedef = { name = name; typedef = typedef }

	
let rec to_string toplevels =
	match toplevels with
	| [] -> ""
	| toplevel::rest -> (stmt_to_string toplevel) ^ "\n" ^ (to_string rest)
	
and declaration_to_string decl =
	decl.name ^ ": " ^ (typedef_to_string decl.typedef) ^ ";"
	
and typedef_to_string typedef =
	match typedef with
	| NamedType name -> name
	| FunctionType (argType, returnType) -> (typedef_to_string argType) ^ " -> " ^ (typedef_to_string returnType)
	| TupleType entries -> "(" ^ (String.concat ", " (List.map (typedef_to_string) entries)) ^ ")"
	| ObjectType decls -> "{" ^ (String.concat ", " (List.map (declaration_to_string) decls)) ^ "}"
	
and stmt_to_string stmt =
	match stmt with
	| ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	| LetBindingStatement (id, expr) -> "let " ^ id ^ ": nat" ^ " := " ^ (expr_to_string expr) ^ ";"
	| TypeDefStatement (id, typedef) -> "type " ^ id ^ " := " ^ (typedef_to_string typedef) ^ ";"
	
and expr_to_string expr = match expr with
	| Identifier id -> id
	| NaturalLiteral x -> x
	| StringLiteral x -> "\"" ^ x ^ "\""
	| Call (func, arg) -> (expr_to_string func) ^ "(" ^ expr_to_string arg ^ ")"
	| Select (expr, member) -> (expr_to_string expr) ^ "." ^ member
	| Tuple values -> "(" ^ (exprs_to_string values) ^ ")"
	| Function(pattern, expr) -> expr_to_string pattern ^ " -> " ^ expr_to_string expr

and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)
