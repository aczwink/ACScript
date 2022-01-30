type declaration = {
	name: string;
	typedef: typedefinition
}

and typedefinition =
	| Any
	| Unknown
	| Generic of int
	| NamedRef of string * string
	| Function of typedefinition * typedefinition
	| Object of declaration list
	| Tuple of typedefinition list
	

let create_named_type moduleName name =
	match name with
	| "any" -> Any
	| "unknown" -> Unknown
	| name -> NamedRef(moduleName, name)
;;
	
let rec to_string typedef =	
	match typedef with
	| Any -> "any"
	| Unknown -> "unknown"
	| Generic x -> "<T" ^ (string_of_int x) ^ ">"
	| NamedRef (moduleName, name) -> moduleName ^ "." ^ name
	| Function (argType, retType) -> (to_string argType) ^ " -> " ^ (to_string retType)
	| Object decls ->
		if (List.length decls) = 0
		then "{}"
		else "{\n" ^ (declarations_to_string decls) ^ "\n}"
	| Tuple entries -> "(" ^ (String.concat ", " (List.map (to_string) entries)) ^ ")"
	
and declarations_to_string decls =
	let declaration_to_string decl =
		"\t" ^ decl.name ^ ": " ^ (to_string decl.typedef) ^ ";"
	in
	(String.concat "\n" (List.map (declaration_to_string) decls))
;;

let generic_to_string genericNumber genericConstraints =
	to_string (Generic genericNumber) ^ ": {\n" ^ declarations_to_string genericConstraints ^ "\n}"
;;


class type_system =
	object(this)
	
	val genericTypes: (int, declaration list) Hashtbl.t = Hashtbl.create 10
	val namedTypes = Hashtbl.create 10
	
	method add moduleName name (typedef: typedefinition) =
		Hashtbl.add namedTypes (moduleName ^ "." ^ name) typedef
	
	method add_generic_type =
		let nextNum = (Hashtbl.length genericTypes) + 1 in
		Hashtbl.add genericTypes nextNum [];
		nextNum
		
	method generics_as_list =
		Hashtbl.fold (fun k v acc -> (k, v)::acc) genericTypes []
		
	method get_generic_constraints genericTypeNumber =
		Hashtbl.find genericTypes genericTypeNumber
	
	method is_assignable (from: typedefinition) (_to: typedefinition) =
		let (result, _) = this#is_assignable_with_typeMap from _to
		in result
		
	method is_assignable_with_typeMap (from: typedefinition) (_to: typedefinition) =
		let typeMap = Hashtbl.create 10 in
		let result = this#is_assignable_inner from _to typeMap in
		(result, typeMap)
		
	method resolve_named_type moduleName name =
		Hashtbl.find namedTypes (moduleName ^ "." ^ name)
		
	method set_generic_constraints genericNumber (constraints: declaration list) =
		Hashtbl.replace genericTypes genericNumber constraints
		
	method union (a: typedefinition) (b: typedefinition) =
		if this#is_assignable a b then
			a
		else if this#is_assignable b a then
			b
		else
			raise (Stream.Error (to_string a ^ " - " ^ to_string b))
		
	(* Private methods *)
	method private avoid_self_cycle genericTypeNumber typedef =
		match typedef with
		| Generic x ->
			if x = genericTypeNumber then
				raise (Stream.Error ("Can't specialize generic with itself")) 
			else ()
		| _ -> ()
		
	method private is_assignable_inner from _to typeMap =
		match (from, _to) with
		| (_, Any) -> true
		| (_, Unknown) -> true
		| (Function (argTypeFrom, retTypeFrom), Function (argTypeTo, retTypeTo)) -> (this#is_assignable_inner argTypeFrom argTypeTo typeMap) && (this#is_assignable_inner retTypeFrom retTypeTo typeMap)
		| (Generic x, Generic y) ->
			if x == y then true
			else this#is_assignable_to_generic_with_dict_check from y typeMap
		| (NamedRef (moduleName1, name1), NamedRef (moduleName2, name2)) ->
			if ((moduleName1 = moduleName2) && (name1 = name2)) then true
			else this#is_assignable_inner (this#resolve_named_type moduleName1 name1) (this#resolve_named_type moduleName2 name2) typeMap
		| (Object fromDecls, Object toDecls) ->
			let fulfills_decl toDecl =
				let fromDecl_opt = List.find_opt (fun decl -> decl.name = toDecl.name) fromDecls in
				match fromDecl_opt with
				| None -> false
				| Some fromDecl -> this#is_assignable_inner fromDecl.typedef toDecl.typedef typeMap
			in
			List.for_all (fulfills_decl) toDecls
		| (Tuple fromEntries, Tuple toEntries) -> List.for_all (fun (a, b) -> this#is_assignable_inner a b typeMap) (List.combine fromEntries toEntries)
		
		| (Unknown, NamedRef _) -> true (* Unknown can be assigned to any type explicitly. TODO: check this. it stinks *)
		
		| (NamedRef (moduleName, name), Generic y) ->
			this#is_assignable_to_generic_with_preserving_name moduleName name y typeMap
		| (NamedRef (moduleName, name), _) ->
			this#is_assignable_inner (this#resolve_named_type moduleName name) _to typeMap
		| (_, Generic y) ->
			this#is_assignable_to_generic_with_dict_check from y typeMap
		| (Generic x, _) ->
			this#is_assignable_from_generic (this#get_generic_constraints x) _to typeMap
		| (_, NamedRef (moduleName, name)) ->
			this#is_assignable_inner from (this#resolve_named_type moduleName name) typeMap
		
		| _ -> raise (Stream.Error ("is_assignable_inner - FROM:" ^ to_string from ^ " TO:" ^ to_string _to))
		
	method private is_assignable_from_generic _ _to _ =
		raise (Stream.Error ("is_assignable_from_generic TO:" ^ to_string _to))
		
	method private is_assignable_to_generic from constraints typeMap =
		this#is_assignable_inner from (Object constraints) typeMap
		
	method private is_assignable_to_generic_with_dict_check from y typeMap = 
		(* Replace type parameter by assuming it is assignable *)
		if (Hashtbl.mem typeMap y) && ((Hashtbl.find typeMap y) == from)
		then true
		else
			let _ = Hashtbl.add typeMap y from in
			this#is_assignable_to_generic from (this#get_generic_constraints y) typeMap
			
	method private is_assignable_to_generic_with_preserving_name moduleName name y typeMap =
		let resolved = (this#resolve_named_type moduleName name) in
		Hashtbl.add typeMap y resolved; (* assume *)
		let res = this#is_assignable_inner resolved (Generic y) typeMap in
		Hashtbl.replace typeMap y (NamedRef (moduleName, name));
		res
end;;
