// BaseBot.cpp : Implementation of CAIApp and DLL registration.

#include "stdafx.h"
#include "BaseBot.h"
#include "PathSpace/PathSpace.h"

/////////////////////////////////////////////////////////////////////////////

using namespace AI;


//Base Bot

void  CBaseBot::WholeReset()
{
    DWORD Start=(DWORD)&TeamDef;
    DWORD End  =(DWORD)&__L1ParamsEnd-Start;
    ZeroMemory((PVOID)Start,End);
}

DWORD   CBaseBot::GetBotID   (){return TeamDef.MyID;}
DWORD   CBaseBot::GetTeam    (){return TeamDef.MyTeam;}
DWORD   CBaseBot::GetSquad   (){return TeamDef.MySquad;}
DWORD   CBaseBot::GetType    (){return TeamDef.MyType;}
DWORD   CBaseBot::GetStatus  (){return TeamDef.MyStatus;}

DWORD   CBaseBot::GetMyWeapon(int i){return Attack[i].CurrentWeapon;}
float   CBaseBot::GetHeal    (){return Heal.Current;}
float   CBaseBot::GetArmor   (){return Armor.Current;}

float   CBaseBot::Tick       ()
{
	if(MyStat.IsAMarker)
		PATH::UpdateMarkerInfo(MyStat.IsAMarker,MyStat.Position);
	return 0;
}


bool CBaseBot::IsMoventPossible()
{
	return Movent.MoventPossible;
}

bool CBaseBot::IsSilentMovePosible()
{
	return Movent.SilentMoventPossible;
}

bool CBaseBot::IsRotationPossible()
{
	return Movent.RotationPossible;
}

bVector3f BotDummpDir;

const bVector3f &CBaseBot::GetPosition         () // получить позицию
{  return MyStat.Position;
}
const bVector3f &CBaseBot::GetVectorOrientation() // вертор направления
{   return MyStat.Orientation;
}
const bVector3f &CBaseBot::GetAngleOrientation () // углы направления
{   return MyStat.Orientation; //_!_ ANGLES!!
}

const bVector3f &CBaseBot::GetCurrentSpeed     () // текушая скорость
{	return MyStat.Speed;
}

const bVector3f &CBaseBot::GetSizeBox          () // размеры
{	return MyStat.SizeBox;
}

DWORD CBaseBot::OnMessage(const CBlopMessage &Message)
{
	bVector3f D;
	switch(Message.Msg)
	{
 	 case bCMD_SYNC_POS:
		 D=*((const bVector3f*)Message.Data);
         MyStat.Position=D;
         break;
	}
	return 1;
}

void  CBaseBot::MakeMyMarker      (DWORD mID)
{
	if(mID!=-1)
	{
		MyStat.IsAMarker=mID;
	}
}

//**************************************************************
//**************************************************************
// операции над пространством

#define CBBFindMarker if(!PATH::FindMarker(Dir,Marker)) return false;	// найти маркет

DWORD CBaseBot::GetMoventToMarker         (bVector3f &Dir,const DWORD Marker) // получить вектор направляния движения до маркера
{
  CBBFindMarker
  //bVector3f MarkerPos;
  //GetWaypointPosition(Marker,MarkerPos);
  bVector3f NewSpeed;
  DWORD     Flags;
  PATH::TracePath(PL_MOVENT,GetSizeBox(),GetPosition(),Dir,GetCurrentSpeed(),GetVectorOrientation(),NewSpeed,Flags);
  Dir=NewSpeed;
  DWORD FLAGS;
  FLAGS=MOVE_OK;
  if(Dir.Length()<10)
      //PATH::MarkerSize(Marker))
      FLAGS|=MOVE_DONE;
  else FLAGS|=MOVE_WAIT;
  return FLAGS;
};
DWORD CBaseBot::GetDirrectionToMarker     (bVector3f &Dir,const DWORD Marker) // получить вектор направляния на маркер
{
  CBBFindMarker
  Dir-=GetPosition();
  DWORD FLAGS;
  FLAGS=MOVE_OK;
  if(Dir.Length()<2)
      //PATH::MarkerSize(Marker))
      FLAGS|=MOVE_DONE;
  else FLAGS|=MOVE_WAIT;
  return FLAGS;
}
DWORD CBaseBot::GetShotDirrectionToMarker (bVector3f &Dir,const DWORD Marker) // получить вектор направляния стрельбы на маркер
{
  CBBFindMarker
  Dir-=GetPosition();  //_!_ прямая направленость, нужно учесть AIM MODE
  return true;
}
DWORD CBaseBot::GetSilentMoventToMarker   (bVector3f &Dir,const DWORD Marker) // получить вектор направляния скрытного движения до маркера
{
  CBBFindMarker
  if(!IsSilentMovePosible()) return false;
  return false;
}



