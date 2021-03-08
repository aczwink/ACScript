let translate_ast ast =	
	let rec translate_expr expr =
		match expr with
		| Ast.Number f -> Ir.LoadConstantInstruction(Ir.ConstantFloat(f))
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
	in
	
	translate_stmts ast
