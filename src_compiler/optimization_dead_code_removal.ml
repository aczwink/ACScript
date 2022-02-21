let remove_unused_code _module =
	let sym_dependencies = Symbol_dependency_graph.compute _module in
	let sym_values = Symbol_evaluator.compute _module in
	
	let rec is_external expr =
		match expr with
		| Semantic_ast.Identifier id -> is_external (Hashtbl.find sym_values id)
		| Semantic_ast.External _ -> true
		| Semantic_ast.Select (expr, member) -> is_external (Symbol_evaluator.eval_select expr member sym_values)
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
	in
	
	let rec has_expr_side_effect expr =
		match expr with
		| Semantic_ast.Call (func, arg) -> (is_external func) || (has_expr_side_effect arg)
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
	in
	
	let remove_dead_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr ->
			if has_expr_side_effect expr then Some stmt else None
		| Semantic_ast.LetBindingStatement (name, _, _) ->
			if (Hashtbl.find sym_dependencies name) = 0 then None
			else Some stmt
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.stmt_to_string stmt))
	in
	
	let mapped_statements = List.filter_map (remove_dead_stmt) _module.statements in
	let mapped_module: Semantic_ast.program_module = { moduleName = _module.moduleName; statements = mapped_statements; exports = _module.exports; exportedTypeNames = _module.exportedTypeNames; } in
	mapped_module
;;
