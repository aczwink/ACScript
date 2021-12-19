module StringMap = Map.Make(String);;


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
	let outputFilePath = moduleName ^ ".ast" in
	let oc = open_out (outputFilePath) in
		output_string oc (Ast.to_string parsedModule);
		close_out oc
;;

let dump_after_semantic_analysis _module =
	let outputFilePath = _module.ProgramModule.moduleName ^ ".semantic" in
	let oc = open_out (outputFilePath) in
		output_string oc (ProgramModule.to_string _module);
		close_out oc
;;

let output_ir inputFilePath ast = 
	let instrs = IrTranslator.translate_ast ast in
		print_endline ("");
		print_endline ("IR Code:");
		print_endline (Ir.instructions_to_string instrs);
		Codedump.dump_module instrs inputFilePath
;;

let output_js inputFilePath ast =
	Jscodedump.dump_module inputFilePath ast
;;


let proces_modules modules =	
	let process_module modulesMap (moduleName, parsedModule) =
		dump_ast moduleName parsedModule;
		let _module = SemanticAnalysis.translate moduleName parsedModule modulesMap in
		dump_after_semantic_analysis _module;
		StringMap.add moduleName _module modulesMap
	in
	
	List.fold_left (process_module) StringMap.empty modules
;;

let dump_code target _ =
	match target with
	| "acsb" -> raise (Stream.Error ("Not implemented: reimplement output_ir"))
	| "node" -> raise (Stream.Error ("Not implemented: reimplement output_js"))
	| _ -> raise (Stream.Error ("Unknown target: " ^ target))
;;

let () =
	let cmd = parse_command_line_args in
	let modules = Parsing.parse_module_and_dependencies cmd.mainModuleName cmd.includeDirectories in
	let analyzedModules = proces_modules modules in
	dump_code cmd.target analyzedModules
