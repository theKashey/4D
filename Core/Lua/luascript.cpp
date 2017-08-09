/********************************************************************
filename: 	lua.cpp
author:		(c)IROV
*********************************************************************/
#include "stdafx.h"
#
#	include "luascript.h"
#
//#	pragma comment (lib,"blopcore.lib")
//#	pragma comment (lib,"liblua.lib")
//#	pragma comment (lib,"liblualib.lib") 
//#	pragma comment (lib,"luabind.lib")
#                       
#

extern void lua_start();

namespace lua{
	lua_State *state = 0;
	object variables;
	DWORD LUA_SYSTEM=0;

#define LUA_STACE_STRINGS     1
#define LUA_TRACE_FUNCTIONDEF 2

    DWORD LUA_TRACEMODE=0;


	plua_State GetState(){return state;}
	pobject    GetVariables(){return &variables;}

    void setTraceMode(DWORD Mode)
    {
      LUA_TRACEMODE=Mode;
    }

LUAFUNC(bOR ){ DWORD A=lua_tonumber(L,1);DWORD B=lua_tonumber(L,2);lua_pushnumber(L,A|B);return 1;}
LUAFUNC(bAND){ DWORD A=lua_tonumber(L,1);DWORD B=lua_tonumber(L,2);lua_pushnumber(L,A&B);return 1;}


	void init()
	{
		state = lua_open();

		luaopen_base  (state);
        luaopen_io    (state);
        luaopen_string(state);
        luaopen_math  (state);

		luabind::open(state);
		lua_start();
		variables = luabind::get_globals(state);
		if(!LUA_SYSTEM)
			RegisterNewConsoleClass(LUA_SYSTEM,"LUA",-1);
        lua::function_def("lua_settrace",setTraceMode);
        //lua::function_def("bOR",bOR);
        //lua::function_def("bAND",bAND);
        lua::luafunction("bOR" ,bOR );
        lua::luafunction("bAND",bAND);
	}

	void close()
	{
		if( state )
		{
			lua_close(state);
			free(state);
		}
		
	}

	void update()
	{
		variables = luabind::get_globals(state);
	}


	LPCSTR getclassName(DWORD Type)
	{
		switch(Type)
		{
		  case lua::TYPE_NULL     :return "NULL";
		  case lua::TYPE_BOOL     :return "bool";
		  case lua::TYPE_STRUCT   :return "struct";
		  case lua::TYPE_INTEGER  :return "int";
		  case lua::TYPE_STRING   :return "string";
		  case lua::TYPE_FUNCTION :return "function";
		  case lua::TYPE_CLASS	  :return "class";
		  case lua::TYPE_TABLE    :return "table";
		  case lua::TYPE_THREAD   :return "thread";
		}
		static char buf[16];
		sprintf_s(buf,sizeof(buf),"undef#%d",Type);
		return buf;
	}

	int type(const char *name)
	{
		return GetFinalObject(name).type();//variables[name].type();
	}

	int dofile(const char *file)
	{
		DWORD f=bOpenFile(file,"rb",true);
		if(!f)
		{
			Warning2Console(LUA_SYSTEM," file <%s> not found ",file);
			return -1;
		}
		int ret=0;
		int i=bFileSize(f);
		LPCSTR  Data=(LPCSTR)bGetFileData(f); 
		GString Fstr=Data;

		for(GString::iterator it=Fstr.begin();it!=Fstr.end();++it,--i)
		{
			if(*it==10) *it=' ';
			if(i<=0)
			{
				*it=0;
				break;
			}
		}
		if((ret=dostring(Fstr,GString("@")+file)))
			Warning2Console(LUA_SYSTEM," -- in file file <%s>-- ",file)	;
		bCloseFile(f);
		return ret;
		/*int i_line = luaL_loadfile(state,file);
		if( i_line != 0 )
		{
			const char * ErrorString = lua_tostring(state, -1);
			Warning2Console(LUA_SYSTEM,"lua [%s]",ErrorString);
			Warning2Console(LUA_SYSTEM,"lua [file = %s]",file);
			Warning2Console(LUA_SYSTEM,"lua [code = %d]",i_line);
			Error2Console(LUA_SYSTEM,"Lua Error");
			lua_pop(state, 1);
		}
		else
		{
			lua_pcall(state, 0, 0, 0);
			//lua_dofile(state,file);
		}
		return i_line;*/
	}

