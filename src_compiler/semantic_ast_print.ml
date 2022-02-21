module IntSet = Set.Make(Int);;
module StringMap = Map.Make(String);;

let generics_to_string (generics: int list) (typeSystem: Type_system.type_system) =
	let generic_to_string genericTypeNumber =
		Type_system.generic_to_string genericTypeNumber (typeSystem#get_generic_constraints genericTypeNumber)
	in
	let generics_in_order = List.rev generics in
	String.concat "\n" (List.map (generic_to_string) generics_in_order)
;;


let module_generics_to_string (_module: Semantic_ast.program_module) (symbolTable: SymbolTable.symbol_table) (typeSystem: Type_system.type_system) =
	let used_generics = Generics.collect_module_generics _module symbolTable typeSystem in
	let genericsList = IntSet.fold (fun genericNumber acc -> genericNumber::acc) used_generics [] in
	generics_to_string genericsList
;;





let rec expr_to_string expression =
	match expression with
	| Semantic_ast.Self -> "self"
	| Semantic_ast.Identifier id -> id
	| Semantic_ast.NaturalLiteral number -> number
	| Semantic_ast.StringLiteral x -> "\"" ^ x ^ "\""
	| Semantic_ast.UnsignedLiteral number -> number ^ "u"
	| Semantic_ast.External externalName -> "extern \"" ^ externalName ^ "\""
	| Semantic_ast.Import moduleName -> "import \"" ^ moduleName ^ "\""
	| Semantic_ast.Call (func, arg) -> (expr_to_string func) ^ "(" ^ expr_to_string arg ^ ")"
	| Semantic_ast.Function(_, rules) -> "{\n" ^ (String.concat ",\n" (List.map (rule_to_string) rules)) ^ "\n}"
	| Semantic_ast.Dictionary entries -> "{\n" ^ entries_to_string entries ^ "\n}"
	| Semantic_ast.Select (expr, member) -> (expr_to_string expr) ^ "." ^ member
	| Semantic_ast.Tuple values -> "(" ^ (exprs_to_string values) ^ ")"
	
and exprs_to_string exprs = String.concat ", " (List.map (expr_to_string) exprs)

and entry_to_string (entry: Semantic_ast.dict_entry) = "\t" ^ entry.name ^ ": " ^ expr_to_string entry.expr
and entries_to_string entries = String.concat ",\n" (List.map (entry_to_string) entries)

and rule_to_string rule =
	let cond_to_string =
		match rule.condition with
		| None -> ""
		| Some x -> " | " ^ expr_to_string x
	in
	"\t" ^ expr_to_string rule.pattern ^ cond_to_string ^ " -> " ^ expr_to_string rule.body

let stmt_to_string stmt =
	match stmt with
	| Semantic_ast.ExpressionStatement expr -> (expr_to_string expr) ^ ";"
	| Semantic_ast.LetBindingStatement (varName, typedef, expr) -> "let " ^ varName ^ ": " ^ (Type_system.to_string typedef) ^ " := " ^ (expr_to_string expr) ^ ";"
	| Semantic_ast.TypeDefStatement (id, typedef) -> "type " ^ id ^ " := \n" ^ (Type_system.to_string typedef) ^ ";"
	| Semantic_ast.UseStatement id -> "use \"" ^ id ^ "\";"
;;

let stmts_to_string stmts = String.concat "\n" (List.map (stmt_to_string) stmts);;


let exports_to_string exports = String.concat "\n" (StringMap.fold (fun k (v: Semantic_ast.export) acc -> (k ^ ": " ^ (Type_system.to_string v.typedef))::acc) exports []);;

let to_string (_module: Semantic_ast.program_module) symbolTable (typeSystem: Type_system.type_system) =
	let stmts = stmts_to_string _module.statements in
	let generics = module_generics_to_string _module symbolTable typeSystem typeSystem in
	stmts ^ "\n\nGENERICS:\n" ^ generics ^ "\n\nEXPORT TABLE:\n" ^ (exports_to_string _module.exports)
;;
