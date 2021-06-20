let opcode_callext = 1;;
let opcode_ldc = 3;;
let opcode_ntp = 5;;
let opcode_ret = 9;;

module ConstantMap = Map.Make(struct type t = Ir.constant let compare = compare end)
module ImportMap = Map.Make(String)

let encode_code instrs constants imports =
	let rec index_constants constants index =
		match constants with
		| [] -> ConstantMap.empty
		| constant::rest -> let res = (index_constants rest (index+1)) in ConstantMap.add constant index res
	in
	let rec index_imports imports index =
		match imports with
		| [] -> ImportMap.empty
		| import::rest -> let res = (index_imports rest (index+1)) in ImportMap.add import index res
	in
	let buffer = (Buffer.create 0) in
	let constantMap = index_constants constants 0 in
	let importMap = index_imports imports 0 in
	
	let encode_instr instr = 
		match instr with
		| Ir.CallExternalInstruction (name, _) -> Buffer.add_uint8 buffer opcode_callext; Buffer.add_uint16_be buffer (ImportMap.find name importMap)
		| Ir.LoadConstantInstruction constant -> Buffer.add_uint8 buffer opcode_ldc; Buffer.add_uint16_be buffer (ConstantMap.find constant constantMap)
		| Ir.NewTupleInstruction args -> Buffer.add_uint8 buffer opcode_ntp; Buffer.add_uint16_be buffer (ConstantMap.find (ConstantNatural(string_of_int (List.length args))) constantMap)
	in
	let rec encode_instrs instrs =
		match instrs with
		| [] -> ()
		| instr::rest -> (encode_instr instr); encode_instrs rest
	in
	encode_instrs instrs;
	Buffer.add_uint8 buffer opcode_ret; (* TODO: emergency bail out *)
	buffer
;;




let collect_constants instrs =
	let rec collect_instrs instrs =
		match instrs with
		| [] -> []
		| Ir.LoadConstantInstruction (constant)::rest -> constant :: (collect_instrs rest)
		| _::rest -> (collect_instrs rest)
	in
	collect_instrs instrs
;;

let encode_constants constants =
	let constant_to_string constant = 
		match constant with
		| Ir.ConstantNatural value -> value
		| Ir.ConstantString value -> value
	in
	let get_constant_type constant =
		match constant with
		| Ir.ConstantNatural _ -> 'n'
		| Ir.ConstantString _ -> 's'
	in
	let rec add_constants_to_buffer buffer constants = 
		match constants with
		| [] -> ()
		| constant::rest ->
			Buffer.add_char buffer (get_constant_type constant);
			Buffer.add_string buffer (constant_to_string constant);
			Buffer.add_uint8 buffer 0;
			add_constants_to_buffer buffer rest
	in
	let buffer = (Buffer.create 0) in
	Buffer.add_uint16_be buffer (List.length constants);
	add_constants_to_buffer buffer constants;
	buffer
;;






let collect_imports instrs =
	let rec collect_instrs instrs =
		match instrs with
		| [] -> []
		| Ir.CallExternalInstruction (name, _) :: rest -> name::(collect_instrs rest)
		| _::rest -> collect_instrs rest;
	in
	collect_instrs instrs
;;

let encode_imports imports =
	let rec add_imports_to_buffer buffer imports = 
		match imports with
		| [] -> ()
		| name::rest ->
			Buffer.add_string buffer name;
			Buffer.add_uint8 buffer 0;
			add_imports_to_buffer buffer rest
	in
	let buffer = (Buffer.create 0) in
	Buffer.add_uint16_be buffer (List.length imports);
	add_imports_to_buffer buffer imports;
	buffer
;;
	
	









	
let write_chunk oc id dataBuffer = 
	output_string oc id;
	output_binary_int oc (Buffer.length dataBuffer);
	Buffer.output_buffer oc dataBuffer
;;

let dump_module _module inputFilePath = 
	let constants = collect_constants _module in
	let imports = collect_imports _module in
	let oc = open_out (inputFilePath ^ "b") in
		(* header *)
		write_chunk oc "ACSB" (Buffer.create 0);
		write_chunk oc "impt" (encode_imports imports);
		write_chunk oc "data" (encode_constants constants);
		write_chunk oc "code" (encode_code _module constants imports);
		(* rest is code *)
		close_out oc
