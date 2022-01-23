module StringMap = Map.Make(String);;

class modules_collection = 
	object
	
	val mutable moduleNamesList = []
	val mutable modulesMap: Semantic_ast.program_module StringMap.t = StringMap.empty
	val symbolTable = new SymbolTable.symbol_table
	val typeSystem = new Type_system.type_system
	
	method add_module moduleName _module =
		modulesMap <- StringMap.add moduleName _module modulesMap;
		moduleNamesList <- moduleNamesList @ [moduleName]
		
	method get_module moduleName =
		StringMap.find moduleName modulesMap
		
	method get_symbol_table =
		symbolTable
		
	method get_type_system =
		typeSystem
	
	method modules_in_order =
		List.map (fun moduleName -> StringMap.find moduleName modulesMap) moduleNamesList
end;;
