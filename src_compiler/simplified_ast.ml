module StringMap = Map.Make(String);;

let map_export_to_let_binding _moduleVarName name export exports =
	let v = Semantic_ast.Select( Semantic_ast.Identifier _moduleVarName, name) in
	Semantic_ast.LetBindingStatement(name, export.Semantic_ast.typedef, v)::exports
;;

let map_export_to_type_decl name (export: Semantic_ast.export) decls =
	let decl: Type_system.declaration = { name = name; typedef = export.typedef} in
	decl::decls
;;

let simplify_stmt modulesCollection stmt =
	let symbolTable = modulesCollection#get_symbol_table in
	match stmt with
	| Semantic_ast.TypeDefStatement _ -> []
	| Semantic_ast.UseStatement moduleName ->
		let import_module = modulesCollection#get_module moduleName in
		let t = Type_system.Object( StringMap.fold (map_export_to_type_decl) import_module.Semantic_ast.exports [] ) in
		let name = symbolTable#create "_module" t in
		let importStatement = Semantic_ast.LetBindingStatement(name, t, Semantic_ast.Import(moduleName)) in
		let assignments = StringMap.fold (map_export_to_let_binding name) import_module.Semantic_ast.exports [] in
		importStatement::assignments
	| _ -> [stmt]
;;

let simplify _module modulesCollection =
	let stmts = List.flatten (List.map (simplify_stmt modulesCollection) _module.Semantic_ast.statements)
	in	
	let result: Semantic_ast.program_module = { moduleName = _module.moduleName; statements = stmts; exports = _module.exports; exportedTypeNames = _module.exportedTypeNames; }
	in
	result
;;
