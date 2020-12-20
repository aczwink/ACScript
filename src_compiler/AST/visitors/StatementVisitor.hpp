/*
* Copyright (c) 2020 Amir Czwink (amir130@hotmail.de)
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
#pragma once

namespace AST
{
	//Forward declarations
	class ExpressionStatement;
	class ExternalDeclarationStatement;
	class ReturnStatement;
	class VariableDefinitionStatement;

	class StatementVisitor
	{
	public:
		virtual void OnVisitingExpressionStatement(const ExpressionStatement& expressionStatement) = 0;
		virtual void OnVisitingExternalDeclaration(const ExternalDeclarationStatement& externalDeclaration) = 0;
		virtual void OnVisitingReturnStatement(const ReturnStatement& returnStatement) = 0;
		virtual void OnVisitingVariableDefinitionStatement(const VariableDefinitionStatement& variableDefinitionStatement) = 0;
	};
}