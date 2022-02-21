type declaration = {
	name: string;
	typedef: typedefinition
}
	
and typedefinition =
	| NamedType of string
	| FunctionType of typedefinition * typedefinition
	| ObjectType of declaration list
	| TupleType of typedefinition list
	
type pattern =
	| ExprPattern of expression
	| IdentifierPattern of string * typedefinition option
	| TuplePattern of pattern list

and expression =
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	| UnsignedLiteral of string
	| External of string
	| Call of expression * expression
	| BinaryInfixCall of expression * string * expression
	| Dictionary of (string * expression) list
	| Function of (pattern * expression option * expression) list
	| Select of expression * string
	| Tuple of expression list


type statement =
	| ExpressionStatement of expression
	| LetBindingStatement of string * typedefinition option * expression
	| TypeDefStatement of string * typedefinition
	| UseStatement of string
	
	
	
	

let create_decl name typedef = { name = name; typedef = typedef }
	
let rec declaration_to_string decl =
	"\t" ^ decl.name ^ ": " ^ (typedef_to_string decl.typedef) ^ ";"
	
and typedef_to_string typedef =
	match typedef with
	| NamedType name -> name
	| FunctionType (argType, returnType) -> (typedef_to_string argType) ^ " -> " ^ (typedef_to_string returnType)
	| TupleType entries -> "(" ^ (String.concat ", " (List.map (typedef_to_string) entries)) ^ ")"
	| ObjectType decls -> "\n{\n" ^ (String.concat "\n" (List.map (declaration_to_string) decls)) ^ "\n}"
;;

let optional_typedef_to_string typedef_opt =
	match typedef_opt with
	| Some typedef -> ": " ^ (typedef_to_string typedef)
	| None -> ""
;;
	
let rec to_string toplevels =
	match toplevels with
	| [] -> ""
	| toplevel::rest -> (statement_to_string toplevel) ^ "\n" ^ (to_string rest)
	
and expr_to_string expr =
	match expr with
	| Identifier id -> id
	| NaturalLiteral x -> x
	| StringLiteral x -> "\"" ^ x ^ "\""
	| UnsignedLiteral x -> x ^ "u"
	| External externalName -> "extern \"" ^ externalName ^ "\""
	| Call (func, arg) -> (expr_to_string func) ^ "(" ^ expr_to_string arg ^ ")"
	| BinaryInfixCall (lhs, op, rhs) -> (expr_to_string lhs) ^ " " ^ op ^ " " ^ (expr_to_string rhs)
	| Dictionary entries -> "{\n" ^ (String.concat "\n" (List.map (dict_entry_to_string) entries)) ^ "\n}"
	| Function(rules) -> "{\n" ^ (String.concat ",\n" (List.map (rule_to_string) rules)) ^ "\n}"
	| Select (expr, member) -> (expr_to_string expr) ^ "." ^ member
	| Tuple values -> "(" ^ (exprs_to_string values) ^ ")"
	
and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)

and dict_entry_to_string (key, value) = key ^ ": " ^ expr_to_string value

and pattern_to_string pattern =
	match pattern with
	| ExprPattern expr -> expr_to_string expr
	| IdentifierPattern (id, typedef_opt) -> id ^ optional_typedef_to_string typedef_opt
	| TuplePattern entries -> "(" ^ (patterns_to_string entries) ^ ")"
	
and patterns_to_string patterns = String.concat ", " (List.map (pattern_to_string) patterns)

and rule_to_string (pattern, cond, expr) =
	let cond_to_string =
		match cond with
		| None -> ""
		| Some x -> " | " ^ expr_to_string x
	in
	"\t" ^ pattern_to_string pattern ^ cond_to_string ^ " -> " ^ expr_to_string expr
	
and statement_to_string stmt =
	match stmt with
	| ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	| LetBindingStatement (id, typedef, expr) -> "let " ^ id ^ (optional_typedef_to_string typedef) ^ " := " ^ (expr_to_string expr) ^ ";"
	| TypeDefStatement (id, typedef) -> "type " ^ id ^ " := " ^ (typedef_to_string typedef) ^ ";"
	| UseStatement id -> "use \"" ^ id ^ "\";"
	
	
	
	
let rec pattern_to_expr pattern =
	match pattern with
	| ExprPattern expr -> expr
	| IdentifierPattern (name, None) -> Identifier name
	| IdentifierPattern (_, Some _) -> raise (Stream.Error "Expected expression and not pattern")
	| TuplePattern entries -> Tuple (List.map (pattern_to_expr) entries)
;;
