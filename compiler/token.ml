type token =
	| Assignment
	| Let
	| Symbol of char
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
