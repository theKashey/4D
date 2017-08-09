/*===========================================================================
    Заголовок: BlopClient.cpp
    Автор: Kashey
    Информация: Клиентская часть джвика

===========================================================================*/
//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProjectSpace.h"
#include "ProjectCounter.h"
/*#include "../NetUtil/Transfer.h"
#include "../Containers/AccauntBase.h"
#include "../Chilli/Chilli.h"
#include "../NetUtil/ConnectManager.h"
#include "Core/Sound/SoundMe.h"
*/

#include "BlopClient.h"

//#include "../NetUtil/NetUtils.h"

//#include "../System/Window/GenericWindow.h"

#include "../System/Window/Win32Window.h"
#include "../Graphics/OpenGL/grOpenGL.h"

#include "../Parser/ParserPlugIn.h"


DWORD SYS_ENGINE;

static bool bc__Started=false;


//---------------------------------------------------------------------------

CObjectID rootID;
const CObjectID& CORE::GetRootID(){ return rootID;}

CBlopClient *MyClient=NULL;

bool modulesStarted=false;

bool  CBlopClient::AddInModule(LPCSTR _Name)
{
    GString Name;
    int i=0;
    int LN=Istrlen(_Name);
    for(i=0;i<LN;++i){if(_Name[i]==':') break;Name+=_Name[i];}
    GString Ext=&_Name[i+1];
#ifdef _DEBUG
    Name="Modules\\"+Name;
#else
    Name="RelModules\\"+Name;
#endif
    cobjectloadfunc loadfunc;
	if(Name=="@")
	loadfunc=(cobjectloadfunc)(LoadPVoidDLLFunc(NULL,Ext)); 
    else
    {
      loadfunc=(cobjectloadfunc)(LoadPVoidDLLFunc(Name,"GetStoredClasses"));
      if(!loadfunc)
       loadfunc=(cobjectloadfunc)(LoadPVoidDLLFunc(Name,"?GetStoredClasses@@YAPAUCLoadObjectList@@XZ"));          
    }
	if(Ext=="T")
	{
        Print2Console(SYS_ENGINE,"#%s touched",Name.c_str());
		return true;
	}
    if(!loadfunc)
    {
      Error2Console(SYS_ENGINE,"#%s - module not found",_Name);
      return false;
    }
    CLoadObjectList *OL=loadfunc();
    if(!OL)
    {
     Error2Console(SYS_ENGINE,"%s - module have no export data",_Name);
     return true;
    }
    Print2Console(SYS_ENGINE,">--> %s",_Name);
    if(OL->SkipExport()) return true;// allready export
    if(LN==i || Ext=="ALL") // entry point start
    {

      for(int i=0;i<OL->GetNumObjects();++i)
	  {
		  try{
          AddIn(OL->GetObject(i),OL->GetObjectStartUp(i),OL->IsAutoStart(i),(this,NULL),OL->GetName(i));
              }catch(...){Error2Console(SYS_ENGINE,"#%s raises exception",_Name);}
	  }
      return true;
    }
    else
    {
      for(int i=0;i<OL->GetNumObjects();++i)
       if(Ext==OL->GetObjectName(i))
       {
		  try{
          AddIn(OL->GetObject(i),OL->GetObjectStartUp(i),OL->IsAutoStart(i),(this,NULL),OL->GetName(i));
		  }catch(...){Error2Console(SYS_ENGINE,"%s raises exception",_Name);}
          return true;
          break;
       }
    }
    return false;
}
void  CBlopClient::AddInModulePack(LPCSTR Name)
{
	if(!Name)return;
    while(*Name)
    {
        AddInModule(Name);
        Name+=Istrlen(Name)+1;
    }
}

//#include "../parser/ParserPlugIn.h"

ConsoleUtil	*ConsoleOut;

static bool GLOBALSTOP=false;

DWORD GetCoreState(DWORD VAL)
{
  switch(VAL)
  {
      case CS_GLOBALSTOP:return GLOBALSTOP;
  };
  return -1;
}



