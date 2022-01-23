module StringMap = Map.Make(String);;

exception ParsingException of string

class parsed_modules_collection =
	object
	
	val mutable moduleNamesList = []
	val mutable modulesMap: Ast.statement list StringMap.t = StringMap.empty
	
	method add_module moduleName _module =
		modulesMap <- StringMap.add moduleName _module modulesMap;
		moduleNamesList <- moduleNamesList @ [moduleName]
		
	method has_module moduleName =
		StringMap.mem moduleName modulesMap
		
	method modules_in_order =
		List.map (fun moduleName -> (moduleName, StringMap.find moduleName modulesMap)) moduleNamesList
end;;

let rec find_module_dependencies parsedModule =
	match parsedModule with
	| [] -> []
	| (Ast.UseStatement moduleName)::rest -> moduleName::(find_module_dependencies rest)
	| _::rest -> find_module_dependencies rest
;;

let rec find_module_path moduleName includeDirectories =
	match includeDirectories with
	| [] -> raise (Stream.Error ("Can't find module: " ^ moduleName))
	| dir::rest ->
		let path = Filename.concat dir (moduleName ^ ".acs") in
		if Sys.file_exists path then path else find_module_path moduleName rest
;;

let print_error_position lexbuf filePath =
	let pos = lexbuf.Lexing.lex_curr_p in
	"Line: " ^(string_of_int pos.pos_lnum) ^ " Position: " ^ (string_of_int (pos.pos_cnum - pos.pos_bol + 1)) ^ " File: " ^ filePath

let parse_with_error_handling lexbuf filePath =
	try(Parser.program_module Lexer.read_token lexbuf) with
	| Lexer.SyntaxError msg ->
		raise (ParsingException (msg ^ " at " ^ (print_error_position lexbuf filePath)))
  | Parser.Error ->
		raise (ParsingException ("Syntax error at " ^ (print_error_position lexbuf filePath)))
;;

let parse_file filePath =
	let ic = open_in filePath in
	let lexbuf = Lexing.from_channel ic in
	let parsed_module = parse_with_error_handling lexbuf filePath in
	close_in ic;
	parsed_module
;;

let parse_module_and_dependencies moduleName includeDirectories =
	let parsedModules = new parsed_modules_collection
	in
	
	let rec parse_module_and_add_to_dict moduleName =
		let path = find_module_path moduleName includeDirectories in
		let parsedModule = parse_file path in
		let dependencies = find_module_dependencies parsedModule in
		parse_modules_if_not_done_already dependencies;
		parsedModules#add_module moduleName parsedModule
		
	and parse_module_if_not_done_already moduleName =
		if parsedModules#has_module moduleName then () else parse_module_and_add_to_dict moduleName
		
	and parse_modules_if_not_done_already moduleNames =
		match moduleNames with
		| [] -> ()
		| moduleName::rest -> parse_module_if_not_done_already moduleName; parse_modules_if_not_done_already rest
	in
	
	parse_module_and_add_to_dict moduleName;
	parsedModules#modules_in_order
;;
