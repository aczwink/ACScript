let translate_ast ast =
	let namedValues:(string, Ir.instruction) Hashtbl.t = Hashtbl.create 10
	in
	
	let rec translate_call func arg =
		match func with
		| Ast.Identifier externalName -> Ir.CallExternalInstruction(externalName, arg)
		| _ -> raise (Stream.Error "call not implemented") 
		
	and translate_expr expr =
		match expr with
		| Ast.Identifier identifier -> Hashtbl.find namedValues identifier
		| Ast.NaturalLiteral f -> Ir.LoadConstantInstruction(Ir.ConstantNatural(f))
		| Ast.StringLiteral x -> Ir.LoadConstantInstruction(Ir.ConstantString(x))
		| Ast.Call (func, arg) -> translate_call func (translate_expr arg)
		| Ast.Tuple args -> Ir.NewTupleInstruction(translate_exprs args)
		| _ -> raise (Stream.Error "call not implemented")
	and translate_exprs exprs =
		match exprs with
		| [] -> []
		| expr::rest -> (translate_expr expr) :: (translate_exprs rest)
	in
	
	let translate_stmt stmt = 
		match stmt with
		| Ast.ExpressionStatement expr -> translate_expr expr
		| Ast.LetBindingStatement (name, expr) ->
			let value = translate_expr expr
			in
				Hashtbl.add namedValues name value;
				value
		| _ -> raise (Stream.Error "call not implemented")
	in
	
	let rec translate_stmts ast =
		match ast with
		| [] -> []
		| stmt::rest ->
			let first = (translate_stmt stmt) in
				first::(translate_stmts rest)
	in
	
	translate_stmts ast
