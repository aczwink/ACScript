/*
* Copyright (c) 2018-2019 Amir Czwink (amir130@hotmail.de)
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
#include "Module.hpp"
#include "RuntimeObject.hpp"

namespace ACSB
{
	struct RuntimeModule
	{
		Module* module;
		RuntimeObject state;
		FixedArray<RuntimeObject> globals;

		inline RuntimeModule() : globals(0)
		{
		}

		inline RuntimeModule(Module& module) : module(&module), globals(module.GetNumberOfGlobals())
		{
		}

		RuntimeModule(RuntimeModule&&) = default;
		RuntimeModule& operator=(RuntimeModule&&) = default;
	};

	struct RuntimeCall
	{
		const uint8* pc;
		const Procedure* proc;
		RuntimeModule* module;
		RuntimeObject thisObj;
		RuntimeObject arg;
		FixedArray<RuntimeObject> locals;
		RuntimeObject ref;
		AtomicFlag running;

		inline RuntimeCall() : locals(0)
		{
		}

		inline RuntimeCall(const Procedure* proc, RuntimeModule* module, RuntimeObject thisObj, RuntimeObject arg)
			: proc(proc), module(module), thisObj(thisObj), arg(arg), locals(proc->GetNumberOfLocals())
		{
			this->pc = &module->module->GetCode()[proc->GetOffset()];
		}

		RuntimeCall(RuntimeCall&&) = default;
		RuntimeCall& operator=(RuntimeCall&&) = default;

		inline RuntimeObject CreateRef(RuntimeCall* call)
		{
			this->ref = RuntimeObject::TaskRef(call);
			return this->ref;
		}
	};

	class VM
	{
	public:
		//Destructor
		~VM();

		//Inline
		inline void AddModule(const String& moduleName, Module& module)
		{
			RuntimeModule m = { module };
			this->InitModule(m);
			this->modules[moduleName] = Move(m);
		}

		inline void RunModule(const String& module)
		{
			RuntimeModule& m = this->modules[module];
			this->AddTask(new RuntimeCall(&m.module->GetProcedure(0), &m, m.state, {})); //0 is main function
		}

		inline void WaitForTaskCompletion()
		{
			this->threadPool.WaitForAllTasksToComplete();
		}

	private:
		//Members
		Map<String, RuntimeModule> modules;
		StaticThreadPool threadPool;

		//Methods
		void InitModule(RuntimeModule& module);
		void ExecuteTask(RuntimeCall* task);
		void Run();

		//Inline
		inline void AddTask(RuntimeCall* task)
		{
			Function<void()> taskFunc([this, task]()
			{
				this->ExecuteTask(task);
			});
			this->threadPool.EnqueueTask(Move(taskFunc));
		}
	};
}

/*
class VM
{
public:
	//Destructor
	~VM();

private:
	DynamicArray<RuntimeObject> valueStack;
	DynamicArray<Map<String, RuntimeObject>> executionStack; //TODO: SLOW
};*/