add_subdirectory(acsb)
add_subdirectory(ast)
add_subdirectory(ir)

set(SOURCE_FILES
	${SOURCE_FILES}

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

	PARENT_SCOPE)