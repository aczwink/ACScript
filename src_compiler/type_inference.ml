module IntMap = Map.Make(Int);;
module IntSet = Set.Make(Int);;
module StringMap = Map.Make(String);;


type constraint_type =
	| Equal
	| Supertype
;;

type inferable_type_structure =
	| Func of int * int
	| Tuple of int list
;;

type generic_system = {
	types: int list;
	constraints: (int * int * constraint_type) list;
	member_constraints: (int * string * int) list;
	stdtype_constraints: (int * constraint_type * Type_system.typedefinition) list;
	typeStructure: inferable_type_structure IntMap.t;
};;

let get_constraints_from t gs =
	List.filter (fun (t1, _, _) -> t1 = t) gs.constraints
;;

let get_constraints t gs =
	List.filter (fun (t1, t2, _) -> (t1 = t) || (t2 = t)) gs.constraints
;;

let get_member_constraints t gs =
	List.filter (fun (t1, _, _) -> t1 = t) gs.member_constraints
;;

let get_std_type_constraints t gs =
	List.filter (fun (tx, _, _) -> tx = t) gs.stdtype_constraints
;;

let get_structure t gs =
	IntMap.find_opt t gs.typeStructure
;;

let to_reference_string number =
	"<U" ^ (string_of_int number) ^ ">"
;;

let structure_to_string structure =
	match structure with
	| Func(a, b) -> to_reference_string a ^ " -> " ^ to_reference_string b
	| Tuple types -> "(" ^ (String.concat ", " (List.map (fun t -> to_reference_string t) types)) ^ ")"
;;
	
let to_string gs =	
	let structure_opt_to_string t =
		match get_structure t gs with
		| None -> ""
		| Some x -> "\t-structure: " ^ structure_to_string x ^ "\n"
	in
	
	let constraint_type_to_string ct =
		match ct with
		| Equal -> "equal to"
		| Supertype -> "super type of"
	in
	
	let constraints_to_strings t =
		let constraint_to_string (t1, t2, ct) =
			"\t-" ^ (to_reference_string t1) ^ " is " ^ (constraint_type_to_string ct) ^ " " ^ (to_reference_string t2)
		in
		List.map (constraint_to_string) (get_constraints t gs)
	in
	
	let member_constraints_to_strings t =
		let member_constraint_to_string (_, member, member_type) =
			"\t-has member '" ^ member ^ "' with type: " ^ (to_reference_string member_type)
		in
		List.map (member_constraint_to_string) (get_member_constraints t gs)
	in
	
	let stdtype_constraints_to_strings t =
		let constraint_to_string (t, ct, std) =
			"\t-" ^ (to_reference_string t) ^ " is " ^ constraint_type_to_string ct ^ " " ^ Type_system.to_string std
		in
		List.map (constraint_to_string) (get_std_type_constraints t gs)
	in

	let type_to_string t =
		let l = (constraints_to_strings t) @ (member_constraints_to_strings t) @ (stdtype_constraints_to_strings t) in
		to_reference_string t  ^ ":" ^ structure_opt_to_string t ^ (String.concat "\n" l)
	in

	let ordered = List.sort (compare) gs.types in
	let genericsStr = List.map (fun t -> type_to_string t) ordered in
	String.concat "\n" genericsStr
;;

