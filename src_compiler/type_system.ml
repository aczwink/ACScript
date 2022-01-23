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
	let declaration_to_string decl =
		"\t" ^ decl.name ^ ": " ^ (to_string decl.typedef) ^ ";"
	in
	
	match typedef with
	| Any -> "any"
	| Unknown -> "unknown"
	| Generic x -> "<T" ^ (string_of_int x) ^ ">"
	| NamedRef (moduleName, name) -> moduleName ^ "." ^ name
	| Function (argType, retType) -> (to_string argType) ^ " -> " ^ (to_string retType)
	| Object decls ->
		if (List.length decls) = 0
		then "{}"
		else "{\n" ^ (String.concat ", " (List.map (declaration_to_string) decls)) ^ "\n}"
	| Tuple entries -> "(" ^ (String.concat ", " (List.map (to_string) entries)) ^ ")"
;;


class generic_constraints =
	object
	
	val constraints: (string, typedefinition) Hashtbl.t = Hashtbl.create 10
	
	method add name typedef =
		Hashtbl.add constraints name typedef
	
	method as_list =
		Hashtbl.fold (fun k v acc -> (k, v)::acc) constraints []
		
	method as_object =
		let to_decl k v decls =
			let decl = { name = k; typedef = v } in
			decl::decls
		in
		let decls = Hashtbl.fold (to_decl) constraints [] in
		Object(decls)
		
	method get name =
		Hashtbl.find constraints name
end;;


type generic_parameter =
	| Unspecified
	| Typedef of typedefinition
	| ConstraintedObject of generic_constraints
	
let generic_parameter_to_string gp =
	match gp with
	| Unspecified -> "?"
	| Typedef typedef -> to_string typedef
	| ConstraintedObject constraints -> to_string constraints#as_object
;;

class type_system =
	object(this)
	
	val genericTypes = Hashtbl.create 10
	val namedTypes = Hashtbl.create 10
	
	method add moduleName name (typedef: typedefinition) =
		Hashtbl.add namedTypes (moduleName ^ "." ^ name) typedef
	
	method add_constraint genericTypeNumber member t =
		let gp = Hashtbl.find genericTypes genericTypeNumber in
		match gp with
		| Unspecified ->
			let constraints = new generic_constraints in
			Hashtbl.replace genericTypes genericTypeNumber (ConstraintedObject constraints);
			constraints#add member t
		| Typedef td -> raise (Stream.Error ("Can't add constraint to typedef: " ^ to_string td))
		| ConstraintedObject constraints -> 
			constraints#add member t
	
	method add_generic_type =
		let nextNum = (Hashtbl.length genericTypes) + 1 in
		Hashtbl.add genericTypes nextNum Unspecified;
		nextNum
		
	method generics_as_list =
		Hashtbl.fold (fun k v acc -> (k, v)::acc) genericTypes []
		
	method get_generic genericTypeNumber =
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
		
	method set_generic_type genericTypeNumber typedef =
		let gp = Hashtbl.find genericTypes genericTypeNumber in
		match gp with
		| Unspecified ->
			Hashtbl.replace genericTypes genericTypeNumber (Typedef typedef)
		| ConstraintedObject constraints ->
			if this#is_assignable typedef (constraints#as_object)
			then Hashtbl.replace genericTypes genericTypeNumber (Typedef typedef)
			else raise (Stream.Error ("Can't specialize generic with: " ^ to_string typedef))
		| _ -> raise (Stream.Error ("Can't specialize generic with: " ^ to_string typedef))
		
	method union (a: typedefinition) (b: typedefinition) =
		if this#is_assignable a b then
			b
		else if this#is_assignable b a then
			a
		else
			raise (Stream.Error (to_string a ^ " - " ^ to_string b))
		
	(* Private methods *)
	method private is_assignable_inner from _to typeMap =
		match (from, _to) with
		| (_, Any) -> true
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
			this#is_assignable_from_generic (this#get_generic x) _to typeMap
		| (_, NamedRef (moduleName, name)) ->
			this#is_assignable_inner from (this#resolve_named_type moduleName name) typeMap
		
		| _ -> raise (Stream.Error ("is_assignable_inner - FROM:" ^ to_string from ^ " TO:" ^ to_string _to))
		
	method private is_assignable_from_generic gp _to typeMap =
		match gp with
		| Unspecified -> raise (Stream.Error ("NOT IMPLEMENTED"))
		| Typedef _ -> raise (Stream.Error ("NOT IMPLEMENTED"))
		| ConstraintedObject constraints -> this#is_assignable_inner constraints#as_object _to typeMap
		
	method private is_assignable_to_generic from gp typeMap =
		match gp with
		| Unspecified -> true
		| Typedef td -> this#is_assignable_inner from td typeMap
		| ConstraintedObject constraints -> this#is_assignable_inner from constraints#as_object typeMap
		
	method private is_assignable_to_generic_with_dict_check from y typeMap = 
		(* Replace type parameter by assuming it is assignable *)
		if (Hashtbl.mem typeMap y) && ((Hashtbl.find typeMap y) == from)
		then true
		else
			let _ = Hashtbl.add typeMap y from in
			this#is_assignable_to_generic from (this#get_generic y) typeMap
			
	method private is_assignable_to_generic_with_preserving_name moduleName name y typeMap =
		let res = this#is_assignable_inner (this#resolve_named_type moduleName name) (Generic y) typeMap in
		Hashtbl.replace typeMap y (NamedRef (moduleName, name));
		res
end;;
