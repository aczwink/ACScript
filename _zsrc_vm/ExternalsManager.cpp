//Private methods
void ExternalsManager::RegisterAllExternals()
{
	//logical
	extern RuntimeValue And(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"and", And);

	extern RuntimeValue LessThan(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"<", LessThan);

	extern RuntimeValue Not(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"not", Not);

	extern RuntimeValue Or(RuntimeValue&, const Module&);
	this->RegisterExternal(u8"or", Or);
}
