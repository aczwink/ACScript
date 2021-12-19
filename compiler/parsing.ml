let read_next_symbol stream =
	try Stream.next stream
	with _ -> '\x00'
;;
    
let rec read_line stream =
		let next = read_next_symbol stream
		in
		match next with
		| '\x00' -> ""
		| '\n' -> "\n"
		| c ->
			let s1 = (String.make 1 c) in
			let s2 = read_line stream in
			s1 ^ s2
;;

let rec read_lines nLines stream =
	match nLines with
	| 0 -> ""
	| _ ->
		let next_line = (read_line stream) in
		let other_lines = (read_lines (nLines-1) stream) in
		next_line ^ other_lines
;;
    

let parse_with_error_handling lexer stream =
	try Parser.parse_module lexer
	with e ->
		Printf.eprintf "Parsing error just before:\n%s\n" (read_lines 4 stream);
		raise e
;;

let parse_file filePath =
	let ic = open_in filePath in
	let stream = (Stream.of_channel ic) in
	let lexer = Lexer.lex stream in
	let parsed_module = parse_with_error_handling lexer stream in
	close_in ic;
	parsed_module
;;

let rec find_module_path moduleName includeDirectories =
	match includeDirectories with
	| [] -> raise (Stream.Error ("Can't find module: " ^ moduleName))
	| dir::rest ->
		let path = Filename.concat dir (moduleName ^ ".acs") in
		if Sys.file_exists path then path else find_module_path moduleName rest
;;


let rec find_module_dependencies parsedModule =
	match parsedModule with
	| [] -> []
	| (Ast.ImportToplevel moduleName)::rest -> moduleName::(find_module_dependencies rest)
	| _::rest -> find_module_dependencies rest
;;


let parse_module_and_dependencies moduleName includeDirectories =
	let parsedModules: (string, Ast.toplevel list) Hashtbl.t = Hashtbl.create 10
	in
	
	let rec parse_module_and_add_to_dict moduleName =
		let path = find_module_path moduleName includeDirectories in
		let parsedModule = parse_file path in
		Hashtbl.add parsedModules moduleName parsedModule;
		let dependencies = find_module_dependencies parsedModule in
		parse_modules_if_not_done_already dependencies
		
	and parse_module_if_not_done_already moduleName =
		match Hashtbl.find_opt parsedModules moduleName with
		| None -> parse_module_and_add_to_dict moduleName
		| _ -> ()
		
	and parse_modules_if_not_done_already moduleNames =
		match moduleNames with
		| [] -> ()
		| moduleName::rest -> parse_module_if_not_done_already moduleName; parse_modules_if_not_done_already rest
	in
	
	let order_modules modulesTable =
		let processedModules: (string, unit) Hashtbl.t = Hashtbl.create 10
		in
		
		let was_module_processed moduleName = 
			match Hashtbl.find_opt processedModules moduleName with
			| None -> false
			| _ -> true
		in
			
		let rec add_module moduleName =
			let parsedModule = Hashtbl.find modulesTable moduleName in
			let dependencies = find_module_dependencies parsedModule in
			Hashtbl.add processedModules moduleName ();
			(order_modules_inner dependencies) @ [(moduleName, parsedModule)]

		and order_modules_inner moduleNames =
			match moduleNames with
			| [] -> []
			| moduleName::rest ->
				let next = if was_module_processed moduleName then (order_modules_inner rest) else (add_module moduleName) in
				next @ (order_modules_inner rest)
		in
		
		let moduleNames = Hashtbl.fold (fun moduleName _ acc -> moduleName::acc) modulesTable [] in
		order_modules_inner moduleNames
	in
	
	parse_module_and_add_to_dict moduleName;
	order_modules parsedModules
;;
