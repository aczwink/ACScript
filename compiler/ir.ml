type constant =
	ConstantFloat of float



type instruction =
	| CallExternalInstruction of string * instruction
	| LoadConstantInstruction of constant



let constant_to_string c = 
	match c with
	| ConstantFloat f -> string_of_float f


module InstructionMap = Map.Make(struct type t = instruction let compare = compare end)
let rec number_instruction instr num_map =
	match instr with
	| CallExternalInstruction (_, arg) -> number_instructions arg num_map
	| _ -> num_map;
and number_instructions instr num_map =
	let t = number_instruction instr num_map in
	InstructionMap.add instr (InstructionMap.cardinal t) t
	
let rec instruction_to_list instr = 
	match instr with
	| CallExternalInstruction (_, arg) -> instruction_to_list arg @ [instr]
	| _ -> [instr]
	
let instructions_to_string instrs num_map =
	let instrResult i =
		"$t" ^ string_of_int (InstructionMap.find i num_map)
	in
	let i2p i =
		match i with
		| CallExternalInstruction (name, arg) -> ("callext", [name; instrResult arg])
		| LoadConstantInstruction c -> ("ldc", [constant_to_string c])
	in
	let i2s i =
		let (mnemonic, args) = i2p i in
		 (instrResult i) ^ " <- " ^ mnemonic ^ " " ^ String.concat ", " args
	in
	String.concat "\n" (List.map i2s instrs)
	
let instruction_to_string instr = 
	let num_map = number_instructions instr InstructionMap.empty in
	let instrs = instruction_to_list instr in
	instructions_to_string instrs num_map
