let rec parse_expression = 
	parser
	| [< 'Token.NaturalLiteral n >] -> Ast.NaturalLiteral n
	| [< 'Token.StringLiteral n >] -> Ast.StringLiteral n
	
	| [< 'Token.Identifier id; stream >] ->
		let parse_prefix_call_or_ident =
			parser
			| [< 'Token.Symbol '('; arg=parse_expression; 'Token.Symbol ')' >] -> Ast.Call(id, [arg])
			| [< >] -> Ast.Identifier(id)
		in parse_prefix_call_or_ident stream
;;

let parse_module =
	parser
	| [< 'Token.Let; 'Token.Identifier id; 'Token.Assignment; expr=parse_expression >] -> Ast.LetBindingStatement(id, expr)
	| [< expr=parse_expression >] -> Ast.ExpressionStatement(expr)