	GString lua_currenterror;
	static int lua_errorreport=0;

	void supresswarning(int on)
	{
		lua_errorreport=on;
	}

	int seterror(LPCSTR Err,int code,int place)
	{
		if(lua_errorreport)return code;
		if(code==0)
		{
		  lua_currenterror.Empty();
		  return 0;
		}
		GString CodeS;
		switch(code)
		{
			case 0         : CodeS="magic";break;
			case LUA_YIELD : CodeS="YIELD";break;
			case LUA_ERRRUN: CodeS="runtime";break;
			case LUA_ERRSYNTAX:CodeS="syntax";break;
			case LUA_ERRMEM: CodeS="memory";break;
			case LUA_ERRERR: CodeS="x2";break;
			default        :  CodeS.Format("unknown%d",code);break;
		};
		GString PlaceS=place==0?"load":"call";
		lua_currenterror.Format("%s error %s - [%s]",CodeS.c_str(),PlaceS.c_str(),Err);
		Error2Console(LUA_SYSTEM,lua_currenterror);
		return code;
	}

	LPCSTR geterror()
	{
		return lua_currenterror;
	}

	int dobuffer(const char *buff, size_t sz,LPCSTR ChunkName)
	{
		if(!ChunkName)ChunkName=buff;
		int i_line = luaL_loadbuffer(state, buff,strlen(buff), ChunkName);
		if ( i_line != 0 )
			return lua::seterror(lua_tostring(state, -1),i_line,0);
		else
		{
			i_line=lua_pcall(state, 0, 0, 0);
			if(i_line!=0)
			  return lua::seterror(lua_tostring(state, -1),i_line,1);
		}
		return lua::seterror(0,0,0);
		//return i_line;
	}

	int doVstring(const char* _str,...)
	{
      DOFORMAT(T,_str);
      return dostring(T);
    }
    int dostring(const char* _str,LPCSTR ChunkName)
    {
		LPCSTR str=_str;
		if(!str || str[0]==0)return 0;
		if(str[0]=='=')
		{
			if(str[1]=='@') PrintStack();
			else if(str[1]=='=')
			{
				Print2Console(LUA_SYSTEM,"no inline SET");
			}
			else
			if(str[1]=='>')
			{
				doVstring(GString("")+
					//"function str(a)7 end;
						 "for i,v in pairs(%s) do \n"+
						 "echo(\">\"..i..\"->\"..str(v));\n"+
						 "end\n ",str+2);
				//dostring(GString("scan(")+(str+2)+")");
			}
			else
			{
				doVstring(" echo \"%s =\"..(%s);",str+1,str+1);
					//tmpretval=%s",str+1);//
				//Print2Console(LUA_SYSTEM,"%s = %s",str+1,getstring("tmpretval"));
				//dostring("tmpretval=nil");
			}
			return 1;
		}
		

		
		return lua::dobuffer(str,strlen(str),ChunkName);
	} 


	GString glFindBestMatch(LPCSTR Line,bool ConsoleOut)
	{
		DWORD Len=strlen(Line);
		lua::object::iterator it_b = lua::variables.begin();
    	lua::object::iterator it_e = lua::variables.end();
		//GString Out,Buf;
		GString Match;
		GString NearMatch=Line;
		int NumMatches=0;
	    for(;it_b!=it_e;++it_b)
	    {
     		lua::object key = it_b.key();
		    lua::proxy_object &po = *it_b;
			LPCSTR v=lua::get<const char *>(key);
			if(strncmp(Line,v,Len)==0)
			{

	        	NumMatches++;
				if(NumMatches>1 && ConsoleOut)
				{
					if(NumMatches==2)Print2Console(LUA_SYSTEM,Match);
                    Print2Console(LUA_SYSTEM,v);
				}
				if(NumMatches==1) NearMatch=v;
				else
				{
				//update near match
					int Len=MIN(NearMatch.length(),(int)strlen(v));
					int j=0;
					for(j=0;j<Len;++j)
						if(NearMatch.c_str()[j]!=v[j])break;
		            NearMatch.v_str()[j]=0;
					NearMatch.CalcSize();
				}


				Match=v;
			}
		}
		if(NumMatches!=1) return NearMatch;
		else return Match;
	}