CBlopClient::CBlopClient(BlopStartInformation &BSI):BaseObject(REFC_CORE,REFB_CORE,"CORE")
{
 GLOBALSTOP=false;
 ConsoleOut       =new ConsoleUtil(BSI.Log,"CLN");
 SetDefaultConsole(ConsoleOut);

 ConsoleOut->Printf("Blop Core%s at %s staring..",
#if _DEBUG
  "[Debug]"
#else
  ""
#endif
  ,__DATE__);

 InitWorld    ();
 InitLuaSystem();
 InitFileSystem("");

 lua::dofile("startup.lua"); //start

 _TimeSwitch      =true;
 MyClient         =this;
 rootID           =GetID();
// ConKeeper        =NULL;
 RegisterNewConsoleClass(SYS_ENGINE,"ENGINE",-1);
 ConsoleOut->Printf("Console started");

 BlopUtilsPack BUP;
 BUP.ConsoleOut=ConsoleOut;

 ConsoleOut->Printf("Loading modules");

 Window=NULL;
 Graphics=NULL;
 if(BSI.Modules.Window==(LPCSTR)1)AddIn(Window   = new CWin32Window(BSI.Window,this),1,true,(this,NULL));
 if(BSI.Modules.Render==(LPCSTR)1)AddIn(Graphics = new COpenGL(this),2,true,(Window,NULL));
 else
 if(BSI.Modules.Render==(LPCSTR)0)AddIn(Graphics = new CGenericGraphic(this),2,true,(NULL));
 else
  AddInModule(BSI.Modules.Render);
 
 AddIn(Input=new CInput(),2,true,(NULL));

 AddInModulePack(BSI.Modules.AddModules);

 //StartParser..
 GetParserStoredClasses()->GetObject(0)->Start();

 ConsoleOut->Printf("Stating modules");
 if(Window) Window->Input=Input;
}

void luaAddModule(LPCSTR Name){MyClient->AddInModule(Name);};
void luaKill(LPCSTR Name){MyClient->Termination=true;bc__Started=false;delete MyClient;}

void CBlopClient::InitLuaSystem()
{
  ConsoleOut->Printf("starting lua");
  lua::init();
  lua::createLPCSTRfunction("core_AddModule",luaAddModule);
  lua::createLPCSTRfunction("core_Kill",luaKill);

  lua::dostring("core={};core.graphics={};core.graphics.models={};core.graphics.camera={};");
  lua::dostring("utils={};utils.model={};utils.convert={};utils.export={}");

  lua::LuaObject::InitModules();
  lua::LuaObject::SetInitDelay(true);
  lua::LuaObject::EnterLevel(0);
  lua::LuaObject::EnterLevel(1);
  lua::LuaObject::EnterLevel(2);

  lua::setdword("win_width",800);
  lua::setdword("win_height",600);
  lua::setdword("win_dc",0);  
  lua::setdword("win_fullscreen",0);
  lua::setdword("win_BPP",32);
  lua::setdword("win_DPP",16);
  lua::setdword("win_updateviewport",1);
  lua::setdword("win_fov",90);
  lua::setfloat("win_znear",1);
  lua::setfloat("win_zfar",-1);

  lua::LuaObject::EnterLevel(3);  
  lua::LuaObject::EnterLevel(4);  
  lua::LuaObject::SetInitDelay(false);
}


extern bool gMEM2TCollapsing;
CBlopClient::~CBlopClient()
{
 bc__Started=false;
 SetDefaultConsole(NULL);
 gMEM2TCollapsing=true;
 ConsoleOut->Printf("Client finilization");
 //delete Input;
 for(VObjects::iterator it=Objects.begin(); it!=Objects.end()-1;++it)
 {
	 try
	 {
	   it->Obj->Stop();
	   delete (it);
	 }
	 catch(...)
	 {
	 }
 }
 Textures::LoadTexture("0SHUTDOWN");
 delete ConsoleOut;
 Objects.DropData();
 GLOBALSTOP=true;
// exit(-1);
}

void  CBlopClient::AddIn(BlopBaseObject* Object,int StartUpOrder,bool AutoStart,BlopBaseObject** DefOn,LPCSTR ExName)
{
	Objects.push_ref(Object);
	if(ExName==NULL)ExName=Object->GetFormatName();
	if(modulesStarted)
		if(!Object->StartObject())
		{
            SleepEx(0,true);
			ConsoleOut->Printf("#unload Unable to run object %s",(Object)->GetFormatName());
			return;
		}
		else
		 ConsoleOut->Printf("#start module %s|%s",ExName,Object->GetFormatName());
	else
	 ConsoleOut->Printf("#add module %s|%s",ExName,Object->GetFormatName());
	//CStartUpOrder StartUp(StartUpOrder,AutoStart,DefOn);
	//StartUp.pushBack (StartUp);
}

DWORD CBlopClient::Connect(LPCSTR ServerName,WORD ServerPort)
{
 /*Transfer.CreateSocket(BLOP_TRANSFER_TPC,0,_ip_from_name(ServerName),ServerPort,this);
 if(ConKeeper) delete ConKeeper;
 ConKeeper = new CConnectKeeper(Transfer,this);
 SessionLoginInformation SL;
 ConKeeper->Login("kashey","worlder",SL);*/
 return 0;
}

bool  CBlopClient::Start()
{
    Init();
	while(TimeTick());
    ConsoleOut->Printf("-------------END GAME----------------");
	return true;
}



