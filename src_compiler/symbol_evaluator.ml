let compute (_module: Semantic_ast.program_module) =
	let namedValues: (string, Semantic_ast.expression) Hashtbl.t = Hashtbl.create 10 in
	
	let process_stmt stmt =
		match stmt with
		| Semantic_ast.LetBindingStatement (name, _, expr) -> Hashtbl.add namedValues name expr
		| _ -> ()
	in
	
	List.iter (process_stmt) _module.statements;
	namedValues
;;


let rec eval_select expr member sym_values =
	match expr with
	| Semantic_ast.Identifier id -> eval_select (Hashtbl.find sym_values id) member sym_values
	| Semantic_ast.Object entries ->
		let entry = List.find (fun (entry: Semantic_ast.object_entry) -> entry.name = member) entries in
		entry.expr
	| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string expr))
;;

let is_object expr =
	match expr with
	| Semantic_ast.Object _ -> true
	| _ -> false
