let translate_ast ast =	
	let rec translate_expr expr =
		match expr with
		| Ast.Identifier _ -> raise (Stream.Error "Unimplemented")
		| Ast.NaturalLiteral f -> Ir.LoadConstantInstruction(Ir.ConstantNatural(f))
		| Ast.StringLiteral x -> Ir.LoadConstantInstruction(Ir.ConstantString(x))
		| Ast.Call (externalName, args) -> Ir.CallExternalInstruction(externalName, List.hd(translate_exprs args))
	and translate_exprs exprs =
		match exprs with
		| [] -> []
		| expr::[] -> [translate_expr expr]
		| _::rest -> translate_exprs rest
	in
	
	let translate_stmts ast = 
		match ast with
		| Ast.ExpressionStatement expr -> translate_expr expr
		| Ast.LetBindingStatement (_, expr) -> translate_expr expr
	in
	
	translate_stmts ast
