#include "stdafx.h"
#include "QDE.h"


DWORD luaF=0;

inline void luaFstr(LPCSTR s)
{
 if(s[0]==0) return;
 bWriteFile((void*)s,strlen(s),1,luaF);
 bWriteFile("\n",1,1,luaF);
}

LUAFUNC(printQDE)
{
  GString S=lua::_tostring(L,1);
  GString Off=lua::_tostring(L,2);
  if(S=="_G")/* && Off.GetLength()>0)*/ return 0;
  S=Off+S;
  luaFstr(S);
  Off=S+".";
  lua::doVstring("for i,v in pairs(%s) do print2QDE(i,\"%s\");end;",(LPCSTR)S,LPCSTR(Off));
  return 0;
}

LUAFUNC(ToQDE)
{
  luaF=bOpenFile("luacore.uwl","wt+");

  luaFstr("[UserWords1]");
  lua::luafunction("print2QDE",printQDE);
  lua::object::iterator it_b = lua::GetVariables()->begin();
  lua::object::iterator it_e = lua::GetVariables()->end();

  //lua::dostring("function expand(a,b)\n print2QDE(a

   for(;it_b!=it_e;++it_b)
   {
   		lua::object key = it_b.key();
        lua::proxy_object &po = *it_b;
     	LPCSTR v=lua::get<const char *>(key);
        //luaFstr(v);
        if( po.type() == LUA_TTABLE)
            lua::doVstring("print2QDE(\"%s\");",v,v);

   }
  luaFstr("[END]");
  lua::dostring("print2QDE=nil;expand=nil;");
  bCloseFile(luaF);
  return 0;
}


    void CQDEexportLua::DoExport()
    {
      lua::luafunction("ToQDE",ToQDE,"utils.export");
    }
