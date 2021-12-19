module StringMap = Map.Make(String);;




let rec order_functions funcs currentNumber =
	match currentNumber < (Hashtbl.length funcs) with
	| false -> []
	| true -> (Hashtbl.find funcs currentNumber)::(order_functions funcs (currentNumber+1))
;;


class translation_state (scope: ProgramModule.value StringMap.t) (orderedTypes: string list) (typesScope: Ast.typedefinition StringMap.t) =
	object
	
	method add_named_value name value =
		new translation_state (StringMap.add name value scope) orderedTypes typesScope
		
	method add_type name typedef =
		new translation_state scope (orderedTypes @ [name]) (StringMap.add name typedef typesScope)
		
	method fold_types_ordered =
		List.map (fun name -> (name, StringMap.find name typesScope)) orderedTypes
		
	method get_named_value name =
		StringMap.find_opt name scope
		
	method get_scope =
		scope
end;;


let translate moduleName _moduleAst modulesMap =
	(*
	let namedTypes: (string, Ast.typedefinition) Hashtbl.t = Hashtbl.create 10
	in
	let functions: (int, ProgramModule.statement list) Hashtbl.t = Hashtbl.create 10
	in
	
	let translate_pattern pattern scope =
		match pattern with
		| Ast.Identifier id -> Scope.add id ProgramModule.Parameter scope
		| _ -> raise (Stream.Error "call not implemented")
	in
	
	let rec translate_expr expr scope =
		match expr with
		| Ast.NaturalLiteral x -> ProgramModule.NaturalValue(x)
		| Ast.Select (expr, memberName) -> ProgramModule.Select(translate_expr expr scope, memberName)
		| Ast.Tuple exprs -> ProgramModule.Tuple( List.map (fun expr -> translate_expr expr scope) exprs )
		| Ast.Function (pattern, expr) ->
			let funcScope = translate_pattern pattern scope in
			let stmt = ProgramModule.ValueStatement(translate_expr expr funcScope) in
			let funcIdx = Hashtbl.length functions in
			Hashtbl.add functions funcIdx [stmt];
			Function(funcIdx)
	in

	
	Hashtbl.add functions (Hashtbl.length functions) stmts;
	let orderedFunctions = order_functions functions 0 in
	*)
	
	let translate_identifier id state =
		match state#get_named_value id with
		| Some value -> value
		| None -> raise (Stream.Error ("Unknown variable: " ^ id))
	in
	
	let rec translate_expr expr state =
		match expr with
		| Ast.Call (func, arg) -> ProgramModule.Call(translate_expr func state, translate_expr arg state)
		| Ast.External externalName -> ProgramModule.ExternalValue(externalName)
		| Ast.Identifier id -> translate_identifier id state
		| Ast.StringLiteral x -> ProgramModule.StringValue(x)
		| _ -> raise (Stream.Error "call not implemented")
	in
	
	let translate_stmt stmt state =
		match stmt with
		| Ast.ExpressionStatement expr -> Some(ProgramModule.ValueStatement(translate_expr expr state)), state
		| Ast.LetBindingStatement (name, typedef, expr) ->
			let translated_expr = translate_expr expr state in
			Some(ProgramModule.LetBindingStatement(name, typedef, translated_expr)), state#add_named_value name translated_expr
		| Ast.TypeDefStatement (name, typedef) -> None, state#add_type name typedef
	in
	
	let translate_toplevel toplevel state =
		match toplevel with
		| Ast.ImportToplevel moduleName ->
			let _module = StringMap.find moduleName modulesMap in
			let newState = StringMap.fold (fun exportName exportValue currentState -> currentState#add_named_value exportName exportValue) _module.ProgramModule.exports state in
			None, newState
		| Ast.StatementToplevel stmt -> translate_stmt stmt state
	in
	
	let rec translate_toplevels toplevels state =
		match toplevels with
		| [] -> [], state
		| toplevel::rest ->
			let (result, newState) = (translate_toplevel toplevel state) in
				match result with
				| Some(translated_stmt) ->
					let (nextResult, nextState) = translate_toplevels rest newState
					in
					translated_stmt::nextResult, nextState
				| None -> translate_toplevels rest newState
	in
	
	
	let stmts, state = translate_toplevels _moduleAst (new translation_state StringMap.empty [] StringMap.empty) in
	let result: ProgramModule.program_module = { moduleName = moduleName; functions = [stmts]; publicTypes = state#fold_types_ordered; exports = state#get_scope }
	in
	result
;;
