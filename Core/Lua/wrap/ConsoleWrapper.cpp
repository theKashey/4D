#include "stdafx.h"
#include "ConsoleWrapper.h"

void luaP2Console(int SYS,LPCSTR Str)
{
	Print2Console(SYS,Str);
}

//extern DWORD lua::LUA_SYSTEM;

LUAFUNC(luaEcho)
{
      GString S = lua_tostring(L, 1);
      Print2Console(lua::LUA_SYSTEM,S);
      return 0; 
}

LUAFUNC(luaPrint)
{
	  DWORD   SYS= lua_tonumber(L, 1);
      GString S  = lua_tostring(L, 2);
      Print2Console(SYS,S);
      return 0; 
}

LUAFUNC(luaStr) 
{
	DWORD thistype=lua_type(L,1);
	switch(thistype)
	{
	case LUA_TNONE	        :
	case LUA_TNIL	        :
	case LUA_TLIGHTUSERDATA	:
	case LUA_TTABLE	        :
	case LUA_TFUNCTION	    :
	case LUA_TUSERDATA	    :
	case LUA_TTHREAD	    :
		lua_pushstring(L,lua_typename(L,thistype));
		return 1;

	case LUA_TBOOLEAN	    :
	case LUA_TNUMBER	    :
	case LUA_TSTRING	    :
		lua_pushstring(L,lua_tostring(L,1));
		return 1;
	}
	lua_pushstring(L,lua_typename(L,LUA_TNONE));
	return 1;
}

#pragma comment(lib,"Comdlg32.lib")

LUAFUNC(OPDDialog)
{
    static char name[0xFF*2];
    GString  _title =lua::_tostring(L,1);
    GString  _ext   =lua::_tostring(L,2);
    LPCSTR title=_title;
    //LPCSTR ext  =_ext;
    char ext2[0xFF];strcpy_s(ext2,sizeof(ext2),_ext);
    LPSTR ext=ext2;
    ext[strlen(ext)+1]=0;
    if(ext && ext[0]==0) ext=NULL;
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize    = sizeof(OPENFILENAME);
    ofn.hwndOwner      = (HWND)lua::getdword("win_hwnd");
	ofn.lpstrFileTitle = name;
	ofn.nMaxFileTitle  = 512;
	if(ext==NULL)
	 ofn.lpstrFilter   = "*.*\0\0";
	else
	 ofn.lpstrFilter   = ext;
	ofn.nFilterIndex   = 1;
	ofn.lpstrTitle     = title;
	ofn.Flags          = OFN_FILEMUSTEXIST | OFN_EXPLORER ;
	DWORD ret=GetOpenFileName(&ofn);
    if(ret)
    {
     lua_pushnumber(L,1);
     lua_pushstring(L,name);
     lua::setstring("OPDfilename",name);
     return 2;
    }
    {
     lua_pushnumber(L,0);
     return 1;
    }
}

LUAFUNC(SPDDialog)
{
    static char name[0xFF*2];
    GString  _title =lua::_tostring(L,1);
    GString  _ext   =lua::_tostring(L,2);
    LPCSTR title=_title;
    //LPCSTR ext  =_ext;
    char ext2[0xFF];strcpy_s(ext2,sizeof(ext2),_ext);
    LPSTR ext=ext2;
    ext[strlen(ext)+1]=0;
    if(ext && ext[0]==0) ext=NULL;
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize    = sizeof(OPENFILENAME);
    ofn.hwndOwner      = (HWND)lua::getdword("win_hwnd");
	ofn.lpstrFileTitle = name;
	ofn.nMaxFileTitle  = 512;
	if(ext==NULL)
	 ofn.lpstrFilter   = "*.*\0\0";
	else
	 ofn.lpstrFilter   = ext;
	ofn.nFilterIndex   = 1;
	ofn.lpstrTitle     = title;
	ofn.Flags          = OFN_EXPLORER ;
	DWORD ret=GetSaveFileName(&ofn);
    if(ret)
    {
     lua_pushnumber(L,1);
     lua_pushstring(L,name);
     lua::setstring("SPDfilename",name);
     return 2;
    }
    {
     lua_pushnumber(L,0);
     return 1;
    }
}



#include "math.h"

void luaP2ConsoleD(int SYS,float f)
{
	GString S;
	if(f-floor(f)<0.000001f || f-ceil(f)<0.000001f)
	 S.Format("%d",(int)f);
	else
	 S.Format("%f",f);
	Print2Console(SYS,S);
}

void luaE2Console(int SYS,LPCSTR Str)
{
	Error2Console(SYS,Str);
}

void luaPrintStack()
{
	Print2Console(lua::LUA_SYSTEM,"stack---------------------");
	lua::PrintStack();
	Print2Console(lua::LUA_SYSTEM,"--------------------------");
}

LUAFUNC(luaTime)
{
	lua_pushnumber(L, Time::Get());
 return 1;
}

LUAFUNC(luaDO)
{
  GString Str=lua_tostring(L,1);
  bool ret=false;
  if(Str.GetLength()>0)
      ret=lua::dostring(Str)!=0;
  lua_pushnumber(L,ret);
  return 1;
}

LUAFUNC(luaFILE)
{
  GString Str=lua_tostring(L,1);
  bool ret=false;
  if(Str.GetLength()>0)
      ret=lua::dofile(Str)!=0;
  lua_pushnumber(L,ret);
  return 1;
}

#pragma comment (lib,"shell32.lib")

LUAFUNC(luaShellExec)
{
  GString Name=lua_tostring(L,1);
  Print2Console(lua::LUA_SYSTEM,"ssexec:%s return %d",(LPCSTR)Name,ShellExecute(0, NULL,Name,NULL,NULL,1));
  
  return 0;
}


	void CLuaConsole::DoExport()
	{
		lua::luafunction("echo",luaEcho);
		lua::luafunction("Print2Console",luaPrint);
		lua::luafunction("time",luaTime);
		lua::luafunction("str",luaStr);

        lua::luafunction("DO",luaDO);
        lua::luafunction("request",luaFILE);
		
		lua::function_def("printstack"     ,luaPrintStack);
		lua::function_def("Error2Console"  ,luaE2Console);
        lua::dostring("utils.dialog={}");
        lua::dostring("utils.convert={}");
        lua::dostring("utils.models={}");

        lua::dostring("os={}");
        lua::luafunction("OpenFileDialog",OPDDialog,"utils.dialog");
        lua::luafunction("SaveFileDialog",SPDDialog,"utils.dialog");

        lua::luafunction("shellexec",luaShellExec,"os");
        

		//lua::function_def("time",__GETCURTIME());

		lua::dostring("function win_setmode(a,b) win_width=a;win_height=b;win_updateviewport=1;end;");

		LuaObject::DoExport();
	}
