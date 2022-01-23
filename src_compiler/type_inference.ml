module IntSet = Set.Make(Int);;
module IntMap = Map.Make(Int);;
module StringMap = Map.Make(String);;

class func_inferer initInferableGenerics (typeSystem: Type_system.type_system) =
	object(this)
	
	val mutable inferableGenerics = initInferableGenerics
	
	method fork additionalInferableGenerics =
		new func_inferer (IntSet.union additionalInferableGenerics inferableGenerics) typeSystem
	
	method infer_arg_and_param_must_match from _to =
		match _to with
		| Type_system.Generic x -> 
			if IntSet.mem x inferableGenerics
			then typeSystem#set_generic_type x from
			else ()
		| _ -> ()
	
	method infer_must_be_function_type t =
		match t with
		| Type_system.Generic x -> if IntSet.mem x inferableGenerics then this#infer_generic_must_be_function_type x else ()
		| _ -> ()
		
	method infer_must_have_constraint t member =
		match t with
		| Type_system.Generic x -> 
			if IntSet.mem x inferableGenerics then this#infer_generic_must_have_constraint x member else ()
		| _ -> ()
		
	method infer_types_must_match a b =
		match (a, b) with
		| (Type_system.Generic _, Type_system.Generic _) -> raise (Stream.Error ("not implemented"))
		| (Type_system.Generic x, _) ->
			this#infer_generic_types_must_match x a b
		| (_, Type_system.Generic y) ->
			this#infer_generic_types_must_match y b a
		| _ -> typeSystem#union a b
		
	method merge (fi: func_inferer) =
		fi#fork inferableGenerics
		
	method to_string =
		let nums = IntSet.fold (fun i acc -> (string_of_int i)::acc) inferableGenerics [] in
		String.concat ", " nums
		
	(* Private methods *)
	method private infer_generic_must_be_function_type genericTypeNumber =
		let argGeneric = typeSystem#add_generic_type in
		let resultGeneric = typeSystem#add_generic_type in
		let argType = Type_system.Generic argGeneric in
		let resultType = Type_system.Generic resultGeneric in
		typeSystem#set_generic_type genericTypeNumber (Type_system.Function(argType, resultType));
		inferableGenerics <- IntSet.add argGeneric inferableGenerics;
		inferableGenerics <- IntSet.add resultGeneric inferableGenerics
		
	method private infer_generic_must_have_constraint genericTypeNumber member =
		let num = typeSystem#add_generic_type in
		typeSystem#add_constraint genericTypeNumber member (Generic num);
		inferableGenerics <- IntSet.add num inferableGenerics
		
	method private infer_generic_types_must_match genericNumber a b =
		match IntSet.mem genericNumber inferableGenerics with
		| true ->
			let _ = typeSystem#set_generic_type genericNumber b in
			b
		| false -> typeSystem#union a b
end;;





