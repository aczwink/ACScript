type constant =
	| ConstantNatural of string
	| ConstantString of string



type instruction =
	| CallExternalInstruction of string * instruction
	| LoadConstantInstruction of constant
	| NewTupleInstruction of instruction list



let constant_to_string c = 
	match c with
	| ConstantNatural n -> n
	| ConstantString x -> "\"" ^ x ^ "\""
	
	
type flatInstr = { name: string; instr: instruction; args: string list }
	
let instructions_to_string flatInstructions =
	(*
	let instrResult i =
		"$t" ^ string_of_int (InstructionMap.find i num_map)
	in
	let i2p i =
		match i with
		| CallExternalInstruction (name, arg) -> ("callext", [name; instrResult arg])
		| LoadConstantInstruction c -> ("ldc", [constant_to_string c])
		| NewTupleInstruction args -> ("ntp", List.map instrResult args)
	in
	*)
	let i2m i=
		match i with
		| CallExternalInstruction _ -> "callext"
		| LoadConstantInstruction _ -> "ldc"
		| NewTupleInstruction _ -> "ntp"
	in
	let i2s i =
		let mnemonic = i2m i.instr in
		i.name ^ " <- " ^ mnemonic ^ " " ^ String.concat ", " i.args
	in
	String.concat "\n" (List.map i2s flatInstructions)


let flatten_instructions instrs =
	let gentemp tempCounter = "$t" ^ (string_of_int tempCounter)
	in
	let rec flatten_instr instr tempCounter =
		match instr with
		(*| 
			let (argName = last argInstrs in
			in argInstrs @ [(gentemp newCounter, [argName], newCounter+1)]*)
		| CallExternalInstruction (externalName, arg) ->
			let (prev, argInstr, newCounter) = flatten_instr arg tempCounter in
			(prev @ [argInstr], { name = gentemp newCounter; instr = instr; args = externalName::[argInstr.name]}, newCounter + 1)
		| LoadConstantInstruction constant -> ([], { name = gentemp tempCounter; instr = instr; args = [ constant_to_string constant ]}, tempCounter+1)
		| NewTupleInstruction args ->
			let (argInstrs, newCounter) = flatten args tempCounter in
			(argInstrs, { name = gentemp newCounter; instr = instr; args = List.map (fun argInstr -> argInstr.name) argInstrs}, newCounter+1)
	and flatten instrs tempCounter = 
		match instrs with
		| [] -> ([], tempCounter)
		| instr::rest ->
			let (prev, flattened, newCounter) = flatten_instr instr tempCounter
			in
			let (restInstrs, newestCounter) = flatten rest newCounter
			in
			(prev @ flattened::restInstrs, newestCounter)
	in
	let (result, _) = flatten instrs 0
	in
	result
	
	
let instructions_to_string instrs = 
	let flatInstrs = flatten_instructions instrs in
	instructions_to_string flatInstrs
