let inline_objects_that_are_used_once (_module: Semantic_ast.program_module) =
	let rec try_simplify_expr expr =
		match expr with
		| Semantic_ast.Self -> expr
		| Semantic_ast.Identifier _ -> expr
		| Semantic_ast.NaturalLiteral _ -> expr
		| Semantic_ast.StringLiteral _ -> expr
		| Semantic_ast.UnsignedLiteral _ -> expr
		| Semantic_ast.External _ -> expr
		| Semantic_ast.Function (globalName, rules) -> Semantic_ast.Function (globalName, List.map (map_rule) rules)
		| Semantic_ast.Call (func, arg) -> Semantic_ast.Call (try_simplify_expr func, try_simplify_expr arg)
		| Semantic_ast.Object entries -> Semantic_ast.Object (List.map (map_object_entry) entries)
		| Semantic_ast.Select (expr, member) ->
			let newExpr = try_simplify_expr expr in
			if Symbol_evaluator.is_object newExpr
			then Symbol_evaluator.eval_select newExpr member (Hashtbl.create 10)
			else Semantic_ast.Select (newExpr, member)
		| Semantic_ast.Tuple entries -> Semantic_ast.Tuple (List.map (try_simplify_expr) entries)
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
		
	and map_object_entry entry =
		let mapped_entry: Semantic_ast.object_entry = { name = entry.name; expr = try_simplify_expr entry.expr } in
		mapped_entry
		
	and map_rule rule =
		let try_simplify_cond cond =
			match cond with
			| None -> None
			| Some x -> Some (try_simplify_expr x)
		in
		let mapped_rule: Semantic_ast.function_rule = { pattern = rule.pattern; condition = try_simplify_cond rule.condition; body = try_simplify_expr rule.body } in
		mapped_rule
	in
	
	let inline_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr -> Semantic_ast.ExpressionStatement (try_simplify_expr expr)
		| Semantic_ast.LetBindingStatement (name, typedef, expr) -> Semantic_ast.LetBindingStatement (name, typedef, try_simplify_expr expr)
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.stmt_to_string stmt))
	in
	
	let mapped_statements = List.map (inline_stmt) _module.statements in
	let mapped_module: Semantic_ast.program_module = { moduleName = _module.moduleName; statements = mapped_statements; exports = _module.exports} in
	mapped_module
;;
