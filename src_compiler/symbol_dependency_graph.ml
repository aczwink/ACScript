let compute (_module: Semantic_ast.program_module) =
	let dependencies: (string, int) Hashtbl.t = Hashtbl.create 10 in
	
	let add_ref name =
		Hashtbl.replace dependencies name ((Hashtbl.find dependencies name) +1)
	in
	
	let rec process_pattern pattern =
		match pattern with
		| Semantic_ast.Identifier id ->
			if Hashtbl.mem dependencies id then ()
			else Hashtbl.add dependencies id 0
		| Semantic_ast.NaturalLiteral _ -> ()
		| Semantic_ast.Tuple exprs -> List.iter (process_pattern) exprs
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string pattern))
	in
	
	let rec process_expr expr =
		match expr with
		| Semantic_ast.Self -> ()
		| Semantic_ast.Identifier id -> add_ref id
		| Semantic_ast.NaturalLiteral _ -> ()
		| Semantic_ast.StringLiteral _ -> ()
		| Semantic_ast.UnsignedLiteral _ -> ()
		| Semantic_ast.External _ -> ()
		| Semantic_ast.Call (func, arg) -> process_expr func; process_expr arg
		| Semantic_ast.Function (_, rules) -> List.iter (process_rule) rules
		| Semantic_ast.Object decls -> List.iter (process_decl) decls
		| Semantic_ast.Select (expr, _) -> process_expr expr
		| Semantic_ast.Tuple exprs -> List.iter (process_expr) exprs
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
		
	and process_decl decl = process_expr decl.expr
	
	and process_rule rule = process_pattern rule.pattern; process_expr rule.body
	in
	
	let process_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr -> process_expr expr
		| Semantic_ast.LetBindingStatement (name, _, expr) -> Hashtbl.add dependencies name 0; process_expr expr
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.stmt_to_string stmt))
	in
	
	List.iter (process_stmt) _module.statements;
	dependencies
;;
