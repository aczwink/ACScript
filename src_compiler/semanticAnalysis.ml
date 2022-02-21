module StringMap = Map.Make(String);;

class translation_state (scope: Semantic_ast.expression StringMap.t) (globalNameMap: string StringMap.t) (symbolTable: SymbolTable.symbol_table) (typeMap: string StringMap.t) =
	object(this)
	
	method add_named_type name moduleName =
		new translation_state scope globalNameMap symbolTable (StringMap.add name moduleName typeMap)
	
	method add_named_value name global_name value =
		new translation_state (StringMap.add name value scope) (StringMap.add name global_name globalNameMap) symbolTable typeMap
		
	method map_to_global_name name =
		StringMap.find name globalNameMap
		
	method get_named_value name =
		match StringMap.find_opt name scope with
		| Some value -> value
		| None -> raise (Stream.Error ("Unknown variable: " ^ name))
		
	method get_named_value_type name =
		symbolTable#get_type (this#map_to_global_name name)
		
	method get_named_type name =
		match StringMap.find_opt name typeMap with
		| None -> Type_system.create_named_type "" name
		| Some moduleName -> Type_system.create_named_type moduleName name
		
	method get_scope =
		scope
end;;


let rec find_export_names stmts =
	match stmts with
	| [] -> []
	| (Ast.LetBindingStatement(name, _, _))::rest -> name::(find_export_names rest)
	| _::rest -> find_export_names rest
;;

