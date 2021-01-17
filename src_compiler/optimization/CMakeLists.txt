set(SOURCE_FILES_COMPILER
	${SOURCE_FILES_COMPILER}

	${CMAKE_CURRENT_SOURCE_DIR}/typeInference/ProcedureTypeInferer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/typeInference/ProcedureTypeInferer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/typeInference/ProcedureTypeReplacer.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/CallGraph.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DependencyGraph.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/PassManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ProcedureTransformer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ValueEvaluator.hpp

	PARENT_SCOPE)