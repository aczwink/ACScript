(*let () = Printexc.record_backtrace true;;*)
let () = Printexc.record_backtrace true;;

type cmd_args = { includeDirectories: string list; target: string; mainModuleName: string };;
let parse_command_line_args =
	let rec parse_next_arg args =
		match args with
		| "-i"::rest ->
			let result = parse_next_arg (List.tl rest)
			in
				{ includeDirectories = result.includeDirectories @ [List.hd rest]; target = result.target; mainModuleName = result.mainModuleName }
		| "--target"::rest ->
			let result = parse_next_arg (List.tl rest)
			in
				{ includeDirectories = result.includeDirectories; target = (List.hd rest); mainModuleName = result.mainModuleName }
		| inputFilePath::[] ->
			let inputDir = Filename.dirname inputFilePath in
			let mainModuleName = Filename.remove_extension(Filename.basename inputFilePath) in
			{ includeDirectories = [inputDir]; target = "acsb"; mainModuleName = mainModuleName }
		| _ -> raise (Stream.Error (List.hd args))
	in
	
	let argsWithoutProgramName = (List.tl (Array.to_list Sys.argv)) in
	let cmd = parse_next_arg argsWithoutProgramName in
	let mainModuleIncludeDir = List.hd cmd.includeDirectories in
	let otherIncludeDirs = List.tl cmd.includeDirectories in
		{ includeDirectories = mainModuleIncludeDir::(List.rev otherIncludeDirs); target = cmd.target; mainModuleName = cmd.mainModuleName }
;;


let dump_ast moduleName parsedModule =
	let outputFilePath = moduleName ^ ".ast.txt" in
	let oc = open_out (outputFilePath) in
		output_string oc (Ast.to_string parsedModule);
		close_out oc
;;


let dump_sast _module step symbolTable typeSystem =
	let outputFilePath = _module.Semantic_ast.moduleName ^ "." ^ step ^ ".txt" in
	let oc = open_out (outputFilePath) in
		output_string oc (Semantic_ast_print.to_string _module symbolTable typeSystem);
		close_out oc
;;


let process_modules modules =
	let process_module modulesCollection (moduleName, parsedModule) =
		dump_ast moduleName parsedModule;
		let _module = SemanticAnalysis.translate moduleName parsedModule modulesCollection in
		dump_sast _module "semantic" modulesCollection#get_symbol_table modulesCollection#get_type_system;
		let _simplifiedModule = Simplified_ast.simplify _module modulesCollection in
		dump_sast _simplifiedModule "simplified" modulesCollection#get_symbol_table modulesCollection#get_type_system;
		modulesCollection#add_module moduleName _simplifiedModule;
		modulesCollection
	in
	
	let modulesCollection = new ModulesCollection.modules_collection in	
	List.fold_left (process_module) modulesCollection modules
;;

let dump_code target =
	match target with
	| "acsb" -> AcsbTarget.compile
	(*| "node" -> NodeJsCodeDump.dump_module*)
	| _ -> raise (Stream.Error ("Unknown target: " ^ target))
;;

let () =
	let cmd = parse_command_line_args in
	let modules = Parsing.parse_module_and_dependencies cmd.mainModuleName cmd.includeDirectories in
	let analyzedModules = process_modules modules in
	(dump_code cmd.target) cmd.mainModuleName analyzedModules
