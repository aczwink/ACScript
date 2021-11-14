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
	| [< 'Token.Map; expr = parse_expression >] -> Ast.Function(lhs, expr)
	| [< 'Token.Identifier func; rhs = parse_primary_expression >] -> Ast.Call(Ast.Select(lhs, func), Ast.Tuple(lhs::rhs::[]))
	| [< >] -> lhs

and parse_expression =
	parser
	| [< lhs=parse_primary_expression; stream >] -> parse_binary_rhs lhs stream
	
let rec parse_object_type_declarations decls =
	parser
	| [< 'Token.Symbol '}' >] -> decls
	| [< 'Token.Identifier id; 'Token.Symbol ':'; typedef = parse_type; 'Token.Symbol ';'; stream>] -> (Ast.create_decl id typedef)::(parse_object_type_declarations decls stream)
	
and parse_tuple_type_inner entries =
	parser
	| [< 'Token.Symbol ')' >] -> entries
	| [< 'Token.Symbol ','; stream >] -> parse_tuple_type entries stream
	
and parse_tuple_type entries =
	parser
	| [< entry = parse_type; stream >] -> parse_tuple_type_inner (entry::entries) stream
	
and parse_type_inner =
	parser
	| [< 'Token.Symbol '{'; decls = parse_object_type_declarations [] >] -> Ast.ObjectType(decls)
	| [< 'Token.Symbol '('; entries = parse_tuple_type [] >] -> Ast.TupleType(entries)
	| [< 'Token.Identifier id >] -> Ast.NamedType id
	
and parse_type_function_or_single argType =
	parser
	| [< 'Token.Map; returnType = parse_type_inner >] -> Ast.FunctionType(argType, returnType)
	| [< >] -> argType
	
and parse_type =
	parser
	| [< argType = parse_type_inner; stream >] -> parse_type_function_or_single argType stream
;;

let parse_let_statement id =
	parser
	| [< 'Token.Symbol ':'; _ = parse_type; 'Token.Assignment; expr=parse_expression >] -> Ast.LetBindingStatement(id, expr)
	| [< 'Token.Assignment; expr=parse_expression >] -> Ast.LetBindingStatement(id, expr)
;;

let parse_statement =
	parser
	| [< 'Token.Let; 'Token.Identifier id; stream >] -> parse_let_statement id stream
	| [< 'Token.Type; 'Token.Identifier id; 'Token.Assignment; typedef = parse_type >] -> Ast.TypeDefStatement(id, typedef)
	| [< expr=parse_expression >] -> Ast.ExpressionStatement(expr)
;;

(*let parse_toplevel_error stream =
	match Stream.peek stream with
		| None -> []
		| Some c -> print_endline (Token.to_string c); raise (Stream.Error "Unexpected token")
;;*)

let rec parse_module =
	parser
	| [< stmt=parse_statement; 'Token.Symbol ';'; stream >] -> stmt::(parse_module stream)
	(*| [< stream >] -> parse_toplevel_error stream*)
	| [< >] -> []
;;
