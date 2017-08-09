

class CLuaConsole:public lua::LuaObject
{
public:
	CLuaConsole():lua::LuaObject(1)	{};
	void DoExport();
};