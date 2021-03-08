let rec lex = parser
	(* number *)
	| [< ' ('0' .. '9' as c); stream >] ->
		let buffer = Buffer.create 1 in
		Buffer.add_char buffer c;
		lex_number buffer stream
      
	(* identifier *)
	| [< ' ('A' .. 'Z' | 'a' .. 'z' as c); stream >] ->
		let buffer = Buffer.create 1 in
		Buffer.add_char buffer c;
		lex_ident buffer stream
		
	(* symbols *)		
	| [< 'c; stream >] ->
      [< 'Token.Symbol c; lex stream >]
		
	(* end of stream *)
	| [< >] -> [< >]
	
and lex_number buffer = parser
  | [< ' ('0' .. '9' | '.' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_number buffer stream
  | [< stream=lex >] ->
      [< 'Token.Number (float_of_string (Buffer.contents buffer)); stream >]
	
and lex_ident buffer = parser
	| [< ' ('A' .. 'Z' | 'a' .. 'z' | '0' .. '9' as c); stream >] ->
		Buffer.add_char buffer c;
		lex_ident buffer stream
	| [< stream=lex >] ->
		let ident = Buffer.contents buffer in
		[< 'Token.Identifier ident; stream >]