bool CBaseBot::MOVE_InDirrection          (const bVector3f &Dir,const WORD Force,const DWORD &Speed)					// бот двигается в указаную сторону
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED");return true;
}
bool CBaseBot::MOVE_ChangeMode            (const DWORD &Mode,const DWORD &Speed)					// смена режима хотьбы
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(CHANGEMODE)");return true;
}
bool CBaseBot::MOVE_DoJump                (const bVector3f &Dir,const DWORD &Speed)					// прыжок в направлении
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(JUMP)");return true;
}
bool CBaseBot::MOVE_ToPoint               (const bVector3f &To,const DWORD &Speed)
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(MOVETOP)");return true;
}

bool CBaseBot::ORI_LOOK                   (const bVector3f &Dir,const float &Speed)                 // посмотреть на
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(LOOK)");return true;
}
bool CBaseBot::ORI_ROTATE                 (const bVector3f &Dir,const float &Speed,const float Force)// повернуться на
{Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(ROTATE)");return true;
}
void CBaseBot::Fire                       (const DWORD Type,const int Rounds)
{
Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(FIRE)");
}

bool CBaseBot::Say                        (LPCSTR S)
{
Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(SAY)");
return true;
}

void CBaseBot::Exec                       (LPCSTR String)
{
	Error2Console(SYS_AI,"UNBINDED COMMAND CALLED(Exec)");
}

char ExecutorLinkNetCMDBUFF[512];

class ExecutorLink:public CBaseBot
{
    CBotExecutor *Link;
public:
    ExecutorLink(CBotExecutor *Exc,LPCSTR Name)
    {
        Link=Exc;
    }
    void Fire                       (const DWORD Type,const int Rounds)
    {
        if(Link)
        {
            CAINetCommand *CMD=(CAINetCommand *)ExecutorLinkNetCMDBUFF;
            CMD->SetCommand(BOT_CMD_FIRE);
            CMD->PushDWORD(Type);
            Link->ExecCMD(CMD);
        }
    }

    bool MOVE_InDirrection          (const bVector3f &Dir,const WORD Force,const DWORD &Speed)
    {
        if(Link)
        {
            CAINetCommand *CMD=(CAINetCommand *)ExecutorLinkNetCMDBUFF;
            CMD->SetCommand(BOT_CMD_MOVE_DIR);
            if(Force)
            {
                CMD->PushBYTE(1);
                CMD->PushBYTE(Force);
                if(Speed==0)
                CMD->PushBYTE(1);
				//CMD->PushBYTE(0);
            }
            else
            {
                CMD->PushBYTE(0);
                CMD->PushVector(Dir.cfloat());
                CMD->PushBYTE(Speed);
				if(Speed<0xFFFFFF)
				  CMD->PushBYTE(0);
				else
                  CMD->PushBYTE(1);
            }
            return Link->ExecCMD(CMD)!=0;
        }
        return true;
    }

	bool ORI_LOOK                   (const bVector3f &Dir,const float &Speed) 
	{
		if(Link)
        {
          CAINetCommand *CMD=(CAINetCommand *)ExecutorLinkNetCMDBUFF;
          CMD->SetCommand(BOT_CMD_LOOK_TO);
		  CMD->PushVector(Dir.cfloat());
		  CMD->PushFLOAT (Speed);
		  return Link->ExecCMD(CMD)>0;
		}
        return true;
	}

	virtual bool Say                        (LPCSTR S)
	{
	  if(Link)
	  {
		  CAINetCommand *CMD=(CAINetCommand *)ExecutorLinkNetCMDBUFF;
          CMD->SetCommand(BOT_CMD_SAY);
		  CMD->PushString(S);
		  return Link->ExecCMD(CMD)>0;
	  }
	  return false;
	}

	void Exec                       (LPCSTR String)
    {
	  if(Link)
	  {
		  CAINetCommand *CMD=(CAINetCommand *)ExecutorLinkNetCMDBUFF;
          CMD->SetCommand(BOT_CMD_EXEC_CONSOLE);
		  CMD->PushString(String);
		  Link->ExecCMD(CMD);
	  }
	  //return false;
    }
};

typedef SmartVector<CBaseBot*> _BotPack;
typedef SmartVector<CBotExecutor*> _CreaturePack;
_AICMDBSP  BotNameBSP;
_AICMDBSP  CreatureNameBSP;
_BotPack   Bots;
_CreaturePack Creatures;

DWORD AI::RegisterBot(CBaseBot *Bot,LPCSTR ID)
{
    Bots.push_back(Bot);
	if(ID)
	{
	 GString T=ID;
	 T.MakeLower();
	 BotNameBSP.PushAs(Bots.size(),T.c_str(),T.length());
	}
    return Bots.size();
}

CBaseBot* AI::GetBotByName(LPCSTR BotName)
{
	GString T=BotName;
	T.MakeLower();
	DWORD ID=BotNameBSP.GetIDFor(T.c_str(),T.length());
	if(ID)
	{
		return Bots[ID-1];
	}
	return NULL;
}