bool CBlopClient::Init()
{
    lua::LuaObject::EnterLevel(5);
	Termination=false;
	modulesStarted=true;
	for(VObjects::iterator it=Objects.begin(); it!=Objects.end();++it)
	{
	 ConsoleOut->Printf("#start module %s",(*it).Obj->GetFormatName());
     bool ret=false;
     try
     {
	  ret=(*it).Obj->StartObject();
     }catch(...){ret =false;}
     if(!ret)
		{
            SleepEx(0,true);
            ConsoleOut->Printf("#unload Unable to run object %s",(*it).Obj->GetFormatName());
			MessageBox(0, StrFormat("Unable to run object %s",(*it).Obj->GetFormatName()), "Error", MB_OK | MB_ICONERROR);
			return false;
		}
	 SleepEx(0,true);
	}
    ConsoleOut->Printf("Init OK");
    ConsoleOut->Printf("Preloading...");
    lua::LuaObject::SetInitDelay(true);
    lua::LuaObject::EnterLevel(6);  
    lua::LuaObject::EnterLevel(7);  
    lua::LuaObject::SetInitDelay(false);
    lua::dofile("poststart.lua");
	PrepareWorld(); 
    
    ConsoleOut->Printf("InitCycles");
    InitCycles(); 

    VARS::Init();

    ConsoleOut->Printf("-------------START GAME--------------");
    bc__Started=true;
    return true;
}

bool CBlopClient::PrepareWorld()
{
 	for(VObjects::iterator it=Objects.begin(); it!=Objects.end();++it)
	{
	 if(!(*it).Obj->Prepare())
		{
			MessageBox(0, StrFormat("Unable to prepare object %s",(*it).Obj->GetFormatName()), "Error", MB_OK | MB_ICONERROR);
		}
		SleepEx(0,true);
	}
     return true;
}

bool CBlopClient::TimeTick()
{
     if(!bc__Started)return true;
	 if(Termination)return true;
     bool FullPro=lua::getdword("profile_fulltime")!=0;
     if(FullPro) EndProfile();
     StartProfile();
	 {
      static MakeProfile(MainCyclePro,"Main")
	  Profile(MainCyclePro);
	  Time::SetThisTime();
	  DetectCycle(DC_MAIN_CYCLE);
	  ProvidePost();
      MemList::Swap();
	  {
	    static MakeProfile(MainTickCycle,"Tick");
		Profile(MainTickCycle);
		for(VObjects::iterator it=Objects.begin(); it!=Objects.end();++it)
		{
			static MakeProfile(DefProfile,"unknown module");
			_Profilers::CProfiler *Profile=&DefProfile;
			if((*it).Profile)Profile=(*it).Profile;
			{
		      //Profile(*Profile);
              Profile->Enter();
			  (*it).Obj->Tick(1);	
              Profile->Leave();
			}
		}
	  }
      double FTIME=GetCounter(DC_MAIN_CYCLE);
      double FPS=1.0/FTIME;
	  lua::setfloat("fps",FPS);
      VARS::fvars(VARS::fv_FPS       )=FPS;
      VARS::fvars(VARS::fv_FSPAN_d_20)=FPS*0.05; 
      VARS::fvars(VARS::fv_FTIME_d_20)=FTIME*20;  	  
      VARS::fvars(VARS::fv_FRAMETIME )=FTIME;
	  SleepEx(0,true);
	 }
     if(!FullPro) EndProfile();
     return !Termination;
}

void CBlopClient::ProvidePost()
{
	static MakeProfile(MainCyclePPost,"MSG_POST")
	Profile(MainCyclePPost);
	WORD Len;
	CBlopMessage* Messages = GetUpSingnal(Len);
	if(!Len) return;
	else
    for(WORD x=0;x<Len;++x,++Messages)
	{
		BlopBaseObject *To=REF_FindObjectByID(Messages->GetAddress().To);
		if(To)
        {
			To->OnMessage(*Messages);
            Messages->Free();
        }
		else
		if(Messages->GetAddress().To==GetID())
			OnMessage(*Messages);        
	}
}

DWORD CBlopClient::OnMessage(const CBlopMessage &Message)
{
	switch(Message.GetMsg())
	{
	  case BMSG::CMD_TERMINATE: Termination=true; return 0;
	}
	return BMSG::E_NOT_ACCEPTED; 
}


DWORD CBlopClient::TimeSwitch()
{
	_TimeSwitch=!_TimeSwitch;
    
	return _TimeSwitch;
}


/**
232 17 33
232 17 34
орто тератология
сокольники стромынка д 2
пожпрная колонча
из семейный доктор


/**/