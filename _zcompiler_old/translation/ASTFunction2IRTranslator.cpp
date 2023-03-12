//Private methods
void ASTFunction2IRTranslator::Translate(const LinkedList<Tuple<UniquePointer<AST::Expression>, UniquePointer<AST::Expression>, AST::StatementBlock>> &rules)
{
	uint8 ruleNumber = 0;

	IR::BasicBlock* lastBlock = this->procedure.EntryBlock();
	for(const auto& rule : rules)
	{
		PatternMatching2IRTranslator patternMatchingTranslator(this->builder, this->procedure, lastBlock);
		IR::Value* condition = patternMatchingTranslator.Translate(*rule.Get<0>());

		if(!rule.Get<1>().IsNull())
		{
			this->PushBlock(lastBlock);
			rule.Get<1>()->Visit(*this);
			this->PopBlock();

			IR::Value* guardCondition = this->valueStack.Pop();
			if(condition)
			{
				this->PushBlock(lastBlock);

				IR::External* andExternal = this->builder.Module().FindExternal(u8"and");

				DynamicArray<IR::Value*> values;
				values.Push(condition);
				values.Push(guardCondition);
				IR::Instruction* argInstruction = this->builder.CreateNewTuple(Move(values));
				this->AddInstruction(argInstruction);

				IR::Instruction* andInstruction = this->builder.CreateExternalCall(andExternal, argInstruction);
				this->AddInstruction(andInstruction);

				this->PopBlock();

				condition = andInstruction;
			}
			else
				condition = guardCondition;
		}
	}
}