	int scan_matches=0;
	int scan_matchesout=0;
	GString scan_match,scan_check,scan_nearmatch;
	int scan_len=0;

	typedef void(*tscancallback)(LPCSTR);
	tscancallback fscancallback=NULL;
	void scancallback(tscancallback f){fscancallback=f;}


	static int scanret(lua_State *L)
	{
		LPCSTR str=lua_tostring(L,1);
		if(!scan_check || strncmp(scan_check,str,scan_len)==0)
		{
			scan_matches++;
			if(fscancallback)fscancallback(str);
			if(scan_matches>1 && scan_matchesout)
			{
				if(scan_matches==2)Print2Console(LUA_SYSTEM,scan_match);
                                   Print2Console(LUA_SYSTEM,str);
			}
			if(scan_matches==1) scan_nearmatch=str;
			else
			{
			//update near match
				int Len=MIN(scan_nearmatch.length(),(int)strlen(str));
				int j=0;
				for(j=0;j<Len;++j)
					if(scan_nearmatch.c_str()[j]!=str[j])break;
		        scan_nearmatch.v_str()[j]=0;
				scan_nearmatch.CalcSize();
			}
			scan_match=str;
		}
		return 0;
	}

	void scanbegin(LPCSTR M,bool ConsoleOut)
	{
		scan_matches=0;
		scan_matchesout=ConsoleOut;
		scan_check=M;
		scan_nearmatch=M;
		scan_len=strlen(M);
		scancallback(NULL);
		lua::luafunction("scanret",scanret);
	}

	GString tlFindBestMatch(LPCSTR Line,bool ConsoleOut)
	{
		GString Match;
		int Len=strlen(Line);
		int i=0;int Mi=0;
		for(i;i<Len;++i)if(Line[i]=='.'){Match=Line;Match.v_str()[i]=0;Mi=i+1;}
		scanbegin(Line+Mi,ConsoleOut);
		doVstring("for i,v in pairs(%s) do scanret(i);end;",Match.c_str());
        if(scan_matches==0)return Match;
        if(scan_matches!=1) return Match+"."+scan_nearmatch;
		return Match+"."+scan_match;
	}

	LPCSTR ExpandLine(LPCSTR Line,bool ConsoleOut)
	{
		if(!Line)return NULL;
		if(Line[0]==0)return Line;
		//first find last token
		//tokenbreaks..
		/*
		 ()
		 []
		 ..
		 +-/*^
		 &nbsp :)
		 ;
		*/
		//as result we need to find part of global variable
		//or name of table and part of index
		int Len=strlen(Line);
		int i=Len;
		for(i=Len;i>=0;--i)
		{
		  bool br=false;
          switch(Line[i])
		  {
			case '(': case ')':
			//case '[': case ']':
			case ' ':
			case '+':
			case '-':
			case '*':
			case '/':
			case '^':
			case ';':
			case '=':
			case '@':
			case ',':
			//case ':':
				br=true;break;
			case '.': if(i>0 && Line[i-1]=='.'){ br=true;break;}
		  }
		  if(br)break;
		}
		//if(i<0)i=0;
		i++;
		//if(!i)i=Len;
		if(1)//i)
		{
			GString L=Line;
			L.v_str()[i]=0;
			L.CalcSize();
			if(NOTTABLE(Line+i)==NT_NOT)
				L+= glFindBestMatch(Line+i,ConsoleOut);
			else
                L+= tlFindBestMatch(Line+i,ConsoleOut);
			static GString defExpand;
			defExpand=L;
			return defExpand; 
		}
		else
		return Line;
	}

