type expression =
	| Number of float
	| Call of string * expression list
	
type statement = 
	| ExpressionStatement of expression

	
let rec to_string toplevel = stmt_to_string toplevel
	
and stmt_to_string stmt =
	match stmt with
	| ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	
and expr_to_string expr = match expr with
	| Number x -> Printf.sprintf "%f" x
	| Call (id, arg) -> id ^ "(" ^ exprs_to_string arg ^ ")"

and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)
