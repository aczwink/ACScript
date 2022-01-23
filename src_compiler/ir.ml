type constant =
	| ConstantNatural of string
	| ConstantString of string



type instruction =
	| BranchInstruction of string * string * string
	| CallInstruction of string * string
	| LoadConstantInstruction of constant
	| LoadExternalInstruction of string
	| NewDictionaryInstruction
	| NewTupleInstruction of int
	| ReturnInstruction of string
	| SelectInstruction of string * string
	| SetInstruction of string * string * string
	
	
type full_instruction = {
	instruction: instruction;
	returnName: string;
}

type block = {
	symbolName: string;
	instructions: full_instruction list;
}

type program_function = {
	symbolName: string;
	blocks: block list;
}





let constant_to_string c = 
	match c with
	| ConstantNatural n -> n
	| ConstantString x -> "\"" ^ x ^ "\""

let instruction_to_mnemonic i=
	match i with
	| BranchInstruction _ -> "br"
	| CallInstruction _ -> "call"
	| LoadConstantInstruction _ -> "ldc"
	| LoadExternalInstruction _ -> "lde"
	| NewDictionaryInstruction -> "ndct"
	| NewTupleInstruction _ -> "ntp"
	| ReturnInstruction _ -> "ret"
	| SelectInstruction _ -> "slct"
	| SetInstruction _ -> "set"
;;

let full_instruction_to_string i =
	let extract_args_as_strings i = 
		match i with
		| BranchInstruction (condSym, thenBlockSym, elseBlockSym) -> [condSym; thenBlockSym; elseBlockSym]
		| CallInstruction (func, arg) -> func::[arg]
		| LoadConstantInstruction constant -> [constant_to_string constant]
		| LoadExternalInstruction retVal -> [retVal]
		| NewDictionaryInstruction -> []
		| NewTupleInstruction nEntries -> [string_of_int nEntries]
		| ReturnInstruction retVal -> [retVal]
		| SelectInstruction (expr, member) -> expr::[member]
		| SetInstruction (dictName, member, value) -> dictName::member::[value]
	in
	let return_assignment i =
		let empty = "\t  " in
		match i.instruction with
		| BranchInstruction _ -> empty
		| ReturnInstruction _ -> empty
		| SetInstruction _ -> empty
		| _ -> "\t" ^ i.returnName ^ " <-- "
	in
	(return_assignment i) ^ (instruction_to_mnemonic i.instruction) ^ " " ^ (String.concat ", " (extract_args_as_strings i.instruction))
;;

let block_to_string (block: block) =
	"  " ^ block.symbolName ^ ":\n" ^ String.concat "\n" (List.map (full_instruction_to_string) block.instructions)
;;

let func_to_string func =
	func.symbolName ^ ":\n" ^ String.concat "\n" (List.map (block_to_string) func.blocks) ^ "\n"
;;

let to_string funcs =
	String.concat "\n" (List.map (func_to_string) funcs)
;;
