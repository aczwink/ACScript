add_subdirectory(acsb)
add_subdirectory(AST)
add_subdirectory(IR)
add_subdirectory(llvm)
add_subdirectory(optimization)
add_subdirectory(translation)
add_subdirectory(types)

set(SOURCE_FILES_COMPILER
	${SOURCE_FILES_COMPILER}

	${CMAKE_CURRENT_SOURCE_DIR}/compile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CompileException.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

	PARENT_SCOPE)