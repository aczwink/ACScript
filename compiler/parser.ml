let rec parse_primary_expression = 
	parser
	| [< 'Token.NaturalLiteral n >] -> Ast.NaturalLiteral n
	| [< 'Token.StringLiteral n >] -> Ast.StringLiteral n
	
	| [< 'Token.Identifier id; stream >] ->
		let ident = Ast.Identifier(id) in
		let parse_identifier_inner =
			parser
			| [< 'Token.Symbol '('; arg=parse_expression; 'Token.Symbol ')' >] -> Ast.Call(ident, arg)
			| [< >] -> ident
		in
		parse_identifier_inner stream

and parse_binary_rhs lhs =
	parser
	| [< func = parse_primary_expression; rhs = parse_primary_expression >] -> Ast.Call(func, Ast.Tuple(lhs::rhs::[]))
	| [< >] -> lhs

and parse_expression =
	parser
	| [< lhs=parse_primary_expression; stream >] -> parse_binary_rhs lhs stream
;;

let parse_statement =
	parser
	| [< 'Token.Let; 'Token.Identifier id; 'Token.Assignment; expr=parse_expression >] -> Ast.LetBindingStatement(id, expr)
	| [< expr=parse_expression >] -> Ast.ExpressionStatement(expr)
;;

let rec parse_module =
	parser
	| [< stmt=parse_statement; stream=parse_module >] -> stmt::stream
	| [< >] -> []
;;
