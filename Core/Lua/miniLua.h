#ifndef MINI_LUA_H
#define MINI_LUA_H

extern "C" 
{
#
#	include "lua/lua.h"
#	include "lua/lauxlib.h"
#	include "lua/lualib.h"
#
}

namespace lua
{
	enum
	{
		TYPE_NULL     =	LUA_TNIL,
		TYPE_BOOL     = LUA_TBOOLEAN,
		TYPE_STRUCT   = LUA_TLIGHTUSERDATA,
		TYPE_INTEGER  = LUA_TNUMBER,
		TYPE_STRING   = LUA_TSTRING,
		TYPE_FUNCTION = LUA_TFUNCTION,
		TYPE_CLASS	  = LUA_TUSERDATA,
		TYPE_TABLE    = LUA_TTABLE,
		TYPE_THREAD   = LUA_TTHREAD
	};

	typedef lua_State * plua_State;
	plua_State DLLTYPE GetState();


	GString DLLTYPE PrintStack(bool ReturnStack=false);
	void DLLTYPE init();
	void DLLTYPE close();
	void DLLTYPE update();

	int DLLTYPE dofile            (LPCSTR file);
	int DLLTYPE dobuffer          (LPCSTR, size_t sz,LPCSTR ChunkName=NULL);
	int DLLTYPE dostring          (LPCSTR,LPCSTR ChunkName=NULL);
    int DLLTYPE doVstring         (LPCSTR,...);
	int DLLTYPE dofunction        (LPCSTR namefunction, LPCSTR value);
	int DLLTYPE callintfunction   (LPCSTR namefunction, int value);
	LPCSTR DLLTYPE callstrfunction(LPCSTR namefunction, LPCSTR value);
    void DLLTYPE callfunction     (LPCSTR namefunction);

    typedef SmartVector<GString> LuaScanTree;
	void DLLTYPE GrabLuaTree(LuaScanTree *ST);


	LPCSTR DLLTYPE ExpandLine(LPCSTR Line,bool ConsoleOut);

	int    DLLTYPE type     (LPCSTR name);
	void   DLLTYPE setfloat (LPCSTR Name,const double val);
	void   DLLTYPE setstring(LPCSTR Name,LPCSTR val);
	void   DLLTYPE setdword (LPCSTR Name,const DWORD val);
	double DLLTYPE getfloat (LPCSTR Name);
	LPCSTR DLLTYPE getstring(LPCSTR Name);
	DWORD  DLLTYPE getdword (LPCSTR Name);

	void   DLLTYPE setVfloat (LPCSTR Name,const double val,...);
	void   DLLTYPE setVstring(LPCSTR Name,LPCSTR val,...);
	void   DLLTYPE setVdword (LPCSTR Name,const DWORD val,...);
	double DLLTYPE getVfloat (LPCSTR Name,...);
	LPCSTR DLLTYPE getVstring(LPCSTR Name,...);
	DWORD  DLLTYPE getVdword (LPCSTR Name,...);

	void   DLLTYPE supresswarning(int on);


	typedef void(*luaLPCSTRF)(LPCSTR);
	void  DLLTYPE createLPCSTRfunction(LPCSTR Name,luaLPCSTRF f);

    void DLLTYPE luafunction(LPCSTR Name,lua_CFunction fn,LPCSTR MoveTo=NULL);
    #define LUAFUNC(a) static int a(lua_State *L) 

    GString DLLTYPE _tostring(lua_State *L,const int Index);
    double  DLLTYPE _tonumber(lua_State *L,const int Index);

    void DLLTYPE pushstring(lua_State *L,LPCSTR S);
    void DLLTYPE pushnumber(lua_State *L,double I);

	int    DLLTYPE seterror(LPCSTR Err,int code,int place);
	LPCSTR DLLTYPE geterror();

    struct CLuaVar
    {
      char   Name[0xFF];
      DWORD  Type;
    };
    typedef CLuaVar *PLuaVar;
    PLuaVar DLLTYPE ScanLuaFolder(LPCSTR Name,int *len);


	class luavarbase
	{
	protected:
		GString name;
		bool    Valid;
		bool    IsValid()const{return Valid;}
	public:
		luavarbase(const char *var):	name(var){Valid=true;}
		luavarbase(){Valid=false;};
		void SetName(LPCSTR Name){name=Name;Valid=true;}
	};


	class luaint:public luavarbase
    {
	public:
		BINLINE luaint & operator = (const float& var){if(IsValid())setfloat(name.c_str(),var);return *this;}
		BINLINE operator float ()const{if(IsValid())return getfloat(name.c_str());else return 0;}
		BINLINE operator int   ()const{if(IsValid())return getdword(name.c_str());else return 0;}
	};

	class luastring:public luavarbase
    {
	public:
		BINLINE luastring & operator = (LPCSTR var){if(IsValid())setstring(name.c_str(),var);return *this;}
		BINLINE operator LPCSTR  ()const{if(IsValid())return getstring(name.c_str());else return 0;}
		BINLINE operator GString ()const{if(IsValid())return getstring(name.c_str());else return 0;}
	};

	class DLLTYPE LuaObject
	{
		static void AddRemoveObject(LuaObject *O,const int AR);
		bool Started;
		static int CurrentLevel;
		DWORD StartLevel;
	protected:
		virtual void DoExport();
	public:
		LuaObject(const int SL=0);
		~LuaObject();
		void SetStartLevel(const int SL);
		virtual void luaOnStart      (const DWORD Level);
		virtual void luaOnStop       (const DWORD Level);
		virtual void luaOnModuleStart(const DWORD Level);
		virtual void luaOnModuleStop (const DWORD Level);

		static void InitModules();
		static void SetInitDelay(const bool Delay);
		static void EnterLevel  (const int CL);
	};

template <class T>
class CLuaAutoWrapper:public LuaObject
{
protected:
	void DoExport()
	{
		T();
		LuaObject::DoExport();
	}
};
};




#endif