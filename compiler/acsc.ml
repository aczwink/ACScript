let () = Printexc.record_backtrace true;;

let parse_file filePath =
	let ic = open_in filePath in
	let stream = Lexer.lex (Stream.of_channel ic) in
	let parsed_module = Parser.parse_module stream in
	close_in ic;
	parsed_module
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

let () =
	let inputFilePath = Sys.argv.( (Array.length Sys.argv) - 1) in
	let ast = parse_file inputFilePath in
		print_endline ("AST:");
		print_endline (Ast.to_string ast);
	let js = Sys.argv.(1) = "--js" in
		if js then output_js inputFilePath ast else output_ir inputFilePath ast