class generic_system_builder =
	object(this)
	
	val mutable counter = 0	
	val mutable types = []
	val mutable constraints = []
	val mutable member_constraints = []
	val mutable stdtype_constraints = []
	val mutable typeStructure: (int, inferable_type_structure) Hashtbl.t = Hashtbl.create 10
	
	method add_constraint from _to ct =
		if ct = Equal then this#must_be_equal_to from _to
		else this#must_be_assignable_to from _to
	
	method add_generic_type =
		let nextNum = counter in
		counter <- counter + 1;
		types <- nextNum::types;
		nextNum
		
	method build =
		this#add_transitive_constraints;
		let typeStructure = Hashtbl.fold (fun k v accu -> IntMap.add k v accu) typeStructure IntMap.empty in
		let gs = { types = types; member_constraints = member_constraints; constraints = constraints; stdtype_constraints = stdtype_constraints; typeStructure = typeStructure } in
		gs
		
	method get_constraints t =
		List.filter (fun (t1, t2, _) -> (t1 = t) || (t2 = t)) constraints
		
	method gen_type structure =
		let t = this#add_generic_type in
		this#must_have_structure t structure;
		t
		
	method gen_type_from_std_type stdt =
		let t = this#add_generic_type in
		this#must_be_standard_type t stdt;
		t
	
	method get_structure t =
		Hashtbl.find_opt typeStructure t
		
	method must_be_assignable_to from _to =
		this#add_constraint_with_existence_check from _to Supertype
		
	method must_be_equal_to t1 t2 =
		this#add_constraint_with_existence_check (min t1 t2) (max t1 t2) Equal
		
	method must_be_standard_type t (stdt: Type_system.typedefinition) =
		stdtype_constraints <- (t, Equal, stdt)::stdtype_constraints
		
	method must_have_function_structure t =
		let argT = this#add_generic_type in
		let resultT = this#add_generic_type in
		this#must_have_structure t (Func (argT, resultT));
		(argT, resultT)
		
	method must_have_member (t: int) (member: string) =
		match List.find_opt (fun (tx, memberx, _) -> (tx = t) && (member = memberx)) member_constraints with
		| None ->
			let member_type = this#add_generic_type in
			member_constraints <- (t, member, member_type)::member_constraints;
			member_type
		| Some (_, _, member_type) -> member_type
		
	method must_have_structure t newStructure =
		match this#get_structure t with
		| None -> Hashtbl.add typeStructure t newStructure
		| Some currentStructure -> this#set_structure currentStructure newStructure
		
	method must_have_tuple_structure t (nEntries: int) =
		let rec genEntries (n: int) =
			match n with
			| 0 -> []
			| x -> (this#add_generic_type)::(genEntries (x-1))
		in
		let entries: int list = genEntries nEntries in
		this#must_have_structure t (Tuple entries);
		entries
		
	(* Private methods *)
	method private add_constraint_with_existence_check from _to ct =
		if List.exists (fun c -> c = (from, _to, ct)) constraints then
			()
		else
			constraints <- (from, _to, ct)::constraints
			
	method private add_transitive_constraints =
		let l = List.length constraints in
		
		List.iter (this#add_transitive_constraints_over_structure) constraints;
		this#add_transitive_constraints_over_operator;
		(*this#add_transitive_constraints_over_members;*)
		
		if (List.length constraints) > l then
			this#add_transitive_constraints
		else
			()
			
	method private add_transitive_constraints_over_members =
		let check_transitive (t1, t2, ct) =
			let t1m = List.filter (fun (t, _, _) -> t1 = t) member_constraints in
			
			let add_transitive_if_not_existant (_, member2, member_type2) =
				match List.find_opt (fun (_, member1, _) -> member2 = member1) t1m with
				| None -> ()
				| Some (_, _, member_type1) ->
				this#add_constraint member_type1 member_type2 ct
			in
			
			let t2m = List.filter (fun (t, _, _) -> t2 = t) member_constraints in
			List.iter (add_transitive_if_not_existant) t2m
		in
		
		List.iter (check_transitive) constraints
		
	method private add_transitive_constraints_over_operator =
		let current_c = constraints in
		
		let check_transitive (t1, t2, ct) =
			let add_transitive_if_not_existant (t3, t4, _) =
				if (ct = Equal) && (t2 = t4) && (t1 != t3) then
					this#must_be_equal_to t1 t3
				else if (ct = Supertype) && (t2 = t3) then
					this#must_be_assignable_to t1 t4
				else
					()
			in
			let t2c = List.filter (fun (_, _, ct2) ->  ct = ct2) (List.filter (fun (a, b, _) -> (a = t2) || (b = t2)) current_c) in
			List.iter (add_transitive_if_not_existant) t2c
		in
		
		List.iter (check_transitive) current_c
		
	method private add_transitive_constraints_and_structure s1 t2 ct =
		match s1 with
		| Tuple e1 ->
			let e2 = this#must_have_tuple_structure t2 (List.length e1) in
			this#add_transitive_constraints_over_structure_impl s1 (Tuple e2) ct
		| _ -> raise (Stream.Error ((structure_to_string s1)))
		
	method private add_transitive_constraints_and_structure_right t1 s2 ct =
		match s2 with
		| Tuple e2 ->
			let e1 = this#must_have_tuple_structure t1 (List.length e2) in
			this#add_transitive_constraints_over_structure_impl (Tuple e1) s2 ct
		| _ -> raise (Stream.Error ((structure_to_string s2)))
		
	method private add_transitive_constraints_over_structure (t1, t2, ct) =
		match (this#get_structure t1, this#get_structure t2) with
		| (None, None) -> ()
		| (Some s1, None) -> this#add_transitive_constraints_and_structure s1 t2 ct
		| (None, Some s2) -> this#add_transitive_constraints_and_structure_right t1 s2 ct
		| (Some s1, Some s2) -> this#add_transitive_constraints_over_structure_impl s1 s2 ct
		
	method private add_transitive_constraints_over_structure_impl s1 s2 ct =
		match (s1, s2) with
		| (Func (a1, r1), Func(a2, r2)) -> this#add_constraint a1 a2 ct; this#add_constraint r1 r2 ct
		| (Tuple a, Tuple b) -> List.iter (fun (t1, t2) -> this#add_constraint t1 t2 ct) (List.combine a b)
		| _ -> ()
		
	method private set_structure currentStructure newStructure =
		match (currentStructure, newStructure) with
		| (Func (a1, r1), Func (a2, r2)) ->
			this#must_be_equal_to a1 a2;
			this#must_be_equal_to r1 r2;
		| (Tuple _, Tuple _) -> raise (Stream.Error (("TODO")))
		| _ -> raise (Stream.Error (("TODO")))
end;;




class func_inferer (context: SymbolTable.context) (gsb: generic_system_builder) =
	object
	
	val args: (string, int) Hashtbl.t = Hashtbl.create 10	
	val mutable selfType = -1
	
	method add_arg name =
		match Hashtbl.find_opt args name with
		| None -> Hashtbl.add args name (gsb#add_generic_type)
		| Some _ -> ()
	
	method get_name globalName =
		match Hashtbl.find_opt args globalName with
		| None ->
			let t = gsb#add_generic_type in
			gsb#must_be_standard_type t (context#get_type globalName);
			t
		| Some x -> x
	
	method get_self =
		selfType
	
	method init =
		let t = gsb#add_generic_type in
		let _ = gsb#must_have_function_structure t in
		selfType <- t
end;;


let uniq_cons x xs = if List.mem x xs then xs else x :: xs

let filter_duplicates xs = List.fold_right (uniq_cons) xs []



let combine_equal_types gs =
	let mergePairs = List.fold_left (fun map (t1, t2, ct) -> if ct = Equal then IntMap.add (max t1 t2) (min t1 t2) map else map) IntMap.empty gs.constraints in
	let rec resolve_transitivity x =
		match IntMap.find_opt x mergePairs with
		| None -> x
		| Some y -> resolve_transitivity y
	in
	let build_group map (source, target) =
		match IntMap.find_opt target map with
		| None -> IntMap.add target [source] map
		| Some l -> IntMap.add target (source::l) map
	in
	
	let mergePairsWithTransitivityResolved = IntMap.fold (fun k v accu -> IntMap.add k (resolve_transitivity v) accu) mergePairs IntMap.empty in
	let mergePairsList = IntMap.fold (fun k v accu -> (k, v)::accu) mergePairsWithTransitivityResolved [] in
	let mergeGroups = List.fold_left (build_group) IntMap.empty mergePairsList in
	let mergeTargets = List.fold_left (fun set (_, t) -> IntSet.add t set) IntSet.empty mergePairsList in
	let mergeSources = List.fold_left (fun set (t, _) -> IntSet.add t set) IntSet.empty mergePairsList in
	let typeIds = List.fold_left (fun set t -> IntSet.add t set) IntSet.empty gs.types in
	let copyTypeIds = IntSet.diff typeIds (IntSet.union mergeTargets mergeSources) in
		
	let renameType t =
		match IntMap.find_opt t mergePairsWithTransitivityResolved with
		| None -> t
		| Some x -> x
	in
	
	let map_constraint (t1, t2, ct) =
		if ct = Equal then None
		else Some (renameType t1, renameType t2, ct)
	in
	
	let map_constraints constraints =
		List.filter_map (map_constraint) constraints
	in
	
	let map_member_constraint (t, member, member_type) =
		(renameType t, member, renameType member_type)
	in
	
	let map_member_constraints mcs =
		List.map (map_member_constraint) mcs
	in
	
	let rename_std_type_constraints std =
		List.map (fun (t, ct, typedef) -> (renameType t, ct, typedef)) std
	in
	
	let get_merged_type (t1, t2) =
		if (renameType t1) != (renameType t2) then raise (Stream.Error ("TODO"))
		else renameType t1
	in
	
	let create_merged_structure s1 s2 =
		match (s1, s2) with
		| (Func(a1, r1), Func(a2, r2)) -> Func(get_merged_type (a1, a2), get_merged_type (r1, r2))
		| (Tuple t1, Tuple t2) -> Tuple (List.map (get_merged_type) (List.combine t1 t2))
		| _ -> raise (Stream.Error ((structure_to_string s1 ^ " - " ^ structure_to_string s2)))
	in
	
	let merge_structures from _to =
		match (from, _to) with
		| (None, None) -> None
		| (Some x, None) -> Some x
		| (None, Some x) -> Some x
		| (Some x, Some y) -> Some (create_merged_structure x y)
	in
	
	let query_type t =
		let c = get_constraints t gs in
		let m = get_member_constraints t gs in
		let std = get_std_type_constraints t gs in
		let str = get_structure t gs in
		(c, m, std, str)
	in
	
	let merge_types (c1, m1, std1, str1) (c2, m2, std2, str2) =
		let c = map_constraints (c1 @ c2) in
		let m = map_member_constraints (m1 @ m2) in
		let std = rename_std_type_constraints (std1 @ std2) in
		let str = merge_structures str1 str2 in
		
		(c, m, std, str)
	in
	let merge_group _to sources =
		let types = List.map (query_type) sources in
		let (c, m, std, str) = List.fold_left (merge_types) (query_type _to) types in
		(_to, c, m, std, str)
	in
	let mergedTypes = IntMap.fold (fun _to sources accu -> (merge_group _to sources)::accu) mergeGroups [] in
	let mergedStructures = List.fold_left (fun map (t, _, _, _, s) -> if Option.is_none s then map else IntMap.add t (Option.get s) map ) IntMap.empty mergedTypes in
	
	let rename_structure structure =
		let rename_structure_impl x =
			match x with
			| Func(a, r) -> Func(renameType a, renameType r)
			| Tuple ts -> Tuple( List.map (renameType) ts )
		in
		match structure with
		| None -> None
		| Some x -> Some (rename_structure_impl x)
	in
	
	let copy_type t =
		(t, map_constraints (get_constraints_from t gs), map_member_constraints (get_member_constraints t gs), get_std_type_constraints t gs, rename_structure (get_structure t gs))
	in
	let copiedTypes = IntSet.fold (fun t accu -> (copy_type t)::accu) copyTypeIds [] in
	let copiedStructures = List.fold_left (fun map (t, _, _, _, s) -> if Option.is_none s then map else IntMap.add t (Option.get s) map ) IntMap.empty copiedTypes in
	
	{
		types = List.filter (fun t -> Bool.not (IntSet.mem t mergeSources)) gs.types;
		constraints = filter_duplicates (List.flatten (List.map (fun (_, c, _, _, _) -> c) (copiedTypes @ mergedTypes)));
		member_constraints = List.flatten (List.map (fun (_, _, m, _, _) -> m) (copiedTypes @ mergedTypes));
		stdtype_constraints = filter_duplicates( (List.flatten (List.map (fun (_, _, _, s, _) -> s) copiedTypes)) @ (List.flatten (List.map (fun (_, _, _, s, _) -> s) mergedTypes)) );
		typeStructure = (IntMap.union (fun _ v1 _ -> Some v1) copiedStructures mergedStructures);
	}
;;

type generic_type = {
	member_constraints: int StringMap.t;
	stdtype_constraints: (constraint_type * Type_system.typedefinition) list;
	structure: inferable_type_structure option;
};;
class generic_system_modifier gs =
	object(this)
		
	val equalityRelations = Hashtbl.create 10
	val modifiedTypes: (int, generic_type) Hashtbl.t = Hashtbl.create 10
	val mutable superTypeRelations = []
	
	method add_equality_constraint t1 t2 =
		Hashtbl.add equalityRelations (min t1 t2) (max t1 t2)
	
	method build =
		{
			types = gs.types;
			constraints = (List.map (fun (t1, t2) -> (t1, t2, Supertype)) superTypeRelations);
			member_constraints = gs.member_constraints;
			stdtype_constraints = gs.stdtype_constraints;
			typeStructure = gs.typeStructure;
		}
		
	method filter_map_superType_relations f =
		superTypeRelations <- List.filter_map (f) superTypeRelations
		
	method get_type t =
		match Hashtbl.find_opt modifiedTypes t with
		| None ->
			let x = {
				member_constraints = List.fold_left (fun map (_, member, member_type) -> StringMap.add member member_type map) StringMap.empty (get_member_constraints t gs);
				stdtype_constraints = List.map (fun (_, ct, typedef) -> (ct, typedef)) (get_std_type_constraints t gs);
				structure = IntMap.find_opt t gs.typeStructure;
			} in
			Hashtbl.add modifiedTypes t x;
			x
		| Some x -> x
		
	method init =
		List.iter (fun (t1, t2, _) -> this#add_equality_constraint t1 t2) (List.filter (fun (_, _, ct) -> ct = Equal) gs.constraints);
		superTypeRelations <- List.map (fun (t1, t2, _) -> (t1, t2)) (List.filter (fun (_, _, ct) -> ct = Supertype) gs.constraints)
end;;

let resolve_super_types gs =
	let gsm = new generic_system_modifier gs in
	gsm#init;
	
	let rec verify_structure_is_super_type_or_make_it_super_type superTypeStructure subTypeStructure =
		match (superTypeStructure, subTypeStructure) with
		| (Func _, Func _) -> raise (Stream.Error ("TODO"))
		| (Tuple t1s, Tuple t2s) -> List.iter (fun (t1, t2) -> verify_is_super_type_or_make_it_super_type t1 t2) (List.combine t1s t2s)
		| _ -> raise (Stream.Error ("TODO"))
		
	and verify_structure_opt_is_super_type_or_make_it_super_type superTypeStructure_opt subTypeStructure_opt =
		match (superTypeStructure_opt, subTypeStructure_opt) with
		| (None, None) -> ()
		| (Some _, None) -> raise (Stream.Error ("TODO"))
		| (None, Some _) -> raise (Stream.Error ("TODO"))
		| (Some x, Some y) -> verify_structure_is_super_type_or_make_it_super_type x y
		
	and verify_is_super_type_or_make_it_super_type superTypeNumber subTypeNumber =
		let superType = gsm#get_type superTypeNumber in
		let subType = gsm#get_type subTypeNumber in
		
		verify_structure_opt_is_super_type_or_make_it_super_type superType.structure subType.structure
	in
	
	gsm#filter_map_superType_relations (fun (t1, t2) -> verify_is_super_type_or_make_it_super_type t1 t2; None);
	gsm#build
;;




class generic_system_realizer (gs: generic_system) (typeSystem: Type_system.type_system) =
	object(this)
	
	val genericMapping = Hashtbl.create 10
	
	method build_type t =
		match Hashtbl.find_opt genericMapping t with
		| None ->
			let real_type = this#build_type_impl t in
			Hashtbl.replace genericMapping t real_type;
			real_type
		| Some x -> x
		
	(* Private methods *)
	method private build_member_constraint (_, member, member_type) =
		let decl: Type_system.declaration = { name = member; typedef = this#build_type member_type } in
		decl
		
	method private build_type_from_structure structure =
		match structure with
		| Func(a, r) -> Type_system.Function(this#build_type a, this#build_type r)
		| Tuple ts -> Type_system.Tuple( (List.map (this#build_type) ts) )
		
	method private build_type_impl t =
		match (get_member_constraints t gs, get_std_type_constraints t gs, get_structure t gs) with
		| (constraints, [], None) ->
			let x = typeSystem#add_generic_type in
			Hashtbl.add genericMapping t (Type_system.Generic x);
			let mappedConstraints = (List.map (this#build_member_constraint) constraints) in
			typeSystem#set_generic_constraints x mappedConstraints;
			Type_system.Generic x
		| ([], (_, _, typedef)::[], None) -> typedef
		| ([], [], Some x) -> this#build_type_from_structure x
		| _ -> raise (Stream.Error (("TODO: " ^ string_of_int t)))
end;;



let find_common_type types gsb = List.fold_left (fun acc t -> gsb#must_be_equal_to t acc; acc) (List.hd types) (List.tl types);;

let build_final_type gsb fi (typeSystem: Type_system.type_system) =
	let combined = combine_equal_types gsb#build in
	let superTypesResolved = resolve_super_types combined in

	let gsr = new generic_system_realizer superTypesResolved typeSystem (*(fi#collect_all_types fi#get_self) fi typeSystem*) in
	print_endline ("---------------------------------------------------------------------------------------");
	print_endline ( to_string superTypesResolved );
	print_endline ("---------------------------------------------------------------------------------------");
	
	gsr#build_type fi#get_self
;;



let infer_function_type (rules: Semantic_ast.function_rule list) (context: SymbolTable.context) (typeSystem: Type_system.type_system) =
	let gsb = new generic_system_builder in
	let fi = new func_inferer context gsb in
	fi#init;
	
	let rec process_expr expr =
		match expr with
		| Semantic_ast.Self -> fi#get_self
		| Semantic_ast.Identifier id -> fi#get_name id
		| Semantic_ast.NaturalLiteral _ -> gsb#gen_type_from_std_type (Type_system.NamedRef("System", "nat"))
		| Semantic_ast.Call (func, arg) ->
			let funcType = process_expr func in
			let argType = process_expr arg in
			
			let (paramType, resultType) = gsb#must_have_function_structure funcType in
			gsb#must_be_assignable_to argType paramType;
			resultType
		| Semantic_ast.Select (select, member) ->
			let t = process_expr select in
			gsb#must_have_member t member
		| Semantic_ast.Tuple exprs -> gsb#gen_type (Tuple( List.map (process_expr) exprs ))
		| _ -> raise (Stream.Error ((Semantic_ast_print.expr_to_string expr)))
	in
	
	let process_expr_opt expr_opt =
		match expr_opt with
		| None -> None
		| Some expr -> Some (process_expr expr)
	in
	
	let rec add_generics_from_pattern pattern =
		match pattern with
		| Semantic_ast.Identifier global_name -> fi#add_arg global_name
		| Semantic_ast.NaturalLiteral _ -> ()
		| Semantic_ast.Tuple entries -> List.iter (add_generics_from_pattern) entries
		| _ -> raise (Stream.Error ((Semantic_ast_print.expr_to_string pattern)))
	in
	
	let process_func_rule (rule: Semantic_ast.function_rule) =
		add_generics_from_pattern rule.pattern;
		
		let patternType = process_expr rule.pattern in
		let conditionType = process_expr_opt rule.condition in
		let resultType = process_expr rule.body in
		
		match conditionType with
		| None -> (patternType, resultType)
		| Some c ->
			gsb#must_be_equal_to c (gsb#gen_type_from_std_type (Type_system.NamedRef("System", "bool")));
			(patternType, resultType)
	in
	
	let ruleTypes = List.map (process_func_rule) rules in
	let argType = find_common_type (List.map (fun (argType, _) -> argType) ruleTypes) gsb in
	let resultType = find_common_type (List.map (fun (_, resultType) -> resultType) ruleTypes) gsb in
	gsb#must_be_equal_to fi#get_self (gsb#gen_type (Func (argType, resultType)));
	build_final_type gsb fi typeSystem
;;