DWORD   AI::RegisterCreatureT(CBotExecutor *Exc,LPCSTR Name)
{
    Creatures.push_back(Exc);
	if(Name)
	{
	 GString T=Name;
	 T.MakeLower();
	 CreatureNameBSP.PushAs(Creatures.size(),T.c_str(),T.length());
     RegisterBot(new ExecutorLink(Exc,Name),Name);
	}
    return Creatures.size();
}

CBotExecutor   *AI::GetCreatureByNameT(LPCSTR Name)
{
	GString T=Name;
	T.MakeLower();
	DWORD ID=CreatureNameBSP.GetIDFor(T.c_str(),T.length());
	if(ID)
	{
		return Creatures[ID-1];
	}
	return NULL;
}


class CVirtual:public CBaseBot
{
public:
	CObjectID TT;
	CVirtual(LPCSTR BaseName)
	{
		BlopBaseObject* Obj=FindObjectByName(BaseName);
		if(Obj)
			TT=Obj->GetID();
		else
			TT=GetID();
	}
	virtual bool MOVE_InDirrection          (const bVector3f &Dir,const DWORD &Speed);					// бот двигается в указаную сторону
	virtual bool MOVE_ChangeMode            (const DWORD &Mode,const DWORD &Speed);					// смена режима хотьбы
	virtual bool MOVE_DoJump                (const bVector3f &Dir,const DWORD &Speed);					// прыжок в направлении

	virtual bool ORI_LOOK                   (const bVector3f &Dir,const DWORD &Speed);                 // посмотреть на
	virtual bool ORI_ROTATE                 (const bVector3f &Dir,const DWORD &Speed,const bool Force);// повернуться на
	//virtual bool Say                        (LPCSTR S);
};


bool CVirtual::MOVE_InDirrection          (const bVector3f &Dir,const DWORD &Speed)					// бот двигается в указаную сторону
{
     bVector3f Pos=Dir;
     float Len=Pos.Length();
     Pos.Normalize();
     Pos*=Speed/10.0f;
	 SendUpSignal(CObjectIDTwin(TT,GetID()),bCMD_MOVE,4*3,(LPVOID)&Pos);
	 return true;
}
bool CVirtual::MOVE_ChangeMode            (const DWORD &Mode,const DWORD &Speed)					// смена режима хотьбы
{return false;
}
bool CVirtual::MOVE_DoJump                (const bVector3f &Dir,const DWORD &Speed)					// прыжок в направлении
{return false;
}

bool CVirtual::ORI_LOOK                   (const bVector3f &Dir,const DWORD &Speed)                 // посмотреть на
{return false;
}
bool CVirtual::ORI_ROTATE                 (const bVector3f &Dir,const DWORD &Speed,const bool Force)// повернуться на
{return false;
}


CVirtual *Bot,*Bot2,*Bot3;
CBotScript *P,*T1,*T2,*TALL;

#include "BotMessages.h"

DWORD         AI::tRegisterWayPoint(DWORD ID,LPCSTR Name,const float* Origin,LPCSTR Target)
{
    bVector3f O=Origin;
    return PATH::AddMarker(Name,O);
}

DWORD AI::SYS_AI=0;

void DLLTYPE AI::CreateAI()
{
    if(AI::SYS_AI==0)
	{

        RegisterNewConsoleClass(SYS_AI,"AI",-1);
	}
	Print2Console(SYS_AI,"Starting AI module...");
    AI::RegisterCallBack_regExt (RegisterCreatureT);
    AI::RegisterCallBack_regWayp(tRegisterWayPoint);
	AI::RegisterLoadAIScript    (LoadAIScriptFileT);
	AI::RegisterRunScript       (RunScriptT);
	AI::RegisterParseScript		(ParseAIScriptT);
	AI::RegisterUpdateMarker	(fUpdateMarker);
    LoadBaseCMDS();
  /*
	Bot =new CVirtual("Box1");Bot ->WholeReset();RegisterBot(Bot ,"Bot1");
    Bot2=new CVirtual("Box2");Bot2->WholeReset();RegisterBot(Bot2,"Bot2");
    Bot3=new CVirtual("Box3");Bot3->WholeReset();RegisterBot(Bot3,"Bot3");
  */
	//LoadAIScriptFile("System/AI/common.AI");
	//RunScript("__START__");

};

DWORD DLLTYPE   AI::fUpdateMarker(LPCSTR Name,const bVector3f &Pos)
{
	return PATH::AddMarker(Name,Pos);
}

void DLLTYPE AI::RetouchScripts(LPCSTR Drive)
{
    //LoadAIScriptFile("common.AI");
    if(Drive)
    {
        if(strcmp(Drive,"STOP")==0)
            CBotScript::StopAll();
    }
    else
    RunScriptT("__START__");
}

void DLLTYPE AI::RetouchScript (LPCSTR Drive)
{
    //LoadAIScriptFile("common.AI");
   // LoadAIScriptFileT("System/AI/common.AI");
}

void DLLTYPE TickAI()
{
	CBotScript::RunActive();
}




