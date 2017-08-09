#include "stdafx.h"
#include "Camera.h"


LUAFUNC(luaCameraPosSet)
{
	World()->Camera()->SetPosition(bVector3f(
		                           lua_tonumber(L,1),
		                           lua_tonumber(L,2),
								   lua_tonumber(L,3)));
	return 0;
}

LUAFUNC(luaCameraPosGet)
{
	bVector3f B=World()->Camera()->GetPosition();
	lua_pushnumber(L,B[0]);
	lua_pushnumber(L,B[1]);
	lua_pushnumber(L,B[2]);
	return 3;
}

LUAFUNC(luaCameraAngSet)
{
	World()->Camera()->SetLookAngles(bVector3f(
		                             lua_tonumber(L,1),
		                             lua_tonumber(L,2),
								     lua_tonumber(L,3)));
	return 0;
}

LUAFUNC(luaCameraAngGet)
{
	bVector3f B=World()->Camera()->GetLookAngles();
	lua_pushnumber(L,B[0]);
	lua_pushnumber(L,B[1]);
	lua_pushnumber(L,B[2]);
	return 3;
}

LUAFUNC(luaCameraLookAt)
{
	    World()->Camera()->SetLookAt(bVector3f(
		                             lua_tonumber(L,1),
		                             lua_tonumber(L,2),
								     lua_tonumber(L,3)));
	return 0;
}

void CCameraexportLua::DoExport()
{
	lua::luafunction("GetPosition",luaCameraPosGet,"core.graphics.camera");
	lua::luafunction("SetPosition",luaCameraPosSet,"core.graphics.camera");
	lua::luafunction("SetAngles",luaCameraAngSet,"core.graphics.camera");
	lua::luafunction("GetAngles",luaCameraAngGet,"core.graphics.camera");
    lua::luafunction("LookAt",luaCameraLookAt,"core.graphics.camera");

}