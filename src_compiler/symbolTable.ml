module StringSet = Set.Make(String);;

class symbol_table =
	object(this)
	
	val prefixTable = Hashtbl.create 10
	val mutable names = StringSet.empty
	val typeTable = Hashtbl.create 10
	
	method create (name: string) (typedef: Type_system.typedefinition) =
		let global_name = this#create_next_name name in
		if StringSet.mem global_name names then this#create name typedef else this#add_symbol global_name typedef
			
	method get_type global_name =
		Hashtbl.find typeTable global_name
		
	(* Private methods*)
	method private add_symbol global_name typedef =
		names <- StringSet.add global_name names;
		Hashtbl.add typeTable global_name typedef;
		global_name
		
	method private create_next_name prefix =
		match Hashtbl.find_opt prefixTable prefix with
		| Some num ->
			Hashtbl.add prefixTable prefix (num+1);
			prefix ^ (string_of_int num)
		| None ->
			Hashtbl.add prefixTable prefix 1;
			prefix
end;;

class context (symbolTable: symbol_table) =
	object
	
	method get_self_type =
		Type_system.Function(Type_system.Unknown, Type_system.Unknown)
	
	method get_type global_name =
		symbolTable#get_type global_name
end;;
