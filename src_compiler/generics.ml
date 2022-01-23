module IntSet = Set.Make(Int);;

let rec collect_generics_from_type typedef typeSystem =
	match typedef with
	| Type_system.Any -> IntSet.empty
	| Type_system.Generic x -> IntSet.add x IntSet.empty
	| Type_system.NamedRef _ -> IntSet.empty
	| Type_system.Function (argType, resultType) -> collect_generics_from_types (argType::[resultType]) typeSystem
	| Type_system.Object decls -> collect_generics_from_types (List.map (fun (decl: Type_system.declaration) -> decl.typedef) decls) typeSystem
	| Type_system.Tuple entries -> collect_generics_from_types entries typeSystem
	| _ -> raise (Stream.Error (Type_system.to_string typedef))
	
and collect_generics_from_types types typeSystem =
	List.fold_left (fun set entry -> IntSet.union set (collect_generics_from_type entry typeSystem)) IntSet.empty types
	
and collect_generics_from_generics generics typeSystem =
	let collect_generics_from_generic genericNumber =
		match typeSystem#get_generic genericNumber with
		| Type_system.Unspecified -> IntSet.empty
		| Type_system.Typedef typedef -> collect_generics_from_type typedef typeSystem
		| Type_system.ConstraintedObject constraints -> collect_generics_from_type (constraints#as_object) typeSystem
	in
	let collect_generics_from_generic_and_merge genericNumber set =
		IntSet.union set (collect_generics_from_generic genericNumber)
	in
	let merged = IntSet.fold (collect_generics_from_generic_and_merge) generics generics in
	let newGenerics = IntSet.diff merged generics in
	if IntSet.is_empty newGenerics then merged else collect_generics_from_generics (IntSet.union merged newGenerics) typeSystem
	
and collect_generics_from_type_transitive typedef typeSystem =
	collect_generics_from_generics (collect_generics_from_type typedef typeSystem) typeSystem
;;

let collect_expr_generics expr symbolTable typeSystem =	
	match expr with
	| Semantic_ast.Identifier global_name -> collect_generics_from_type (symbolTable#get_type global_name) typeSystem
	| Semantic_ast.NaturalLiteral _ -> IntSet.empty
	| Semantic_ast.StringLiteral _ -> IntSet.empty 
	| Semantic_ast.External _ -> IntSet.empty
	| Semantic_ast.Import _ -> IntSet.empty
	| Semantic_ast.Call _ -> IntSet.empty
	| Semantic_ast.Function _ -> IntSet.empty
	| Semantic_ast.Object _ -> IntSet.empty
	| Semantic_ast.Select _ -> IntSet.empty
	| Semantic_ast.Tuple _ -> IntSet.empty
;;

let collect_expr_generics_transitive expr symbolTable typeSystem =
	collect_generics_from_generics (collect_expr_generics expr symbolTable typeSystem) typeSystem
;;

let collect_stmt_generics stmt symbolTable typeSystem =
	match stmt with
	| Semantic_ast.ExpressionStatement expr -> collect_expr_generics expr symbolTable typeSystem
	| LetBindingStatement (_, typedef, _) -> collect_generics_from_type typedef typeSystem
	| TypeDefStatement _ -> IntSet.empty
	| UseStatement _ -> IntSet.empty
;;


let collect_module_generics (_module: Semantic_ast.program_module) symbolTable typeSystem =
	List.fold_left (fun set stmt -> IntSet.union set (collect_stmt_generics stmt symbolTable typeSystem)) IntSet.empty _module.statements
;;
