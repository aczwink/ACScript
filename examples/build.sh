cd ../src_compiler
dune build || { echo 'Building failed' ; exit 1; }
cd ../examples
mkdir -p _build
cd _build
../../src_compiler/_build/default/acsc.exe -i ../../acs_lib --target acsb ../$1
#OUT_FILE=${1/acs/js}
#node $OUT_FILE
