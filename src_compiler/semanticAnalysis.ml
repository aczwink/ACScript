module StringMap = Map.Make(String);;

class translation_state (scope: Semantic_ast.expression StringMap.t) (globalNameMap: string StringMap.t) (symbolTable: SymbolTable.symbol_table) =
	object(this)
	
	method add_named_value name global_name value =
		new translation_state (StringMap.add name value scope) (StringMap.add name global_name globalNameMap) symbolTable
		
	method ensure_has_named_value name =
		let _ = this#get_named_value name in
		()
		
	method get_named_value name =
		match StringMap.find_opt name scope with
		| Some value -> value
		| None -> raise (Stream.Error ("Unknown variable: " ^ name))
		
	method get_named_value_type name =
		this#ensure_has_named_value name;
		symbolTable#get_type (StringMap.find name globalNameMap)
		
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

let rec convert_object_decl moduleName declaration =
	let mapped: Type_system.declaration = { name = declaration.Ast.name; typedef = convert_type moduleName declaration.Ast.typedef } in
	mapped

and convert_type moduleName typedef =
	match typedef with
	| Ast.NamedType name -> Type_system.create_named_type moduleName name
	| Ast.FunctionType (arg, ret) -> Type_system.Function(convert_type moduleName arg, convert_type moduleName ret)
	| Ast.ObjectType decls -> Type_system.Object (List.map (convert_object_decl moduleName) decls)
	| Ast.TupleType entries -> Type_system.Tuple (List.map (convert_type moduleName) entries)
;;

let translate moduleName _moduleAst modulesCollection =
	let typeSystem: Type_system.type_system = modulesCollection#get_type_system in
	let symbolTable = modulesCollection#get_symbol_table in

	let check_variable_type _type_optional expr_type =
		match _type_optional with
		| None -> expr_type
		| Some desired ->
			let desired_type = convert_type moduleName desired in
			if typeSystem#is_assignable expr_type desired_type then desired_type else
				raise (Stream.Error "type error")
	in
	
	let translate_pattern pattern state =
		match pattern with
		| Ast.Identifier id ->
			let t = Type_system.Generic( typeSystem#add_generic_type ) in
			let global_name = symbolTable#create id t in
			let translated_value = Semantic_ast.Identifier global_name in
			let newState = state#add_named_value id global_name translated_value in
			(translated_value, newState)
		| Ast.NaturalLiteral literal -> (Semantic_ast.NaturalLiteral literal, state)
		| _ -> raise (Stream.Error "call not implemented")
	in

	let rec translate_expr expr state =
		match expr with
		| Ast.NaturalLiteral x -> Semantic_ast.NaturalLiteral(x)
		| Ast.StringLiteral x -> Semantic_ast.StringLiteral(x)
		| Ast.Identifier id -> state#ensure_has_named_value id; Semantic_ast.Identifier id
		| Ast.External externalName -> Semantic_ast.External(externalName)
		| Ast.Call (func, arg) -> Semantic_ast.Call(translate_expr func state, translate_expr arg state)
		| Ast.Function rules -> Semantic_ast.Function (List.map (fun rule -> translate_rule rule state) rules)
		| Ast.Tuple exprs -> Semantic_ast.Tuple( List.map (fun expr -> translate_expr expr state) exprs )
		| Ast.Select (expr, member) -> Semantic_ast.Select(translate_expr expr state, member)
		
	and translate_rule (pattern, expr) state =
		let (pattern, funcState) = translate_pattern pattern state in
		
		let t = Type_system.Function(Type_system.Generic( typeSystem#add_generic_type ), Type_system.Generic( typeSystem#add_generic_type )) in
		let global_name = symbolTable#create "self" t in
		let funcStateWithSelf = funcState#add_named_value "self" global_name (Semantic_ast.Identifier "self") in
		
		let translated_expr = translate_expr expr funcStateWithSelf in
		let rule: Semantic_ast.function_rule = { pattern = pattern; body = translated_expr } in
		rule
	in
	
	let translate_statement toplevel state =
		match toplevel with
		| Ast.ExpressionStatement expr ->
			let translated_expr = translate_expr expr state in
			let _ = Type_inference.eval_expr_type translated_expr symbolTable typeSystem in
			Semantic_ast.ExpressionStatement(translated_expr), state
		| Ast.LetBindingStatement (name, typedef, expr) ->
			let translated_value = translate_expr expr state in
			let expr_type = Type_inference.eval_expr_type translated_value symbolTable typeSystem in
			let t = check_variable_type typedef expr_type in
			let global_name = symbolTable#create name t in
			Semantic_ast.LetBindingStatement(global_name, t, translated_value), state#add_named_value name global_name translated_value
		| Ast.TypeDefStatement (name, typedef) ->
			let t = convert_type moduleName typedef in
			typeSystem#add moduleName name t;
			Semantic_ast.TypeDefStatement(name, t), state
		| Ast.UseStatement moduleName ->
			let _module = modulesCollection#get_module moduleName in
			let newState = StringMap.fold (fun exportName exportValue currentState ->
			currentState#add_named_value exportName (symbolTable#create exportName exportValue.Semantic_ast.typedef) exportValue.Semantic_ast.value) _module.Semantic_ast.exports state in
			Semantic_ast.UseStatement(moduleName), newState
	in
	
	let rec translate_toplevels toplevels state =
		match toplevels with
		| [] -> [], state
		| toplevel::rest ->
			let (translated_stmt, newState) = (translate_statement toplevel state) in
			let (nextResult, nextState) = translate_toplevels rest newState in
			translated_stmt::nextResult, nextState
	in
	
	let stmts, state = translate_toplevels _moduleAst (new translation_state StringMap.empty StringMap.empty symbolTable) in
	let exportNames = find_export_names _moduleAst in
	let exports = find_exports exportNames state in
	let result: Semantic_ast.program_module = { moduleName = moduleName; statements = stmts; exports = exports }
	in
	result
;;
