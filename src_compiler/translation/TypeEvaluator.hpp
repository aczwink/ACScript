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
//Local
#include "../AST/typeSpecifications/TypeSpec.hpp"
#include "../types/Type.hpp"
#include "../types/TypeCatalog.hpp"

namespace Translation
{
	class TypeEvaluator : private AST::TypeSpecVisitor
	{
	public:
		//Constructor
		inline TypeEvaluator(TypeCatalog& typeCatalog) : typeCatalog(typeCatalog)
		{
		}

		//Methods
		const ::Type *Evaluate(const AST::TypeSpec& typeSpec);

	private:
		//Members
		TypeCatalog& typeCatalog;
		DynamicArray<const ::Type*> typeStack;

		//Methods
		void OnVisitedFunctionTypeSpec(const AST::FunctionTypeSpec &functionTypeSpec) override;
		void OnVisitedIdentifierTypeSpec(const AST::IdentifierTypeSpec &identifierTypeSpec) override;
		void OnVisitedTupleTypeSpec(const AST::TupleTypeSpec &tupleTypeSpec) override;
	};
}