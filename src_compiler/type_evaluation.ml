let find_common_type types typeSystem = List.fold_left (fun acc t -> typeSystem#union acc t) (List.hd types) (List.tl types);;

(*
let eval_func_call_result_type =
	
	let rec replace_types t typeMap =
		match t with
		| Type_system.Any -> Type_system.Any
		| Type_system.Unknown -> Type_system.Unknown
		| Type_system.Function (arg, result) -> Type_system.Function (replace_types arg typeMap, replace_types result typeMap)
		| Type_system.Object decls ->
			let map_decl decl =
				let mapped_decl: Type_system.declaration = { name = decl.Type_system.name; typedef = replace_types decl.typedef typeMap } in
				mapped_decl
			in
			Type_system.Object ( List.map (map_decl) decls)
		| Type_system.Tuple entries -> Type_system.Tuple (List.map (fun entry -> replace_types entry typeMap) entries)
	in
;;*)
let eval_func_call_result_type resultType paramType argType typeSystem =
	let rec replace_generics t typeMap =
		let replace_generic_if_possible x =
			match Hashtbl.find_opt typeMap x with
			| None -> Type_system.Generic x
			| Some t -> replace_generics t typeMap
		in
		
		match t with
		| Type_system.Generic x -> replace_generic_if_possible x
		| Type_system.NamedRef _ -> t
		| _ -> raise (Stream.Error (( Type_system.to_string t ))) 
	in
	
	let (result, typeMap) = typeSystem#is_assignable_with_typeMap argType paramType in
	if result then
		replace_generics resultType typeMap
	else
	raise (Stream.Error ("Can't assign from: " ^ (Type_system.to_string paramType) ^ " to: " ^ (Type_system.to_string argType)))
;;


let unwrap_function_type t _ =
	match t with
	| Type_system.Function (arg, res) -> (arg, res)
	| _ -> raise (Stream.Error ("'" ^ (Type_system.to_string t) ^  "' is not a function type."))
;;

let rec eval_expr_type expr (context: SymbolTable.context) (typeSystem: Type_system.type_system) =
	match expr with
	| Semantic_ast.Self -> context#get_self_type
	| Semantic_ast.Identifier global_name -> context#get_type global_name
	| Semantic_ast.NaturalLiteral _ -> typeSystem#resolve_named_type "System" "nat"
	| Semantic_ast.StringLiteral _ -> typeSystem#resolve_named_type "System" "string"
	| Semantic_ast.External _ -> Type_system.Function(Type_system.Unknown, Type_system.Unknown)
	| Semantic_ast.Call (func, arg) ->
		let evaled_func = eval_expr_type func context typeSystem in
		let evaled_arg = eval_expr_type arg context typeSystem in
		let (paramType, resultType) = unwrap_function_type evaled_func typeSystem in
		eval_func_call_result_type resultType paramType evaled_arg typeSystem
	| Semantic_ast.Function (globalName, _) -> context#get_type globalName
	| Semantic_ast.Tuple exprs -> Type_system.Tuple( List.map (fun expr -> eval_expr_type expr context typeSystem) exprs )
	| _ -> raise (Stream.Error ("NOT IMPLEMENTED: " ^ Semantic_ast_print.expr_to_string expr))