	int dofunction(const char *namefunction, const char *value,...)
	{
		GString str = namefunction;
		DOFORMAT(Val,value);
		str+="(";
		if( value )
		{
			str+=Val;
		}
		str+=");";
		return dostring(str.c_str());
	}

	int doclassfunction(const char *nameclass,const char *namefunction,const char *value,...)
	{
		DOFORMAT(Val,value);
		GString str = nameclass;
		str+=":";
		str+=namefunction;
		str+="(";
		str+=Val;
		str+=");";
		return dostring(str.c_str());
	}

	GString PrintStack(bool ReturnStack)
	{
		lua::object::iterator it_b = lua::variables.begin();
    	lua::object::iterator it_e = lua::variables.end();
		GString Out,Buf;
	    for(;it_b!=it_e;++it_b)
	    {
     		lua::object key = it_b.key();
		    lua::proxy_object &po = *it_b;
			
			Buf.Format("[%-20s]",lua::get<const char *>(key));
		    //printf("[%s]  ",lua::get<const char *>(key),key.type(),po.type());
			Out+=Buf;
			Buf.Format("%-20s",lua::getclassName(po.type()));
			Out+=Buf;
			Buf="";
		    switch( po.type() )
		    {
		      case lua::TYPE_INTEGER:
				Buf.Format("= %d",lua::get<int>(po));
			    break;
		      case lua::TYPE_STRING:		 
				Buf.Format("= '%s'",lua::get<LPCSTR>(po));
			    break;
			};
			
			if(ReturnStack)
				Out+=Buf+"\n";
			else
			{
				Out+=Buf;
				Print2Console(LUA_SYSTEM,Out);
				Out="";
			}
		}   /**/
		if(ReturnStack) return Out;
		else
			Print2Console(LUA_SYSTEM,Out);
		return "";
	}



// LUA OBJECT	

	bool LuaO_Delay=true;

	int LuaObject::CurrentLevel=0;

	void LuaObject::EnterLevel(const int CL)
	{
		CurrentLevel=CL;
		const bool LOD=LuaO_Delay;
		LuaO_Delay=false;
        InitModules();
		LuaO_Delay=LOD;
	}

	void LuaObject::AddRemoveObject(LuaObject *O,const int AR)
	{
		typedef SmartVector<LuaObject*> CLuaVector;
		static CLuaVector LuaVector;
		if(AR==2)
		{
			for(CLuaVector::iterator it=LuaVector.begin(); it!=LuaVector.end();++it)
				if((*it)!=NULL)
				   (*it)->luaOnStart(CurrentLevel);
		}
		else
		if(AR==1)
		{
			for(CLuaVector::iterator it=LuaVector.begin(); it!=LuaVector.end();++it)
				if((*it)==NULL)
				{
					*it=O;
					O->luaOnStart(CurrentLevel);
					return;
				}
			LuaVector.push_back(O);
			O->luaOnStart(CurrentLevel);
		}
		else
		{
			for(CLuaVector::iterator it=LuaVector.begin(); it!=LuaVector.end();++it)
				if((*it)==O)
				{
					(*it)=NULL;
					O->luaOnStop(CurrentLevel);
					return;
				}
			Error2Console(LUA_SYSTEM,"Remove object: object not found");
		}
	}

	LuaObject::LuaObject(const int SL)
	{
		SetStartLevel(SL);
		Started=false;
		AddRemoveObject(this,true);
	}

	LuaObject::~LuaObject()
	{
		AddRemoveObject(this,false);
	}

	void LuaObject::SetStartLevel(const int SL)
	{
		if(SL!=(int)StartLevel)
		{
			Started=false;
			StartLevel=SL;
		}
	}

	void LuaObject::DoExport()
	{
		Started=true;
	};

