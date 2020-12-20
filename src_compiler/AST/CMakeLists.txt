set(SOURCE_FILES_COMPILER
	${SOURCE_FILES_COMPILER}

	${CMAKE_CURRENT_SOURCE_DIR}/expressions/CallExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/Expression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/ExpressionList.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/FunctionExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/IdentifierExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/NaturalLiteralExpression.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/expressions/TupleExpression.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/leftValues/LeftValue.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/statements/ExpressionStatement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/ExternalDeclarationStatement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/ReturnStatement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/Statement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/StatementBlock.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/statements/VariableDefinitionStatement.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/typeSpecifications/FunctionTypeSpec.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/typeSpecifications/IdentifierTypeSpec.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/typeSpecifications/TupleTypeSpec.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/typeSpecifications/TypeSpec.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/visitors/ExpressionVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/StatementVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/visitors/TypeSpecVisitor.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/Node.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ParserState.hpp

	PARENT_SCOPE)