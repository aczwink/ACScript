/*
* Copyright (c) 2020-2021 Amir Czwink (amir130@hotmail.de)
*
* This file is part of ACScript.
*
* ACScript is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ACScript is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ACScript.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "expressions/CallExpression.hpp"
#include "expressions/Expression.hpp"
#include "expressions/ExpressionList.hpp"
#include "expressions/FunctionExpression.hpp"
#include "expressions/IdentifierExpression.hpp"
#include "expressions/NaturalLiteralExpression.hpp"
#include "expressions/ObjectExpression.hpp"
#include "expressions/SelectExpression.hpp"
#include "expressions/TupleExpression.hpp"

#include "leftValues/IdentifierLeftValue.hpp"

#include "statements/ExpressionStatement.hpp"
#include "statements/StatementBlock.hpp"
#include "statements/VariableDefinitionStatement.hpp"

#include "typeSpecifications/FunctionTypeSpec.hpp"
#include "typeSpecifications/IdentifierTypeSpec.hpp"
#include "typeSpecifications/TupleTypeSpec.hpp"

#include "statements/ExternalDeclarationStatement.hpp"
#include "statements/TypeDeclarationStatement.hpp"