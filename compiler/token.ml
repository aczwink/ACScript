type keyword =
	| Extern
	| Let
	| Import
	| Type

type token =
	| Assignment
	| Keyword of keyword
	| Map
	| Symbol of char
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	
let kw_to_string kw =
	match kw with
	| Extern -> "extern"
	| Let -> "let"
	| Import -> "import"
	| Type -> "type"
;;
	
let to_string token =
	match token with
	| Assignment -> ":="
	| Keyword kw -> kw_to_string kw
	| Map -> "->"
	| Symbol s -> String.make 1 s
	| Identifier id -> id
	| NaturalLiteral lit -> lit
	| StringLiteral lit -> lit
;;
