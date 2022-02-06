module StringMap = Map.Make(String);;

type expression =
	| Self
	| Identifier of string
	| NaturalLiteral of string
	| StringLiteral of string
	| UnsignedLiteral of string
	| External of string
	| Import of string
	| Call of expression * expression
	| Function of string * function_rule list
	| Object of object_entry list
	| Select of expression * string
	| Tuple of expression list
	
and function_rule = {
	pattern: expression;
	condition: expression option;
	body: expression;
}
	
and object_entry = {
	name: string;
	expr: expression
}
;;

type statement = 
	| ExpressionStatement of expression
	| LetBindingStatement of string * Type_system.typedefinition * expression
	| TypeDefStatement of string * Type_system.typedefinition
	| UseStatement of string
	
type export = {
	value: expression;
	typedef: Type_system.typedefinition;
}

type program_module = {
	moduleName: string;
	statements: statement list;
	exports: export StringMap.t;
}
