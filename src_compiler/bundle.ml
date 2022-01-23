module StringMap = Map.Make(String);;

let map_export_to_type_decl name (export: Semantic_ast.export) decls =
	let decl: Type_system.declaration = { name = name; typedef = export.typedef} in
	decl::decls
;;

let bundle modulesCollection =
	let symbolTable = modulesCollection#get_symbol_table in
	
	let renameMap: (string, string) Hashtbl.t = Hashtbl.create 10 in	
	let importMap: (string, string) Hashtbl.t = Hashtbl.create 10 in
	
	let map_export_to_object_entry name _ entries =
		let entry: Semantic_ast.object_entry = { name = name; expr = Semantic_ast.Identifier (Hashtbl.find renameMap name) } in
		entry::entries
	in
	
	let rec translate_expr expr =
		match expr with
		| Semantic_ast.Identifier global_name -> Semantic_ast.Identifier (Hashtbl.find renameMap global_name)
		| Semantic_ast.Call (func, arg) -> Semantic_ast.Call (translate_expr func, translate_expr arg)
		| Semantic_ast.Import moduleName -> Semantic_ast.Identifier (Hashtbl.find importMap moduleName)
		| _ -> expr
	in
	let translate_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr -> Semantic_ast.ExpressionStatement (translate_expr expr)
		| Semantic_ast.LetBindingStatement (name, td, expr) ->
			let globalName = if Hashtbl.mem renameMap name then symbolTable#create name td else name
			in
			Hashtbl.replace renameMap name globalName;
			Semantic_ast.LetBindingStatement(globalName, td, translate_expr expr)
		| _ -> stmt
	in
	
	let build_export _module =
		let t = Type_system.Object( StringMap.fold (map_export_to_type_decl) _module.Semantic_ast.exports [] ) in
		let name = symbolTable#create "_module" t in
		Hashtbl.add importMap _module.Semantic_ast.moduleName name;
		let entries = StringMap.fold (map_export_to_object_entry) _module.Semantic_ast.exports [] in
		Semantic_ast.LetBindingStatement(name, t, Semantic_ast.Object(entries))
	in

	
	let translate_module _module =
		let mappedStmts = (List.map (translate_stmt) _module.Semantic_ast.statements) in
		mappedStmts @ [build_export _module]
	in
	let translate_modules modules = List.flatten (List.map (translate_module) modules)
	in
	
	let stmts = translate_modules (modulesCollection#modules_in_order) in
	let _bundledModule: Semantic_ast.program_module = { moduleName = "bundle"; statements = stmts; exports = StringMap.empty } in
	_bundledModule
;;
