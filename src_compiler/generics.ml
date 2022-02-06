module IntSet = Set.Make(Int);;

let collect_generics_from_type typedef typeSystem =
	let rec collect_inner typedef set =
		match typedef with
		| Type_system.Any -> set
		| Type_system.Unknown -> set
		| Type_system.Generic x ->
			if IntSet.mem x set then set
			else
				collect_inner_multiple_decls (typeSystem#get_generic_constraints x) (IntSet.add x set)
		| Type_system.NamedRef _ -> set
		| Type_system.Function (argType, resultType) -> collect_inner_multiple (argType::[resultType]) set
		| Type_system.Object decls -> collect_inner_multiple_decls decls set
		| Type_system.Tuple entries -> collect_inner_multiple entries set
		
	and collect_inner_multiple types set =
		List.fold_left (fun set entry -> collect_inner entry set) set types
		
	and collect_inner_multiple_decls decls set =
		collect_inner_multiple (List.map (fun (decl: Type_system.declaration) -> decl.typedef) decls) set
	in
	collect_inner typedef IntSet.empty
;;


let rec collect_expr_generics expr symbolTable typeSystem =	
	match expr with
	| Semantic_ast.Self -> IntSet.empty
	| Semantic_ast.Identifier global_name -> collect_generics_from_type (symbolTable#get_type global_name) typeSystem
	| Semantic_ast.NaturalLiteral _ -> IntSet.empty
	| Semantic_ast.StringLiteral _ -> IntSet.empty
	| Semantic_ast.UnsignedLiteral _ -> IntSet.empty
	| Semantic_ast.External _ -> IntSet.empty
	| Semantic_ast.Import _ -> IntSet.empty
	| Semantic_ast.Call _ -> IntSet.empty
	| Semantic_ast.Function _ -> IntSet.empty
	| Semantic_ast.Object _ -> IntSet.empty
	| Semantic_ast.Select _ -> IntSet.empty
	| Semantic_ast.Tuple entries -> List.fold_left (fun set entry -> IntSet.union set (collect_expr_generics entry symbolTable typeSystem)) IntSet.empty entries
;;

let collect_stmt_generics stmt symbolTable typeSystem =
	match stmt with
	| Semantic_ast.ExpressionStatement expr -> collect_expr_generics expr symbolTable typeSystem
	| LetBindingStatement (_, typedef, _) -> collect_generics_from_type typedef typeSystem
	| TypeDefStatement _ -> IntSet.empty
	| UseStatement _ -> IntSet.empty
;;


let collect_module_generics (_module: Semantic_ast.program_module) symbolTable (typeSystem: Type_system.type_system) =
	List.fold_left (fun set stmt -> IntSet.union set (collect_stmt_generics stmt symbolTable typeSystem)) IntSet.empty _module.statements
;;
