type token =
	| Assignment
	| Let
	| Map
	| Type
	| Symbol of char
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	
let to_string token =
	match token with
	| Assignment -> ":="
	| Let -> "let"
	| Map -> "->"
	| Type -> "type"
	| Symbol s -> String.make 1 s
	| Identifier id -> id
	| NaturalLiteral lit -> lit
	| StringLiteral lit -> lit
;;
