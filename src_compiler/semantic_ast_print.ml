module IntSet = Set.Make(Int);;
module StringMap = Map.Make(String);;

let generics_to_string generics =
	let constraint_to_string (constraintName, typedef) =
		constraintName ^ ": " ^ (Type_system.to_string typedef)
	in
	let object_generic_to_string constraints =
		let cstr = String.concat "\n" (List.map (constraint_to_string) constraints#as_list) in
		"{\n" ^ cstr ^ "\n}"
	in
	let generic_parameter_to_string genericParameter =
		match genericParameter with
		| Type_system.Unspecified -> "?"
		| Type_system.Typedef typedef -> (Type_system.to_string typedef)
		| Type_system.ConstraintedObject constraints -> object_generic_to_string constraints
	in
	let generic_to_string (genericTypeNumber, gp) =
		Type_system.to_string (Generic genericTypeNumber) ^ ": " ^ (generic_parameter_to_string gp)
	in
	let generics_in_order = List.rev generics in
	String.concat "\n" (List.map (generic_to_string) generics_in_order)
;;


let module_generics_to_string _module symbolTable typeSystem =
	let collect_dependencies genericNumber =
		Generics.collect_generics_from_generics (IntSet.add genericNumber IntSet.empty) typeSystem
	in
	let rec build_simplified_type genericNumber referenced =
		let build_simplified_type_as_typedef genericNumber referenced =
			let gp = build_simplified_type genericNumber referenced in
			match gp with
			| Type_system.Unspecified -> Type_system.NamedRef ("?", "?")
			| Type_system.Typedef typedef -> typedef
			| Type_system.ConstraintedObject constraints -> constraints#as_object
		in
		
		let rec simplify typedef referenced =
			match typedef with
			| Type_system.Generic x ->
				if IntSet.mem x referenced then typedef
				else build_simplified_type_as_typedef x (IntSet.add x referenced)
			| Type_system.NamedRef _ -> typedef
			| Type_system.Function (argType, resultType) -> Type_system.Function (simplify argType referenced, simplify resultType referenced)
			| Type_system.Tuple entries -> Type_system.Tuple (List.map (fun entry -> simplify entry referenced) entries)
			| _ -> raise (Stream.Error (Type_system.to_string typedef))
		in
	
		let gp = typeSystem#get_generic genericNumber in
		match gp with
		| Type_system.Unspecified -> gp
		| Type_system.Typedef typedef -> Type_system.Typedef (simplify typedef referenced)
		| Type_system.ConstraintedObject constraints ->
			let map_constraint mapped_constraints (constraintName, _constraint) =
				mapped_constraints#add constraintName (simplify _constraint referenced);
				mapped_constraints
			in
			Type_system.ConstraintedObject (List.fold_left (map_constraint) (new Type_system.generic_constraints) constraints#as_list)
	in
	
	let used_generics = Generics.collect_module_generics _module symbolTable typeSystem in

	let dependencies = IntSet.fold (fun genericNumber acc -> (collect_dependencies genericNumber)::acc) used_generics [] in
	let common_dependencies = List.fold_left (fun set dependencies -> IntSet.diff set dependencies) used_generics dependencies in
	let common = IntSet.union used_generics common_dependencies in
	let simplifiedTypes = IntSet.fold (fun genericNumber acc -> (genericNumber, build_simplified_type genericNumber common)::acc) common [] in
	
	generics_to_string simplifiedTypes
	(*(generics_to_string transitive_generics typeSystem) ^ "\n---\n" ^ (String.concat "\n\n" (List.map (fun d -> generics_to_string d typeSystem) dependencies))*)
;;





let rec expr_to_string expression =
	match expression with
	| Semantic_ast.Identifier id -> id
	| Semantic_ast.NaturalLiteral number -> number
	| Semantic_ast.StringLiteral x -> "\"" ^ x ^ "\""
	| Semantic_ast.External externalName -> "extern \"" ^ externalName ^ "\""
	| Semantic_ast.Import moduleName -> "import \"" ^ moduleName ^ "\""
	| Semantic_ast.Call (func, arg) -> (expr_to_string func) ^ "(" ^ expr_to_string arg ^ ")"
	| Semantic_ast.Function(rules) -> "{\n" ^ (String.concat ",\n" (List.map (rule_to_string) rules)) ^ "\n}"
	| Semantic_ast.Object entries -> "{\n" ^ entries_to_string entries ^ "\n}"
	| Semantic_ast.Select (expr, member) -> (expr_to_string expr) ^ "." ^ member
	| Semantic_ast.Tuple values -> "(" ^ (exprs_to_string values) ^ ")"
	
and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)

and entry_to_string (entry: Semantic_ast.object_entry) = "\t" ^ entry.name ^ ": " ^ expr_to_string entry.expr
and entries_to_string entries = String.concat ",\n" (List.map (entry_to_string) entries)

and rule_to_string rule = "\t" ^ expr_to_string rule.pattern ^ " -> " ^ expr_to_string rule.body

let stmt_to_string stmt =
	match stmt with
	| Semantic_ast.ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	| Semantic_ast.LetBindingStatement (varName, typedef, expr) -> "let " ^ varName ^ ": " ^ (Type_system.to_string typedef) ^ " := " ^ (expr_to_string expr) ^ ";"
	| Semantic_ast.TypeDefStatement (id, typedef) -> "type " ^ id ^ " := \n" ^ (Type_system.to_string typedef) ^ ";"
	| Semantic_ast.UseStatement id -> "use \"" ^ id ^ "\";"
;;

let stmts_to_string stmts = String.concat "\n" (List.map (stmt_to_string) stmts);;


let exports_to_string exports = String.concat "\n" (StringMap.fold (fun k (v: Semantic_ast.export) acc -> (k ^ ": " ^ (Type_system.to_string v.typedef))::acc) exports []);;

let to_string (_module: Semantic_ast.program_module) symbolTable typeSystem =
	let stmts = stmts_to_string _module.statements in
	stmts ^ "\n\nGENERICS:\n" ^ (module_generics_to_string _module symbolTable typeSystem) ^ "\n\nEXPORT TABLE:\n" ^ (exports_to_string _module.exports)
;;
