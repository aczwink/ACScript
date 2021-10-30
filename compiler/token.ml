type token =
	| Assignment
	| Let
	| Map
	| Symbol of char
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