	void LuaObject::luaOnStart      (const DWORD Level)
	{
	 if(Level==StartLevel)
	   if(!Started)
		  if(!LuaO_Delay)
			  DoExport();
	}

	void LuaObject::luaOnStop       (const DWORD Level){}

	void LuaObject::luaOnModuleStart(const DWORD Level)
	{
	}

	void LuaObject::luaOnModuleStop (const DWORD Level){}

    void LuaObject::InitModules()
    {
		AddRemoveObject(NULL,2);
    }
    void LuaObject::SetInitDelay(const bool Delay)
    {
		if(!Delay) InitModules();
		LuaO_Delay=Delay;
    }

	void   setfloat(LPCSTR Name,const double val)
	{	 set(Name,val);}

	void   setVfloat(LPCSTR Name,const double val,...)
	{	 set(Name,val); }

	void   setstring(LPCSTR Name,LPCSTR val)
	{
		set(Name,val);
	}

	void   setVstring(LPCSTR Name,LPCSTR val,...)
	{
		DOFORMAT(Name2,val);
		set(Name,Name2.c_str());
	}


	void   setdword(LPCSTR Name,const DWORD val)
	{	set(Name,val);	}

	void   setVdword(LPCSTR Name,const DWORD val,...)
	{	set(Name,val);	}


#define getformat	DOFORMAT(Name,_Name);   

	double getfloat (LPCSTR Name   )	{ return gets<float> (Name,0);}
	LPCSTR getstring(LPCSTR Name    )	{ return gets<LPCSTR>(Name,0);}
	DWORD  getdword (LPCSTR Name    )	{ return gets<DWORD> (Name,0);}

	double getVfloat (LPCSTR _Name,...)	{getformat; return gets<float> (Name,0);}
	LPCSTR getVstring(LPCSTR _Name,...)	{getformat; return gets<LPCSTR>(Name,0);}
	DWORD  getVdword (LPCSTR _Name,...)	{getformat; return gets<DWORD> (Name,0);}


    int    dofunction        (LPCSTR namefunction, LPCSTR value)
    {
      if(NOTTABLE(namefunction))
          {
            doVstring("tmpf=%s(\"%s\")",namefunction,value);
            return gets<int>("tmpf",0);
          }
      else
          {
      lua::functor<int> foo(namefunction);
      if(foo.is_valid()) return (int)foo(value);
	  else               return 0;
          }
    }

	int  callintfunction   (LPCSTR namefunction, int value)
    {
      if(NOTTABLE(namefunction))
          {
            doVstring("tmpf=%s(%d)",namefunction,value);
            return gets<int>("tmpf",0);
          }
      else
          {
      lua::functor<int> foo(namefunction);
      if(foo.is_valid()) return (int)foo(value);
	  else               return 0;
          }
    }
	LPCSTR  callstrfunction(LPCSTR namefunction, LPCSTR value)
    {
      if(NOTTABLE(namefunction))
          {
            doVstring("tmpf=%s(\"%s\")",namefunction,value);
            return gets<LPCSTR>("tmpf",NULL);
          }
      else
          {
      lua::functor<LPCSTR> foo(namefunction);
      if(foo.is_valid()) return (LPCSTR)foo(value);
      else               return 0;
          }
    }
    void  callfunction     (LPCSTR namefunction)
    {
      if(NOTTABLE(namefunction))
          {
            doVstring("%s()",namefunction);
          }
      else
          {
           lua::functor<void> foo(namefunction);
           if(foo.is_valid()) (void)foo();
          }
    }

	


	typedef SmartVector<CLuaVar> LuaVarVector;
	LuaVarVector *CVarTmp;
	GString slfStartName;
	void scanluafolderlb(LPCSTR Name)
	{
		CLuaVar	V;
		strncpy_s(V.Name,sizeof(V.Name),Name,sizeof(V.Name));
		//V.Type=lua::type(slfStartName+Name);
		CVarTmp->push_back(V);
	}



