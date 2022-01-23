let opcode_call = 0;;
let opcode_jmpf = 1;;
let opcode_ldc = 2;;
let opcode_lde = 3;;
let opcode_ndct = 4;;
let opcode_ntp = 5;;
let opcode_pop = 6;;
let opcode_popa = 7;;
let opcode_push = 8;;
let opcode_push_uint16 = 9;;
let opcode_ret = 10;;
let opcode_slct = 11;;
let opcode_set = 12;;

module ConstantMap = Map.Make(struct type t = Ir.constant let compare = compare end)
module IntSet = Set.Make(Int);;
module StringMap = Map.Make(String);;



let dump_ir moduleName instrs =
	let outputFilePath = moduleName ^ ".ir.txt" in
	let oc = open_out (outputFilePath) in
		output_string oc (Ir.to_string instrs);
		close_out oc
;;

let dump_bundle moduleName tail bundle symbolTable typeSystem =
	let outputFilePath = moduleName ^ "." ^ tail ^ ".txt" in
	let oc = open_out (outputFilePath) in
		output_string oc (Semantic_ast_print.to_string bundle symbolTable typeSystem);
		close_out oc
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


type acsb_instruction = {
	opcode: int;
	args: int list;
};;

type acsb_program = {
	constants: Ir.constant list;
	instructions: acsb_instruction list;
	funcOffsets: int list;
};;

let get_byte_size instr = 1 + (List.length instr.args)*2;;

let compute_offsets instrs =
	let with_offset (currentOffset, mapped) instr =
		( currentOffset + get_byte_size instr, (currentOffset, instr)::mapped)
	in
	let (_, mapped) = List.fold_left (with_offset) (0, []) instrs in
	List.rev mapped
;;

