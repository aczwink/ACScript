{
	open Lexing
	open Parser
	
	exception SyntaxError of string
	
	let next_line lexbuf =
    let pos = lexbuf.lex_curr_p in
    lexbuf.lex_curr_p <-
      { pos with pos_bol = lexbuf.lex_curr_pos;
                pos_lnum = pos.pos_lnum + 1
      }
}

let digit = ['0'-'9']
let alpha = ['a'-'z' 'A'-'Z']
let operator = ['*' '+' '-' '<' '=' '_']
let id = (alpha|operator) (alpha|digit|operator)*

let whitespace = [' ' '\t']+
let newline = '\r' | '\n' | "\r\n"

rule read_token = 
	parse
	| ":" 			{ SYMBOL_COLON }
	| "," 			{ SYMBOL_COMMA }
	| "{" 			{ SYMBOL_LEFT_BRACE }
	| "}" 			{ SYMBOL_RIGHT_BRACE }
	| "(" 			{ SYMBOL_LEFT_PARENTHESIS }
	| ")" 			{ SYMBOL_RIGHT_PARENTHESIS }
	| "|" 			{ SYMBOL_PIPE }
	| ";" 			{ SYMBOL_SEMICOLON }
	| ":=" 			{ SYMBOL_ASSIGNMENT }
	| "->" 			{ SYMBOL_MAP }
	| "extern"		{ KEYWORD_EXTERN }
	| "let"			{ KEYWORD_LET }
	| "type"		{ KEYWORD_TYPE }
	| "use"			{ KEYWORD_USE }
	| whitespace 	{ read_token lexbuf }
	| "//" 			{ read_single_line_comment lexbuf }
	| digit+"u"		{ UNSIGNED_LITERAL (Lexing.lexeme lexbuf) }
	| digit+		{ NATURAL_LITERAL (Lexing.lexeme lexbuf) }
	| id 			{ IDENTIFIER (Lexing.lexeme lexbuf) }
	| '"' 			{ read_string (Buffer.create 17) lexbuf }
	| newline 		{ next_line lexbuf; read_token lexbuf }
	| eof 			{ END_OF_STREAM }
	| _ 			{raise (SyntaxError ("Illegal character: '" ^ Lexing.lexeme lexbuf ^ "'")) }
	
	
and read_single_line_comment = parse
	| newline { next_line lexbuf; read_token lexbuf } 
	| eof { END_OF_STREAM }
	| _ { read_single_line_comment lexbuf } 
	
and read_string buf = parse
  | '"'       { STRING_LITERAL (Buffer.contents buf) }
  | [^ '"' '\\']+
    { Buffer.add_string buf (Lexing.lexeme lexbuf);
      read_string buf lexbuf
    }
