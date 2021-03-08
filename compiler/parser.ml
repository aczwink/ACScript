let rec parse_expression = 
	parser
	| [< 'Token.Number n >] -> Ast.Number n
	| [< 'Token.Identifier id; 'Token.Symbol '('; arg=parse_expression; 'Token.Symbol ')' >] -> Ast.Call(id, [arg])
;;

let parse_module =
	parser
	| [< expr=parse_expression >] -> Ast.ExpressionStatement(expr)
