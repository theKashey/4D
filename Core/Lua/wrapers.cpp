#include "stdafx.h"
#include "wrap/SoundWrapper.h"
#include "wrap/ConsoleWrapper.h"
#include "wrap/QDE.h"
#include "wrap/Camera.h"

void lua_start()
{
	lua::LuaObject *Obj=NULL;
	
	Obj=new CLuaConsole     ();
	Obj=new CLuaSound       ();
    Obj=new CQDEexportLua   ();
    Obj=new CCameraexportLua();
}