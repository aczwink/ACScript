set(SOURCE_FILES_COMPILER
	${SOURCE_FILES_COMPILER}

	${CMAKE_CURRENT_SOURCE_DIR}/instructions/BranchOnTrueInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/CallInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/CreateNewObjectInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/CreateNewTupleInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/ExternalCallInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/ReturnInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/SelectInstruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/instructions/StoreInstruction.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/visitors/AllSymbols.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/BasicBlockVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/Printer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/Printer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/ValueVisitor.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/BasicBlock.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Builder.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ConstantFloat.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ConstantString.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Environment.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/External.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Instruction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Module.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NullValue.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ObjectValue.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ObjectValue.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Procedure.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Scope.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Symbol.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/TupleValue.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Value.hpp

	PARENT_SCOPE)