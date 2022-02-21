let inline_symbols_that_are_only_used_once _module =
	let sym_dependencies = Symbol_dependency_graph.compute _module in
	let sym_values = Symbol_evaluator.compute _module in
	
	let rec map_expr expr =
		match expr with
		| Semantic_ast.Self -> expr
		| Semantic_ast.Identifier id -> map_id id
		| Semantic_ast.NaturalLiteral _ -> expr
		| Semantic_ast.StringLiteral _ -> expr
		| Semantic_ast.UnsignedLiteral _ -> expr
		| Semantic_ast.External _ -> expr
		| Semantic_ast.Call (func, arg) -> Semantic_ast.Call (map_expr func, map_expr arg)
		| Semantic_ast.Function (globalName, rules) -> Semantic_ast.Function (globalName, List.map (map_rule) rules)
		| Semantic_ast.Dictionary entries -> Semantic_ast.Dictionary (List.map (map_object_entry) entries)
		| Semantic_ast.Select (expr, member) -> Semantic_ast.Select (map_expr expr, member)
		| Semantic_ast.Tuple entries -> Semantic_ast.Tuple (List.map (map_expr) entries)
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
		
	and map_id id =
		match Hashtbl.find_opt sym_values id with
		| None -> Semantic_ast.Identifier id
		| Some expr ->
			if (Hashtbl.find sym_dependencies id) = 1 then map_expr expr
			else Semantic_ast.Identifier id
		
	and map_object_entry entry =
		let mapped_entry: Semantic_ast.dict_entry = { name = entry.name; expr = map_expr entry.expr } in
		mapped_entry
		
	and map_rule rule =
		let map_cond cond =
			match cond with
			| None -> None
			| Some x -> Some (map_expr x)
		in
		let mapped_rule: Semantic_ast.function_rule = { pattern = map_expr rule.pattern; condition = map_cond rule.condition; body = map_expr rule.body } in
		mapped_rule
	in
	
	let inline_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr -> Some (Semantic_ast.ExpressionStatement (map_expr expr))
		| Semantic_ast.LetBindingStatement (name, typedef, expr) ->
			if (Hashtbl.find sym_dependencies name) = 1 then None
			else Some (Semantic_ast.LetBindingStatement (name, typedef, map_expr expr))
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.stmt_to_string stmt))
	in
	
	let mapped_statements = List.filter_map (inline_stmt) _module.statements in
	let mapped_module: Semantic_ast.program_module = { moduleName = _module.moduleName; statements = mapped_statements; exports = _module.exports; exportedTypeNames = _module.exportedTypeNames; } in
	mapped_module
;;
