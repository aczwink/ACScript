let is_binary_tuple expr =
	match expr with
	| Ast.Tuple args -> (List.length args) = 2
	| _ -> false
;;

let is_operator externalName =
	match externalName with
	| "*" -> true
	| _ -> false
;;

let is_binop func arg =
	match func with
	| Ast.Identifier externalName -> (is_operator externalName) && (is_binary_tuple arg)
	| _ -> false
;;

let translate_external name =
	match name with
	| "print" -> "console.log"
	| "*" -> "*"
	| _ -> name
;;

let translate_call func =
	match func with
	| Ast.Identifier externalName -> translate_external externalName
	| _ -> raise (Stream.Error "call not implemented") 
;;

let translate_pattern expr =
	match expr with
	| Ast.Identifier name -> "const " ^ name ^ " = arg"
	| _ -> raise (Stream.Error "call not implemented")
;;


let rec translate_binop func arg =
	let extractFuncName =
		match func with
		| Ast.Identifier externalName -> externalName
		| _ -> raise (Stream.Error ("Can't happen"))
	in
	
	let extractArgs = 
		match arg with
		| Ast.Tuple args -> args
		| _ -> raise (Stream.Error ("Can't happen"))
	in
	
	(translate_expr (List.hd extractArgs)) ^ extractFuncName ^ (translate_expr (List.hd (List.tl extractArgs)))
	
and translate_expr expr =
	match expr with
	| Ast.Call (func, arg) ->
		if is_binop func arg then translate_binop func arg else (translate_call func) ^ "(" ^ (translate_expr arg) ^ ")"
	| Ast.Identifier identifier -> identifier
	| Ast.NaturalLiteral literal -> literal ^ "n"
	| Ast.StringLiteral x -> "\"" ^ x ^ "\""
	| Ast.Tuple args -> "[" ^ (translate_exprs args) ^ "]"
	| Ast.Function (pattern, expr) -> "function(arg)\n{\n\t" ^ (translate_pattern pattern) ^ ";\n\treturn " ^ (translate_expr expr) ^ ";\n}"
and translate_exprs exprs = String.concat "," (List.map (translate_expr) exprs)
;;

let translate_stmt stmt = 
	match stmt with
	| Ast.ExpressionStatement expr -> (translate_expr expr) ^ ";"
	| Ast.LetBindingStatement (name, expr) -> "const " ^ name ^ " = " ^ (translate_expr expr) ^ ";"
;;
		
let translate_stmts_string stmts =
	String.concat "\n" (List.map (translate_stmt) stmts)
;;

let dump_module inputFilePath _moduleAst = 
	let oc = open_out (inputFilePath ^ ".js") in		
		let code = translate_stmts_string _moduleAst in
			output_string oc code;
			close_out oc