	PLuaVar ScanLuaFolder(LPCSTR Name,int *len)
	{
		static LuaVarVector VarVector;
		VarVector.clear();
		CVarTmp=&VarVector;
		slfStartName=Name;slfStartName+=".";
		char Buf[2];Buf[0]=0;
		scanbegin(Buf,false);
		scancallback(scanluafolderlb);
		doVstring("for i,v in pairs(%s) do scanret(i);end;",Name);
		scancallback(NULL);
		if(len)*len=VarVector.size();
		return VarVector.begin();
	}

	void   createLPCSTRfunction(LPCSTR Name,luaLPCSTRF f)
	{
		 lua::function_def(Name  ,f);
	}

    void luafunction(LPCSTR Name,lua_CFunction fn,LPCSTR MoveTo)
    {
      	lua_pushcfunction(lua::GetState(), fn);
        lua_setglobal(lua::GetState(), Name);
        if(MoveTo)
            lua::doVstring("%s.%s=%s;%s=nil;",MoveTo,Name,Name,Name);

        /**
		if(LUA_SYSTEM)
            if(LUA_TRACEMODE & LUA_TRACE_FUNCTIONDEF)
              if(MoveTo)
                  Print2Console(LUA_SYSTEM,"+f %s:%s",MoveTo,Name);
              else
                  Print2Console(LUA_SYSTEM,"+f %s",Name);
		/**/
    }

GString _tostring(lua_State *L,const int Index){return lua_tostring(L,Index);}
double  _tonumber(lua_State *L,const int Index){return lua_tonumber(L,Index);}
void  pushstring(lua_State *L,LPCSTR S){lua_pushstring(L,S);}
void  pushnumber(lua_State *L,double I){lua_pushnumber(L,I);}


 object GetFinalObject(LPCSTR Name)
 {
   static char charbuf[0xFF*4];
   static int off[30];
   {
   char *C=charbuf;
   const char *N=Name;
   int i=1;
   int ii=-1;
   for(;*N;++N,++C,++i) if(*N=='.'){*C=0;off[++ii]=i;i=0;}else *C=*N;
   off[++ii]=0;
   *C=0;
   }
   char *C=charbuf;
   pobject Obj=GetVariables();
   object obj=(*Obj)[C];
   if(!off[0])return obj;
   C+=off[0];
   int ii=0;
   while(1)
   {
     if(obj.type() == LUA_TTABLE)
     {
       obj=obj[C];
       int of=off[++ii];
       if(!of)
       {
        //if(obj.type() == LUA_TTABLE)ASSERT(FALSE);
        return obj;
       }
       C+=of;
     }
     else 
     { 
      //ASSERT("deep in not a table" && false);
      return obj;
     }
   }
   return obj;

 }

LuaScanTree *WorkScanTree;


LUAFUNC(GrabprintQDE)
{
  GString S=lua::_tostring(L,1);
  GString Off=lua::_tostring(L,2);
  if(S=="_G") return 0;
  S=Off+S;
  WorkScanTree->push_back(S); 
  Off=S+".";
  lua::doVstring("for i,v in pairs(%s) do GrabprintQDE(i,\"%s\");end;",(LPCSTR)S,LPCSTR(Off));
  return 0;
}

void GrabLuaTree(LuaScanTree *ST)
{
  if(!ST) return;
  WorkScanTree=ST;
  WorkScanTree->clear();
  lua::luafunction("GrabprintQDE",GrabprintQDE);
  lua::object::iterator it_b = lua::GetVariables()->begin();
  lua::object::iterator it_e = lua::GetVariables()->end();
   for(;it_b!=it_e;++it_b)
   {
   		lua::object key = it_b.key();
        lua::proxy_object &po = *it_b;
     	LPCSTR v=lua::get<const char *>(key);
        if( po.type() == LUA_TTABLE)
            lua::doVstring("GrabprintQDE(\"%s\");",v,v);
		else WorkScanTree->push_back(v); 
   }
   lua::dostring("GrabprintQDE=nil;");
//  return 0;
}



};
