module StringMap = Map.Make(String);;

type value =
	| Parameter
	| ExternalValue of string
	| Function of int
	| NaturalValue of string
	| StringValue of string
	| Call of value * value
	| Select of value * string
	| Tuple of value list

type statement = 
	| LetBindingStatement of string * Ast.typedefinition * value
	| ValueStatement of value
	
type program_module = {
	moduleName: string;
	functions: statement list list;
	publicTypes: (string * Ast.typedefinition) list;
	exports: value StringMap.t;
}


let rec value_to_string expr =
	match expr with
	| Call (func, arg) -> (value_to_string func) ^ "(" ^ value_to_string arg ^ ")"
	| ExternalValue externalName -> "extern \"" ^ externalName ^ "\""
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
	| LetBindingStatement (varName, typedef, value) -> "let " ^ varName ^ ": " ^ (Ast.typedef_to_string typedef) ^ " := " ^ (value_to_string value) ^ ";"
	| ValueStatement expr -> (value_to_string expr) ^ ";"
;;

let stmts_to_string stmts = String.concat "\n" (List.map (stmt_to_string) stmts);;

let types_to_string types = String.concat "\n" (List.map (fun (name, typedef) -> "type " ^ name ^ " := " ^ (Ast.typedef_to_string typedef) ^ ";") types);;

let exports_to_string exports = String.concat "\n" (StringMap.fold (fun k v acc -> (k ^ ": " ^ (value_to_string v))::acc) exports []);;

let to_string _module =
	let rec funcs_to_string funcs num =
		match funcs with
		| [] -> ""
		| first::rest -> "function $f" ^ (string_of_int num) ^ "($p)\n{\n" ^ (stmts_to_string first) ^ "\n}\n\n" ^ (funcs_to_string rest (num+1))
	in
	
	let types = types_to_string _module.publicTypes
	in
	
	let funcs = funcs_to_string _module.functions 0
	in
	
	types ^ "\n" ^ funcs ^ "\n\nEXPORT TABLE:\n" ^ (exports_to_string _module.exports)
;;