class program_assembler funcs =
	object(this)
	
	val mutable valueStack = Stack.create ()
	val instrs = Queue.create ()
	val instructionDependencies = (Ir_dependency_graph.build funcs)#counts
	val mutable currentProgramOffset = 0
	val mutable funcOffsets = []
	val mutable blockOffsets = StringMap.empty
	val mutable constantMap = ConstantMap.empty
	val mutable constants = []
	val blockOffsetsToInsertLater = Hashtbl.create 10
	
	method add_opcode opcode dependentSymbols resultSym =
		this#add_opcode_with_args opcode [] dependentSymbols resultSym
		
	method add_opcode_with_args opcode args dependentSymbols resultSym =
		List.iter (this#push_symbol) dependentSymbols;
		
		let instr = { opcode = opcode; args = args } in
		this#insert_instruction instr;
		
		List.iter (fun _ -> this#reduce_one) dependentSymbols;
		if resultSym = "" then () else Stack.push resultSym valueStack;
		if resultSym = "" then this#insert_pop_if_possible else this#insert_popa_if_possible;
		
	method assemble_func func =
		this#begin_func;
		this#assemble_block func (List.hd func.Ir.blocks)
		
	method get_program =
		let map_instr (_, instr) =
			if instr.opcode == opcode_push_uint16
			then { opcode = opcode_push_uint16; args = [this#find_offset_to_block instr]; }
			else instr
		in
		let instrs = List.rev (Queue.fold (fun accu instr -> instr::accu) [] instrs) in
		let instrs_with_offsets = compute_offsets instrs in
		let mapped_instrs = List.map (map_instr) instrs_with_offsets in
		{ constants = this#order_constants; instructions = mapped_instrs; funcOffsets = funcOffsets }
		
	method map_constant_to_index constant =
		match ConstantMap.find_opt constant constantMap with
		| Some idx -> idx
		| None ->
			let result = ConstantMap.cardinal constantMap in
			constantMap <- ConstantMap.add constant result constantMap;
			result
		
	(* Private methods *)
	method private assemble_block func (block: Ir.block) =
		blockOffsets <- StringMap.add block.symbolName currentProgramOffset blockOffsets;
		List.iter (this#assemble_instr func) block.instructions
		
	method private assemble_instr func (instr: Ir.full_instruction) = 
		match instr.instruction with
		| Ir.BranchInstruction (condSym, thenBlockSym, elseBlockSym) ->
			this#add_opcode opcode_jmpf [condSym; elseBlockSym] "";
			let currentStack = Stack.copy valueStack in
			this#assemble_block func (this#find_block func thenBlockSym);
			valueStack <- currentStack;
			this#assemble_block func (this#find_block func elseBlockSym)
		| Ir.CallInstruction (funcSym, argSym) -> this#add_opcode opcode_call (funcSym::[argSym]) instr.returnName
		| Ir.LoadConstantInstruction constant -> this#add_opcode_with_args opcode_ldc [this#map_constant_to_index constant] [] instr.returnName
		| Ir.LoadExternalInstruction externalSym -> this#add_opcode opcode_lde [externalSym] instr.returnName
		| Ir.NewDictionaryInstruction -> this#add_opcode opcode_ndct [] instr.returnName
		| Ir.NewTupleInstruction nEntries -> this#add_opcode_with_args opcode_ntp [nEntries] [] instr.returnName
		| Ir.ReturnInstruction retSym ->
			this#push_symbol retSym;
			this#clean_stack_for_return;
			let instr = { opcode = opcode_ret; args = [] } in
			this#insert_instruction instr
		| Ir.SelectInstruction (dictSym, keySym) -> this#add_opcode opcode_slct (dictSym::[keySym]) instr.returnName
		| Ir.SetInstruction (dictSym, keySym, valSym) ->
			this#add_opcode opcode_set (dictSym::keySym::[valSym]) ""
	
	method private begin_func =
		Stack.clear valueStack;
		Stack.push "$p" valueStack;
		funcOffsets <- funcOffsets @ [currentProgramOffset]
		
	method private clean_stack_for_return =
		if Stack.length valueStack > 1
		then this#clean_stack_for_return_iteration
		else ()
		
	method private clean_stack_for_return_iteration =
		this#insert_popa;
		this#clean_stack_for_return
		
	method private find_block func blockName =
		List.find (fun (b: Ir.block) -> b.symbolName = blockName) func.Ir.blocks
		
	method private find_offset_to_block instr =
		let blockOffsetToInsertLater = (List.hd instr.args) in
		let blockSymbol = Hashtbl.find blockOffsetsToInsertLater blockOffsetToInsertLater in
		let blockOffset = StringMap.find blockSymbol blockOffsets in
		blockOffset
	
	method private find_sym_on_stack sym =
		if (String.starts_with ~prefix:"$b" sym) then this#push_offset_to_block sym
		else if (String.starts_with ~prefix:"$f" sym) then this#push_function_symbol sym
		else this#find_sym_on_stack_iteration sym (Stack.copy valueStack) 0
		
	method private find_sym_on_stack_iteration sym stack offset =
		if (Stack.pop stack) = sym then
			offset
		else
			this#find_sym_on_stack_iteration sym stack (offset + 1)
			
	method private insert_instruction instr =
		Queue.add (instr) instrs;
		currentProgramOffset <- (currentProgramOffset + (get_byte_size instr))
	
			
	method private insert_pop =
		this#insert_instruction({ opcode = opcode_pop; args = [] });
		this#reduce_one;
		this#insert_pop_if_possible
		
	method private insert_popa =
		this#insert_instruction({ opcode = opcode_popa; args = [] });
		let v = Stack.pop valueStack in
		this#reduce_one;
		Stack.push v valueStack;
		this#insert_popa_if_possible
		
	method private insert_pop_if_possible =
		match Stack.top_opt valueStack with
		| None -> ()
		| Some top -> if (Hashtbl.find instructionDependencies top) <= 0 then
			this#insert_pop
		else
			()
			
	method private insert_popa_if_possible =
		let stackCopy = Stack.copy valueStack in
		let _ = Stack.pop stackCopy in
		match Stack.top_opt stackCopy with
		| None -> ()
		| Some top -> if (Hashtbl.find instructionDependencies top) <= 0 then
			this#insert_popa
		else
			()
			
	method private insert_push sym offset =
		this#insert_instruction({ opcode = opcode_push; args = [offset] });
		Stack.push sym valueStack
		
	method private order_constants =
		let consts = ConstantMap.fold (fun k v accu -> (k, v)::accu) constantMap [] in
		let ordered = List.sort (fun (_, v1) (_, v2) -> v1 - v2) consts in
		List.map (fun (k, _) -> k) ordered
		
	method private push_function_symbol sym =
		let numStr = String.sub sym 2 ((String.length sym)-2) in
		let num = int_of_string numStr in
		let offset = List.nth funcOffsets num in
		
		let constant = Ir.ConstantNatural(string_of_int offset) in
		this#insert_instruction({ opcode = opcode_ldc; args = [this#map_constant_to_index constant] });
		Stack.push sym valueStack;
		0
		
	method private push_offset_to_block sym =
		let offset = (Int.min_int + (Hashtbl.length blockOffsetsToInsertLater)) in
		Hashtbl.add blockOffsetsToInsertLater offset sym;
		this#insert_instruction({ opcode = opcode_push_uint16; args = [ offset ] });
		Stack.push sym valueStack;
		0
			
	method private push_symbol sym =
		let offset = this#find_sym_on_stack sym in
		let newRefCount = (Hashtbl.find instructionDependencies sym) - 1 in
		Hashtbl.replace instructionDependencies sym newRefCount;
		if ((newRefCount > 0) || (offset > 0)) then
			this#insert_push sym offset
		else
			()
			
	method private reduce_one =
		let _ = Stack.pop valueStack in
		()
end;;

let assemble_program funcs =
	let asm = new program_assembler funcs in	
	List.iter (asm#assemble_func) funcs;
	asm#get_program
;;

class program_printer funcOffsets =
	object(this)
	
	val mutable currentOffset = 0
	
	method print_instruction instr =
		let offset = if IntSet.mem currentOffset funcOffsets then "\n" ^ (string_of_int currentOffset) ^ ":\n" else "" in
		currentOffset <- currentOffset + (get_byte_size instr);
		offset ^ "\t" ^ (this#opc2mnemonic instr.opcode) ^ " \t" ^ String.concat ", " (List.map (string_of_int) instr.args)
		
	method private opc2mnemonic opc =
		if opc == opcode_call then "CALL"
		else if opc == opcode_jmpf then "JMPF"
		else if opc == opcode_ldc then "LDC"
		else if opc == opcode_lde then "LDE"
		else if opc == opcode_ndct then "NDCT"
		else if opc == opcode_ntp then "NTP"
		else if opc == opcode_pop then "POP"
		else if opc == opcode_popa then "POPA"
		else if opc == opcode_push then "PUSH"
		else if opc == opcode_push_uint16 then "PU16"
		else if opc == opcode_ret then "RET"
		else if opc == opcode_slct then "SLCT"
		else if opc == opcode_set then "SET"
		else raise (Stream.Error ("not implemented: " ^ (string_of_int opc)))
end;;

let to_string prog =	
	let constsPart = String.concat "\n" (List.mapi (fun i c -> (string_of_int i) ^ ": " ^ (Ir.constant_to_string c)) prog.constants) in
	let funcOffsets = List.fold_left (fun set elem -> IntSet.add elem set) IntSet.empty prog.funcOffsets in
	let printer = new program_printer funcOffsets in
	let codePart = String.concat "\n" (List.map (printer#print_instruction) prog.instructions) in
	
	"constants:\n" ^ constsPart ^ "\n\ncode:\n" ^ codePart
;;


let dump_acsb_asm moduleName prog =
	let outputFilePath = moduleName ^ ".acsb_asm.txt" in
	let oc = open_out (outputFilePath) in
		output_string oc (to_string prog);
		close_out oc
;;


let encode_code prog entryPoint =	
	let buffer = (Buffer.create 0) in
	
	let encode_instr i =
		Buffer.add_uint8 buffer i.opcode;
		List.iter (fun arg -> Buffer.add_uint16_be buffer arg) i.args
	in
	
	Buffer.add_uint16_be buffer entryPoint;	
	List.iter (encode_instr) prog;
	buffer
;;


let write_chunk oc id dataBuffer = 
	output_string oc id;
	output_binary_int oc (Buffer.length dataBuffer);
	Buffer.output_buffer oc dataBuffer
;;

let dump_acsb moduleName funcs =
	let prog = assemble_program funcs in
	dump_acsb_asm moduleName prog;
	let oc = open_out (moduleName ^ ".acsb") in
		(* header *)
		write_chunk oc "ACSB" (Buffer.create 0);
		write_chunk oc "data" (encode_constants prog.constants);
		write_chunk oc "code" (encode_code prog.instructions (List.hd (List.rev prog.funcOffsets)));
		close_out oc
;;

let apply_optimizations bundle =
	let optimizations = [
		Optimization_dead_code_removal.remove_unused_code;
		Optimization_symbol_inlining.inline_symbols_that_are_only_used_once;
		Optimization_literal_inlining.inline_objects_that_are_used_once
	] in
	List.fold_left (fun _mod optimizer -> optimizer _mod) bundle optimizations
;;


let compile mainModuleName modulesCollection =
	let bundle = Bundle.bundle modulesCollection in
	dump_bundle mainModuleName "bundle" bundle modulesCollection#get_symbol_table modulesCollection#get_type_system;
	let optBundle = apply_optimizations bundle in
	dump_bundle mainModuleName "bundle.optimized" optBundle modulesCollection#get_symbol_table modulesCollection#get_type_system;
	let funcs = Ir_translate.translate optBundle in
		dump_ir mainModuleName funcs;
		dump_acsb mainModuleName funcs
;;
