exception TypeException of Type_system.typedefinition * Type_system.typedefinition * (int, Type_system.typedefinition) Hashtbl.t * Type_system.type_system

let typeMap_to_string typeMap typeSystem =
	let entries = Hashtbl.fold (fun k _ acc -> (Type_system.generic_to_string k (typeSystem#get_generic_constraints k))::acc) typeMap [] in
	String.concat "\n" entries
;;

let format_exception e =
	match e with
	| TypeException (from, _to, typeMap, typeSystem) -> "Can't assign from: " ^ (Type_system.to_string from) ^ " to: " ^ (Type_system.to_string _to) ^ " with: " ^ (typeMap_to_string typeMap typeSystem)
	| _ -> Printexc.to_string e ^ "\n" ^ Printexc.get_backtrace ()
;;