let rec find_exports exports state =
	match exports with
	| [] -> StringMap.empty
	| export::rest ->
		let x = find_exports rest state in
		let entry: Semantic_ast.export = { value = state#get_named_value export; typedef = state#get_named_value_type export; } in
		StringMap.add export entry x
;;

let rec find_export_type_names stmts =
	match stmts with
	| [] -> []
	| (Ast.TypeDefStatement(name, _))::rest -> name::(find_export_type_names rest)
	| _::rest -> find_export_type_names rest
;;

let translate moduleName _moduleAst modulesCollection =
	let typeSystem: Type_system.type_system = modulesCollection#get_type_system in
	let symbolTable = modulesCollection#get_symbol_table in
	
	let rec convert_object_decl declaration state =
		let mapped: Type_system.declaration = { name = declaration.Ast.name; typedef = convert_type declaration.Ast.typedef state } in
		mapped

	and convert_type typedef state =
		match typedef with
		| Ast.NamedType name -> state#get_named_type name
		| Ast.FunctionType (arg, ret) -> Type_system.Function(convert_type arg state, convert_type ret state)
		| Ast.ObjectType decls -> Type_system.Object (List.map (fun decl -> convert_object_decl decl state) decls)
		| Ast.TupleType entries -> Type_system.Tuple (List.map (fun td -> convert_type td state) entries)
	in

	let check_variable_type _type_optional expr_type state =
		match _type_optional with
		| None -> expr_type
		| Some desired ->
			let desired_type = convert_type desired state in
			if typeSystem#is_assignable expr_type desired_type then desired_type else
				raise (Stream.Error (Type_system.to_string expr_type ^ " to " ^ Type_system.to_string desired_type))
	in
	
	let typedef_opt_to_typedef typedef_opt default state =
		match typedef_opt with
		| None -> default
		| Some x -> convert_type x state
	in
	
	let verify_types_are_equal t1 t2 =
		if t1 = t2 then ()
		else raise (Stream.Error "type error")
	in
	
	let rec translate_pattern pattern state patternVars =
		match pattern with
		| Ast.IdentifierPattern (id, typedef_opt) -> match_identifier id state patternVars (typedef_opt_to_typedef typedef_opt Type_system.Unknown state)
		(*| Ast.NaturalLiteral literal -> (Semantic_ast.NaturalLiteral literal, state)*)
		| Ast.TuplePattern entries ->
			let map_entry entry (currentState, result) =
				let (translated_value, newState) = translate_pattern entry currentState patternVars in
				(newState, translated_value::result)
			in
			let (newState, mappedEntries) = List.fold_right (map_entry) entries (state, []) in
			(Semantic_ast.Tuple mappedEntries, newState)
		| _ -> raise (Stream.Error "TODO")
		
	and match_identifier id state patternVars t =
		match Hashtbl.find_opt patternVars id with
		| None ->
			let global_name = symbolTable#create id t in
			let translated_value = Semantic_ast.Identifier global_name in
			let newState = state#add_named_value id global_name translated_value in
			Hashtbl.add patternVars id global_name;
			(translated_value, newState)
		| Some global_name ->
			let t2 = symbolTable#get_type global_name in
			verify_types_are_equal t t2;
			let translated_value = Semantic_ast.Identifier global_name in
			(translated_value, state)
	in

	let rec translate_expr expr state =
		match expr with
		| Ast.NaturalLiteral x -> Semantic_ast.NaturalLiteral(x)
		| Ast.StringLiteral x -> Semantic_ast.StringLiteral(x)
		| Ast.UnsignedLiteral x -> Semantic_ast.UnsignedLiteral (String.sub x 0 ((String.length x) - 1))
		| Ast.Identifier id -> if id = "self" then Semantic_ast.Self else Semantic_ast.Identifier (state#map_to_global_name id)
		| Ast.External externalName -> Semantic_ast.External(externalName)
		| Ast.Call (func, arg) -> Semantic_ast.Call(translate_expr func state, translate_expr arg state)
		| Ast.BinaryInfixCall (lhs, op, rhs) -> translate_expr (Ast.Call( Ast.Select(lhs, op), Ast.Tuple(lhs::rhs::[]) )) state
		| Ast.Dictionary entries -> Semantic_ast.Dictionary (List.map (fun t -> translate_dict_entry t state) entries)
		| Ast.Function rules ->
			let mappedRules = List.map (fun rule -> translate_rule rule state) rules in
			let t = Type_inference.infer_function_type mappedRules (new SymbolTable.context symbolTable) typeSystem in
			let global_name = symbolTable#create "__func__" t in
			Semantic_ast.Function(global_name, mappedRules)
		| Ast.Tuple exprs -> Semantic_ast.Tuple( List.map (fun expr -> translate_expr expr state) exprs )
		| Ast.Select (expr, member) -> Semantic_ast.Select(translate_expr expr state, member)
		
	and translate_dict_entry (key, value) state =
		let mappedEntry: Semantic_ast.dict_entry = { name = key; expr = translate_expr value state } in
		mappedEntry
		
	and translate_rule (pattern, cond, expr) state =
		let translate_cond funcState =
			match cond with
			| None -> None
			| Some x -> Some (translate_expr x funcState)
		in
		let (pattern, funcState) = translate_pattern pattern state (Hashtbl.create 10) in
		
		let funcStateWithSelf = funcState in
		
		let translated_cond = translate_cond funcState in
		let translated_expr = translate_expr expr funcStateWithSelf in
		let rule: Semantic_ast.function_rule = { pattern = pattern; condition = translated_cond; body = translated_expr } in
		rule
	in
	
	let translate_statement toplevel state =
		match toplevel with
		| Ast.ExpressionStatement expr ->
			let translated_expr = translate_expr expr state in
			let _ = Type_evaluation.eval_expr_type translated_expr (new SymbolTable.context symbolTable) typeSystem in
			Semantic_ast.ExpressionStatement(translated_expr), state
		| Ast.LetBindingStatement (name, typedef, expr) ->
			let translated_value = translate_expr expr state in
			let expr_type = Type_evaluation.eval_expr_type translated_value (new SymbolTable.context symbolTable) typeSystem in
			let t = check_variable_type typedef expr_type state in
			let global_name = symbolTable#create name t in
			Semantic_ast.LetBindingStatement(global_name, t, translated_value), state#add_named_value name global_name translated_value
		| Ast.TypeDefStatement (name, typedef) ->
			let newState = state#add_named_type name moduleName in
			let t = convert_type typedef newState in
			typeSystem#add moduleName name t;
			Semantic_ast.TypeDefStatement(name, t), newState
		| Ast.UseStatement moduleName ->
			let _module = modulesCollection#get_module moduleName in
			let newState = StringMap.fold (fun exportName exportValue currentState ->
				currentState#add_named_value exportName exportName exportValue.Semantic_ast.value) _module.Semantic_ast.exports state in
			let newStateWithTypes = List.fold_left (fun state name -> state#add_named_type name moduleName) newState _module.Semantic_ast.exportedTypeNames in
			Semantic_ast.UseStatement(moduleName), newStateWithTypes
	in
	
	let rec translate_toplevels toplevels state =
		match toplevels with
		| [] -> [], state
		| toplevel::rest ->
			let (translated_stmt, newState) = (translate_statement toplevel state) in
			let (nextResult, nextState) = translate_toplevels rest newState in
			translated_stmt::nextResult, nextState
	in
	
	let stmts, state = translate_toplevels _moduleAst (new translation_state StringMap.empty StringMap.empty symbolTable StringMap.empty) in
	let exportNames = find_export_names _moduleAst in
	let exportTypeNames = find_export_type_names _moduleAst in
	let exports = find_exports exportNames state in
	let result: Semantic_ast.program_module = { moduleName = moduleName; statements = stmts; exports = exports; exportedTypeNames = exportTypeNames; }
	in
	result
;;
