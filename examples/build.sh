cd ../compiler
dune build
cd ../examples
mkdir -p _build
cd _build
../../compiler/_build/default/acsc.exe -i ../../acs_lib --target node ../$1
