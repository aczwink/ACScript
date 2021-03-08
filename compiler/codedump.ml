let output_uint16 oc i =
	output_byte oc (Int.shift_right i 8);
	output_byte oc i

let dump_module inputFilePath = 
	let oc = open_out (inputFilePath ^ ".ascb") in
		output_string oc "ACSB";
		output_uint16 oc 0; (* number of imports *)
		output_uint16 oc 0; (* number of constants *)
		output_uint16 oc 0; (* entry point *)
		(* rest is code *)
		close_out oc
