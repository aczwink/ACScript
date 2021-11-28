let () = Printexc.record_backtrace true;;


let stream_concat streams =
    let current_stream = ref None in
    let rec next i =
      try
        let stream =
          match !current_stream with
          | Some stream -> stream
          | None ->
             let stream = Stream.next streams in
             current_stream := Some stream;
             stream in
        try Some (Stream.next stream)
        with Stream.Failure -> (current_stream := None; next i)
      with Stream.Failure -> None in
    Stream.from next;;
    
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
	(*let stream = stream_concat (Stream.of_list [stream1; stream2]) in*)
	let lexer = Lexer.lex stream in
	let parsed_module = parse_with_error_handling lexer stream in
	close_in ic;
	parsed_module
;;

let dump_ast ast =
	let outputFilePath = "_AST.acs" in
	print_endline ("Dumping AST to: " ^ outputFilePath);
	let oc = open_out (outputFilePath) in
		output_string oc (Ast.to_string ast);
		close_out oc
;;

let dump_after_semantic_analysis _module =
	let outputFilePath = "_semantic.txt" in
	print_endline ("Dumping results of semantic analysis to: " ^ outputFilePath);
	let oc = open_out (outputFilePath) in
		output_string oc (SemanticAnalysis.to_string _module);
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

(*
let () =
	let inputFilePath = Sys.argv.( (Array.length Sys.argv) - 1) in
	let ast = parse_file inputFilePath in
		dump_ast ast;
		let sa = SemanticAnalysis.translate ast in
			dump_after_semantic_analysis sa;
			let js = Sys.argv.(1) = "--js" in
				if js then output_js inputFilePath ast else output_ir inputFilePath ast
*)

let parse_module_and_dependencies moduleName _ =
	print_endline moduleName
;;

let () =
	let inputFilePath = Sys.argv.( (Array.length Sys.argv) - 1) in
	let inputDir = Filename.dirname inputFilePath in
	let includeDirectories = [inputDir] in
	let mainModuleName = Filename.remove_extension(Filename.basename inputFilePath) in
	parse_module_and_dependencies mainModuleName includeDirectories
