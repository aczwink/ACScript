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
//Local
#include "../IR/Module.hpp"
#include "CallGraph.hpp"

namespace Optimization
{
	class PassManager
	{
	public:
		//Constructor
		PassManager(IR::Module& module, TypeCatalog& typeCatalog) : module(module), typeCatalog(typeCatalog)
		{
			DependencyGraph dependencyGraph(this->module.Procedures());
			DynamicArray<IR::Procedure*> orderedProcedures = dependencyGraph.QueryProceduresOrdered();
			module.Procedures(orderedProcedures);
		}

		//Inline
		template<typename T>
		inline void AddProcedurePass(const String& passName)
		{
			for(IR::Procedure*const& procedure : module.Procedures())
			{
				T pass(*procedure, this->typeCatalog);
				pass.Transform();
			}

			stdOut << u8"After pass '" << passName << u8"': " << endl;
			this->PrintState();
		}

		inline void PrintState()
		{
			IR::Printer printer(stdOut);
			printer.Print(this->module);
		}

	private:
		//Members
		IR::Module& module;
		TypeCatalog& typeCatalog;
	};
}