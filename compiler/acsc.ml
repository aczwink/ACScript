let () = Printexc.record_backtrace true;;

let parse_file filePath =
	let ic = open_in filePath in
	let stream = Lexer.lex (Stream.of_channel ic) in
	let parsed_module = Parser.parse_module stream in
	close_in ic;
	parsed_module
;;

let () =
	let inputFilePath = Sys.argv.(1) in
	let ast = parse_file inputFilePath in
	let instr = IrTranslator.translate_ast ast in
		print_endline ("AST:");
		print_endline (Ast.to_string ast);
		print_endline ("");
		print_endline ("IR Code:");
		print_endline (Ir.instruction_to_string instr);
		Codedump.dump_module instr inputFilePath