let extract_member_type t member typeSystem =
	let extract_member_of_generic genericParameter =
		match genericParameter with
		| Type_system.Unspecified -> raise (Stream.Error ("unspec"))
		| Type_system.Typedef _ -> raise (Stream.Error ("Typedef"))
		| Type_system.ConstraintedObject constraints -> constraints#get member
	in
	
	match t with
	| Type_system.Generic x -> extract_member_of_generic (typeSystem#get_generic x)
	| _ -> raise (Stream.Error ((Type_system.to_string t) ^ member))
;;

let rec unwrap_function_type t typeSystem =
	let unwrap_function_type_from_generic genericParameter =
		match genericParameter with
		| Type_system.Unspecified -> raise (Stream.Error ("unspecified"))
		| Type_system.Typedef typedef -> unwrap_function_type typedef typeSystem
		| Type_system.ConstraintedObject _ -> raise (Stream.Error ("ConstraintedObject"))
	in
	
	match t with
	| Type_system.Generic x -> unwrap_function_type_from_generic (typeSystem#get_generic x)
	| Type_system.Function (arg, res) -> (arg, res)
	| _ -> raise (Stream.Error ("'" ^ (Type_system.to_string t) ^  "' is not a function type."))
;;



let eval_func_call_result_type paramType argType resultType typeSystem =
	
	let rec replace_types t typeMap =
		let replace_generic_if_possible x =
			match Hashtbl.find_opt typeMap x with
			| None -> Type_system.Generic x
			| Some t -> replace_types t typeMap
		in
		match t with
		| Type_system.Any -> Type_system.Any
		| Type_system.Generic x -> replace_generic_if_possible x
		| Type_system.NamedRef (moduleName, name) -> Type_system.NamedRef (moduleName, name)
		| Type_system.Function (arg, result) -> Type_system.Function (replace_types arg typeMap, replace_types result typeMap)
		| Type_system.Object decls ->
			let map_decl decl =
				let mapped_decl: Type_system.declaration = { name = decl.Type_system.name; typedef = replace_types decl.typedef typeMap } in
				mapped_decl
			in
			Type_system.Object ( List.map (map_decl) decls)
		| Type_system.Tuple entries -> Type_system.Tuple (List.map (fun entry -> replace_types entry typeMap) entries)
		| _ -> raise (Stream.Error (Type_system.to_string t))
	in
	
	
	let (result, typeMap) = typeSystem#is_assignable_with_typeMap paramType argType in
	if result then
		replace_types resultType typeMap
	else
	raise (Stream.Error ("Can't assign from: " ^ (Type_system.to_string paramType) ^ " to: " ^ (Type_system.to_string argType)))
;;

let find_common_type types func_inferer = List.fold_left (fun acc t -> func_inferer#infer_types_must_match acc t) (List.hd types) (List.tl types);;

let eval_expr_type expr symbolTable (typeSystem: Type_system.type_system) =

	let rec eval_expr_type_inner expr func_inferer =
		match expr with
		| Semantic_ast.Identifier global_name -> symbolTable#get_type global_name
		| Semantic_ast.NaturalLiteral _ -> Type_system.NamedRef("System", "nat")
		| Semantic_ast.StringLiteral _ -> Type_system.NamedRef("System", "string")
		| Semantic_ast.External _ -> Type_system.Function(Type_system.Unknown, Type_system.Unknown)
		| Semantic_ast.Import _ -> raise (Stream.Error "import error")
		| Semantic_ast.Call (func, arg) ->
			let evaled_func = eval_expr_type_inner func func_inferer in
			func_inferer#infer_must_be_function_type evaled_func;
			let (argType, resultType) = unwrap_function_type evaled_func typeSystem in
			let paramType = eval_expr_type_inner arg func_inferer in
			
			func_inferer#infer_arg_and_param_must_match paramType argType;
			eval_func_call_result_type paramType argType resultType typeSystem
		| Semantic_ast.Function rules ->
			let mapped = List.map (eval_func_rule func_inferer) rules in
			let funcInferableGenerics = List.fold_left (fun acc (_, _, fi) -> fi#merge acc) func_inferer mapped in
			let argType = find_common_type (List.map (fun (argType, _, _) -> argType) mapped) funcInferableGenerics in
			let resultType = find_common_type (List.map (fun (_, resultType, _) -> resultType) mapped) funcInferableGenerics in
			Type_system.Function(argType, resultType)
		| Semantic_ast.Object _ -> raise (Stream.Error "object error")
		| Semantic_ast.Select (select, member) ->
			let t = eval_expr_type_inner select func_inferer in
			func_inferer#infer_must_have_constraint t member;
			extract_member_type t member typeSystem
		| Semantic_ast.Tuple exprs -> Type_system.Tuple( List.map (fun expr -> eval_expr_type_inner expr func_inferer) exprs )
		
	and eval_func_rule func_inferer rule =
		let funcInferableGenerics = func_inferer#fork (Generics.collect_expr_generics_transitive rule.pattern symbolTable typeSystem) in
		let argType = eval_expr_type_inner rule.pattern funcInferableGenerics in
		let resultType = eval_expr_type_inner rule.body funcInferableGenerics in
		(argType, resultType, funcInferableGenerics)
	in
	eval_expr_type_inner expr (new func_inferer IntSet.empty typeSystem)
;;
