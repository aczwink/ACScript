module StringMap = Map.Make(String);;

class block_builder (symbolName: string) =
	object(this)
	
	val instrs: Ir.full_instruction Queue.t = Queue.create ()
	
	method add_instruction fi =
		Queue.add fi instrs
		
	method build =
		let block: Ir.block = { symbolName = symbolName; instructions = this#get_instructions } in
		block
		
	method get_instructions =
		List.rev (Queue.fold (fun accu instr -> instr::accu) [] instrs)
		
	method get_symbol_name =
		symbolName
end;;

class function_builder (symbolName: string) =
	object(this)
	
	val mutable blocks = []
	
	method active_block =
		List.hd blocks
	
	method add_block symbolName =
		let newBlock = new block_builder symbolName in
		blocks <- newBlock::blocks;
		newBlock
	
	method add_instruction fi =
		this#active_block#add_instruction fi
		
	method get_blocks =
		List.rev (List.map (fun bb -> bb#build) blocks)
		
	method get_symbol_name =
		symbolName
end;;

class program_builder =
	object(this)
	
	val mutable funcCounter = 0	
	val funcStack = Stack.create ()
	val mutable funcs = []
	val mutable blockCounter = 0
	val mutable instrCounter = 0
	
	method active_func =
		Stack.top funcStack
	
	method add_block =
		let symbolName = "$b" ^ (string_of_int blockCounter) in
		blockCounter <- blockCounter + 1;
		this#active_func#add_block symbolName
	
	method add_func =
		let fb = new function_builder ("$f" ^ (string_of_int (funcCounter))) in
		funcCounter <- funcCounter + 1;
		Stack.push fb funcStack;
		let _ = this#add_block in
		()
	
	method add_instruction (instr: Ir.instruction) =
		let name = this#next_instruction_name in
		let fi: Ir.full_instruction = { instruction = instr; returnName = name } in
		this#active_func#add_instruction fi;
		name
		
	method add_instruction_without_result (instr: Ir.instruction) =
		let _ = this#add_instruction instr in
		()
		
	method finish_func =
		let f = Stack.pop funcStack in
		let pf: Ir.program_function = { symbolName = f#get_symbol_name; blocks = f#get_blocks } in
		funcs <- funcs @ [pf];
		pf.symbolName
		
	method get_funcs =
		funcs
		
	method next_instruction_name = 
		let name = "$t" ^ (string_of_int instrCounter) in
		instrCounter <- instrCounter +1;
		name
end;;


let translate _module =
	let builder = new program_builder in
	let namedValues: (string, string) Hashtbl.t = Hashtbl.create 10
	in
	
	let load_nat x = builder#add_instruction (Ir.LoadConstantInstruction(Ir.ConstantNatural(x))) in
	let load_string x = builder#add_instruction (Ir.LoadConstantInstruction(Ir.ConstantString(x))) in
	let load_uint x = builder#add_instruction (Ir.LoadConstantInstruction(Ir.ConstantUnsigned(x))) in
	let build_compare sym1 sym2 =
		let paramSym = builder#add_instruction(Ir.NewTupleInstruction(2)) in
		builder#add_instruction_without_result(Ir.SetInstruction(paramSym, load_nat "0", sym1));
		builder#add_instruction_without_result(Ir.SetInstruction(paramSym, load_nat "1", sym2));
		let eqSym = builder#add_instruction(Ir.SelectInstruction(sym1, load_string "=")) in
		builder#add_instruction(Ir.CallInstruction(eqSym, paramSym))
	in
	
	let translate_id_pattern id parameterSymbol =
		match Hashtbl.find_opt namedValues id with
		| None -> Hashtbl.add namedValues id parameterSymbol; None
		| Some sym ->
			let resSym = build_compare parameterSymbol sym in
			Some ( resSym )
	in
	
	let rec translate_pattern pattern parameterSymbol =
		match pattern with
		| Semantic_ast.Identifier id -> translate_id_pattern id parameterSymbol
		| Semantic_ast.NaturalLiteral x ->
			let argSym = load_nat x in
			let resSym = build_compare parameterSymbol argSym in
			Some ( resSym )
		| Semantic_ast.Tuple entries ->
			let map_entry (idx: int) entry =
				let idxNat = string_of_int idx in
				let paramSym = builder#add_instruction(Ir.SelectInstruction(parameterSymbol, load_nat idxNat)) in
				translate_pattern entry paramSym
			in
			let and_results acc sym =
				let paramSym = builder#add_instruction(Ir.NewTupleInstruction(2)) in
				builder#add_instruction_without_result(Ir.SetInstruction(paramSym, load_nat "0", acc));
				builder#add_instruction_without_result(Ir.SetInstruction(paramSym, load_nat "1", sym));
				let andSym = builder#add_instruction(Ir.SelectInstruction(parameterSymbol, load_string "and")) in
				let resSym = builder#add_instruction(Ir.CallInstruction(andSym, paramSym)) in
				resSym
			in
			let mapped = List.mapi (map_entry) entries in
			let l = List.filter_map (fun x -> x) mapped in
			if List.length l = 0 then
				None
			else
				Some (List.fold_left (and_results) (List.hd l) (List.tl l))
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.expr_to_string pattern))
	in

	let rec translate_expr expr =
		match expr with
		| Semantic_ast.Self -> builder#active_func#get_symbol_name
		| Semantic_ast.Identifier id -> Hashtbl.find namedValues id
		| Semantic_ast.NaturalLiteral x -> load_nat x
		| Semantic_ast.StringLiteral x -> load_string x
		| Semantic_ast.UnsignedLiteral x -> load_uint x
		| Semantic_ast.External externalName -> builder#add_instruction (Ir.LoadExternalInstruction(load_string externalName))
		| Semantic_ast.Import _ -> raise (Stream.Error ("Import is not implemented"))
		| Semantic_ast.Call (func, arg) ->
			builder#add_instruction(Ir.CallInstruction(translate_expr func, translate_expr arg))
		| Semantic_ast.Function (_, rules) ->
			builder#add_func;
			List.iter (translate_rule) rules;
			builder#finish_func
		| Semantic_ast.Dictionary entries -> 
			let map_entry dictName entry =
				let _ = builder#add_instruction(Ir.SetInstruction(dictName, load_string entry.Semantic_ast.name, translate_expr entry.expr)) in
				()
			in		
			let res = builder#add_instruction(Ir.NewDictionaryInstruction) in
			List.iter (map_entry res) entries;
			res
		| Semantic_ast.Select (expr, member) ->
			let memRes = load_string member in
			builder#add_instruction(Ir.SelectInstruction(translate_expr expr, memRes))
		| Semantic_ast.Tuple exprs ->
			let tupleName = builder#add_instruction(Ir.NewTupleInstruction(List.length exprs)) in
			let set_entry index expr = 
				let k = load_nat (string_of_int index) in
				builder#add_instruction_without_result(Ir.SetInstruction(tupleName, k, translate_expr expr))
			in
			List.iteri (set_entry) exprs;
			tupleName
			
	and add_condition condSym condExpr =
		match (condSym, condExpr) with
		| (None, None) -> None
		| (None, Some expr) -> Some (translate_expr expr)
		| (Some sym, None) -> Some sym
		| (Some _, Some _) -> raise (Stream.Error ("both is not implemented"))
			
	and translate_rule rule =
		let condition = add_condition (translate_pattern rule.pattern "$p") rule.condition in
		match condition with
			| None ->
				let resultSym = translate_expr rule.body in
				builder#add_instruction_without_result (Ir.ReturnInstruction resultSym)
			| Some c ->
				let activeBlock = builder#active_func#active_block in
				let branchName = builder#next_instruction_name in
				let thenBlock = builder#add_block in
				let resultSym = translate_expr rule.body in
				builder#add_instruction_without_result (Ir.ReturnInstruction resultSym);
				let elseBlock = builder#add_block in
				let fi: Ir.full_instruction = { instruction = (Ir.BranchInstruction(c, thenBlock#get_symbol_name, elseBlock#get_symbol_name)); returnName = branchName } in
				activeBlock#add_instruction fi
	in

	let translate_stmt stmt =
		match stmt with
		| Semantic_ast.ExpressionStatement expr -> Some(translate_expr expr)
		| Semantic_ast.LetBindingStatement (name, _, expr) ->
			let sym = translate_expr expr in
			Hashtbl.add namedValues name sym;
			None
		| _ -> raise (Stream.Error ("not implemented: " ^ Semantic_ast_print.stmt_to_string stmt))
	in

	let translate_module _module = List.filter_map (translate_stmt) _module.Semantic_ast.statements
	in
	
	let _ = builder#add_func in
	let rets = translate_module _module in
	let lastValue = List.hd (List.rev rets) in
	builder#add_instruction_without_result (Ir.ReturnInstruction lastValue);
	let _ = builder#finish_func in
	builder#get_funcs
;;
