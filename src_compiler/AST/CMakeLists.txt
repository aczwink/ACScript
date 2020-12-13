set(SOURCE_FILES_COMPILER
	${SOURCE_FILES_COMPILER}

	${CMAKE_CURRENT_SOURCE_DIR}/expressions/CallExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/Expression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/ExpressionList.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/NaturalLiteralExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/TupleExpression.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/statements/ExpressionStatement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/Statement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/StatementBlock.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/ASTNode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ASTVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/External.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ParserState.hpp

	PARENT_SCOPE)