let rec lex = parser
	(* Skip whitespaces *)
	| [< ' (' ' | '\n' | '\r' | '\t'); stream >] -> lex stream
	
	(* line comments *)
	| [< ' ('#'); stream >] ->
		lex_line_comment stream
		
	(* number *)
	| [< ' ('0' .. '9' as c); stream >] ->
		let buffer = Buffer.create 1 in
		Buffer.add_char buffer c;
		lex_number buffer stream
		
	(* text literal *)
	| [< ' ('"'); stream >] ->
		let buffer = Buffer.create 0 in
		lex_text_literal buffer stream
      
	(* identifier *)
	| [< ' ('A' .. 'Z' | 'a' .. 'z' | '*' as c); stream >] ->
		let buffer = Buffer.create 1 in
		Buffer.add_char buffer c;
		lex_ident buffer stream
		
	(* special symbols *)
	| [< ' (':'); stream >] -> lex_colon stream
	| [< ' ('-'); ' ('>'); stream >] -> [< 'Token.Map; lex stream >]
	
	| [< ' ('(' | ')' | ';' as c); stream >] -> [< 'Token.Symbol c; lex stream >]
		
	(* end of stream *)
	| [< >] -> [< >]
	
and lex_number buffer = parser
  | [< ' ('0' .. '9' | '.' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_number buffer stream
  | [< stream=lex >] ->
      [< 'Token.NaturalLiteral (Buffer.contents buffer); stream >]
	
and lex_ident buffer = parser
	| [< ' ('A' .. 'Z' | 'a' .. 'z' | '0' .. '9' as c); stream >] ->
		Buffer.add_char buffer c;
		lex_ident buffer stream
	| [< stream=lex >] ->
		let ident = Buffer.contents buffer in
		match ident with
		| "let" -> [< 'Token.Let; stream >]
		| _ -> [< 'Token.Identifier ident; stream >]
		
and lex_text_literal buffer = parser
	| [< ' ('"'); stream=lex >] -> [< 'Token.StringLiteral (Buffer.contents buffer); stream >]
	| [< 'c; stream >] ->
		Buffer.add_char buffer c;
		lex_text_literal buffer stream
		
and lex_line_comment = parser
  | [< ' ('\n'); stream=lex >] -> stream
  | [< '_; e=lex_line_comment >] -> e
  | [< >] -> [< >]
  
  
and lex_colon = parser
	| [< ' ('='); stream=lex >] -> [< 'Token.Assignment; stream >]
