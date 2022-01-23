module StringSet = Set.Make(String);;


class graph =
	object(this)
	
	val dependencies: (string, StringSet.t) Hashtbl.t = Hashtbl.create 10
	
	method add (instr: Ir.full_instruction) =
		this#ensure_dict_entry_exists instr.returnName;
		match instr.instruction with
		| Ir.BranchInstruction (condSym, thenBlockSym, elseBlockSym) -> this#add_dependencies instr.returnName [condSym; thenBlockSym; elseBlockSym]
		| Ir.CallInstruction (func, arg) ->
			this#add_dependencies instr.returnName (func::[arg])
		| Ir.LoadConstantInstruction _ -> ()
		| Ir.LoadExternalInstruction externalSym -> this#add_dependency instr.returnName externalSym
		| Ir.NewDictionaryInstruction -> ()
		| Ir.NewTupleInstruction _ -> ()
		| Ir.ReturnInstruction retSym -> this#add_dependency instr.returnName retSym
		| Ir.SelectInstruction (dictSym, keySym) -> this#add_dependencies instr.returnName (dictSym::[keySym])
		| Ir.SetInstruction (dictSym, keySym, valSym) -> this#add_dependencies instr.returnName (dictSym::keySym::[valSym])
		
	method counts =
		let res = Hashtbl.create 10 in
		Hashtbl.iter (fun k v -> Hashtbl.add res k (StringSet.cardinal v)) dependencies;
		res
		
	(* Private methods *)
	method private add_dependencies (from: string) (_to: string list) =
		List.iter (this#add_dependency from) _to
	
	method private add_dependency (from: string) (_to: string) =
		match Hashtbl.find_opt dependencies _to with
		| Some x -> 
			Hashtbl.replace dependencies _to (StringSet.add from x)
		| None ->
			Hashtbl.add dependencies _to (StringSet.add from StringSet.empty)
			
	method private ensure_dict_entry_exists (_to: string) =
		match Hashtbl.find_opt dependencies _to with
		| Some _ -> ()
		| None -> Hashtbl.add dependencies _to StringSet.empty
end;;

let build funcs =
	let g = new graph in
	let process_block block =
		List.iter (g#add) block.Ir.instructions
	in
	List.iter (fun func -> List.iter (process_block) func.Ir.blocks) funcs;
	g
