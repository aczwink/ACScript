type value =
	| Parameter
	| ExternalCall of string
	| Function of int
	| NaturalValue of string
	| StringValue of string
	| Call of value * value
	| Select of value * string
	| Tuple of value list

type statement = 
	| ValueStatement of value
	
module Scope = Map.Make(String);;

type program_module = {
	functions: statement list list;
}
	
	
let rec value_to_string expr =
	match expr with
	| Call (func, arg) -> (value_to_string func) ^ "(" ^ value_to_string arg ^ ")"
	| ExternalCall externalName -> "$e_" ^ externalName
	| Function funcIdx -> "$f" ^ string_of_int(funcIdx)
	| NaturalValue x -> x
	| Parameter -> "$p"
	| Select (value, member) -> (value_to_string value) ^ "." ^ member
	| StringValue x -> "\"" ^ x ^ "\""
	| Tuple entries -> "(" ^ (values_to_string entries) ^ ")"
	
and values_to_string values = String.concat ", " (List.map (value_to_string) values)
;;

let stmt_to_string stmt =
	match stmt with
	| ValueStatement expr -> (value_to_string expr) ^ ";"
;;

let stmts_to_string stmts = String.concat "\n" (List.map (stmt_to_string) stmts);;

let to_string _module =
	let rec funcs_to_string funcs num =
		match funcs with
		| [] -> ""
		| first::rest -> "function $f" ^ (string_of_int num) ^ "($p)\n{\n" ^ (stmts_to_string first) ^ "\n}\n\n" ^ (funcs_to_string rest (num+1))
	in
	
	funcs_to_string _module.functions 0
;;




let rec order_functions funcs currentNumber =
	match currentNumber < (Hashtbl.length funcs) with
	| false -> []
	| true -> (Hashtbl.find funcs currentNumber)::(order_functions funcs (currentNumber+1))
;;



let translate _moduleAst =
	let namedTypes: (string, Ast.typedefinition) Hashtbl.t = Hashtbl.create 10
	in
	let functions: (int, statement list) Hashtbl.t = Hashtbl.create 10
	in
	
	let translate_identifier id scope =
		let found = Scope.find_opt id scope in
			match found with
			| Some value -> value
			| None -> raise (Stream.Error ("Unknown variable: " ^ id))
	in
	
	let translate_pattern pattern scope =
		match pattern with
		| Ast.Identifier id -> Scope.add id Parameter scope
		| _ -> raise (Stream.Error "call not implemented")
	in
	
	let rec translate_expr expr scope =
		match expr with
		| Ast.Identifier id -> translate_identifier id scope
		| Ast.NaturalLiteral x -> NaturalValue(x)
		| Ast.StringLiteral x -> StringValue(x)
		| Ast.Call (func, arg) -> translate_call func arg scope
		| Ast.Select (expr, memberName) -> Select(translate_expr expr scope, memberName)
		| Ast.Tuple exprs -> Tuple( List.map (fun expr -> translate_expr expr scope) exprs )
		| Ast.Function (pattern, expr) ->
			let funcScope = translate_pattern pattern scope in
			let stmt = ValueStatement(translate_expr expr funcScope) in
			let funcIdx = Hashtbl.length functions in
			Hashtbl.add functions funcIdx [stmt];
			Function(funcIdx)
		
	and translate_call func arg scope =
		match func with
		| Ast.Identifier "extern" -> translate_external_call arg
		| _ -> Call(translate_expr func scope, translate_expr arg scope)
	
	and translate_external_call arg =
		match arg with
		| Ast.StringLiteral x -> ExternalCall(x)
		| _ -> raise (Stream.Error ("Externals can only be referenced with a constant string literal but got: " ^ (Ast.expr_to_string arg)))
	in
	
	let translate_stmt stmt scope =
		match stmt with
		| Ast.ExpressionStatement expr -> Some(ValueStatement(translate_expr expr scope)), scope
		| Ast.ImportStatement _ -> raise (Stream.Error "call not implemented")
		| Ast.LetBindingStatement (name, _, expr) ->
			let translated_expr = translate_expr expr scope in
			Some(ValueStatement(translated_expr)), Scope.add name translated_expr scope
		| Ast.TypeDefStatement (name, typedef) -> Hashtbl.add namedTypes name typedef; None, scope
	in
	
	let rec translate_stmts stmts scope =
		match stmts with
		| [] -> []
		| stmt::rest ->
			let (result, newScope) = (translate_stmt stmt scope) in
				match result with
				| Some(translated_stmt) -> translated_stmt::(translate_stmts rest newScope)
				| None -> translate_stmts rest newScope
	in
	
	let stmts = translate_stmts _moduleAst Scope.empty in
	Hashtbl.add functions (Hashtbl.length functions) stmts;
	let orderedFunctions = order_functions functions 0 in
	{ functions = orderedFunctions }
;;
