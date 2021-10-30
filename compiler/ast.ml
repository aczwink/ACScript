type expression =
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	| Call of expression * expression
	| Tuple of expression list
	| Function of expression * expression
	
type statement = 
	| ExpressionStatement of expression
	| LetBindingStatement of string * expression

	
let rec to_string toplevels =
	match toplevels with
	| [] -> ""
	| toplevel::rest -> (stmt_to_string toplevel) ^ "\n" ^ (to_string rest)
	
and stmt_to_string stmt =
	match stmt with
	| ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	| LetBindingStatement (id, expr) -> "let " ^ id ^ ": nat" ^ " := " ^ (expr_to_string expr) ^ ";"
	
and expr_to_string expr = match expr with
	| Identifier id -> id
	| NaturalLiteral x -> x
	| StringLiteral x -> "\"" ^ x ^ "\""
	| Call (func, arg) -> (expr_to_string func) ^ "(" ^ expr_to_string arg ^ ")"
	| Tuple values -> "(" ^ (exprs_to_string values) ^ ")"
	| Function(pattern, expr) -> expr_to_string pattern ^ " -> " ^ expr_to_string expr

and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)
