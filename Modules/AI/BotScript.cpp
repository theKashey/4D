// BotScript.cpp: implementation of the BotScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Parser/Parser.h"
#include "BaseBot.h"

int MAX_COMMAND_PER_TICK=10;

/////////////////////////////////////////////////////////////////////////////

using namespace AI;

//Command

_AICMDBSP     CBotScript::AICMDBSP;
_AICMDBSP     CBotScript::TRIGCMDBSP;
_AICMDBSP     CBotScript::TRIGPACKBSP;
CBotScript::CScriptList   CBotScript::ScriptList;

void  CBotScript::SetName    (LPCSTR Name)
{
	ScriptName=Name;
}

void  CBotScript::MakeTrigger(LPCSTR Name)
{
 static GString Base;
 #define ADDBSP(b,a) Base=a;Base.MakeLower();CBotScript::TRIGPACKBSP.PushAs(b,Base.v_str(),(DWORD)strlen(a))
 GString Token=Name;
 Token.MakeLower();
 DWORD IDToken=TRIGPACKBSP.GetIDFor(Token.c_str(),(DWORD)Token.length());
 if(!IDToken)
 {
   ScriptList.push_back(this);
   ADDBSP(ScriptList.size(),Name);
 }
 else
 {
	 CBotScript* Src=ScriptList[IDToken-1];
	 if(Src!=this)
	 {
	   if(Src) delete Src;
       ScriptList[IDToken-1]=this;
	 }
 }
#undef ADDBSP
}

bool CBotScript::Activate(CBaseBot* B)
{
	Reset();
    if(B) SetMyBot(B);
    if(!Enabled) Print2Console(SYS_AI,"%s activated",ScriptName.c_str());
    else         Print2Console(SYS_AI,"%s reset activated",ScriptName.c_str());
	Enabled=true;
	return Enabled;
}

bool CBotScript::Deactivate()
{
    Print2Console(SYS_AI,"%s stoped",ScriptName.c_str());
	Enabled=false;
	return Enabled;
}

void CBotScript::RunActive()
{
    try
    {
	for(int i=0;i<ScriptList.size();++i)
		if(ScriptList[i] && ScriptList[i]->Enabled)
			ScriptList[i]->Execute(ScriptList[i]->ValidBot);
    }
    catch(...)
    {
        Print2Console(SYS_AI,"RUNACTIVE::ERROR OCCURED");
    }
}


void CBotScript::StopAll()
{
	for(int i=0;i<ScriptList.size();++i)
		if(ScriptList[i] && ScriptList[i]->Enabled)
        {
            ScriptList[i]->Reset();
            ScriptList[i]->Deactivate();
        }
}

CBotScript* CBotScript::GetTriggerByName(LPCSTR Name)
{
    GString Token=Name;
	Token.MakeLower();
	DWORD IDToken=TRIGPACKBSP.GetIDFor(Token.c_str(),(DWORD)Token.length());
	if(!IDToken)return NULL;
	return ScriptList[IDToken-1];

}

bool CBotScript::inAddCMD(DWORD ID,DWORD Len,LPCVOID Data)
{
 CBotCMD *CMD=(CBotCMD*)malloc(sizeof(CBotCMD)+Len);
 CMD->CMD =ID;
 CMD->Len =Len;
 memcpy(CMD->Data,Data,Len);
 CMD->Wait=false;
 AddCommand(CMD);
 return true;
}

void CBotScript::AddCommand(CBotCMD *CMD)
{
#ifdef AI_SCRIPT_DEBUG
		CheckCMD(CMD);
#endif
	Commands.push_back(CMD);
}
void CBotScript::Execute   (CBaseBot *Bot)
{
 int i=0;
 CommandList::iterator end=Commands.end();
 CommandList::iterator it =Commands.begin()+CurrentCommand;
 if(it<end)
 for(;it<end,i<MAX_COMMAND_PER_TICK;)
 {
	 if(!Bot && ValidBot) Bot=ValidBot;
     CBotCMD *CMD=*it;
     i++;
     if(!CMD) break;
     if(CMD->Wait && IsCommandComplette(CMD->Wait)) break;
	 LastError="";
	 BotCMDPacked MOUT(CMD,Bot);
     int ok=0;
	 switch(MOUT.Class)
     {
      case BOT_CMDCLASS_MOVE  :ok=DoMove  (MOUT);break;
      case BOT_CMDCLASS_ATTACK:ok=DoAttack(MOUT);break;
      case BOT_CMDCLASS_ACTION:ok=DoAction(MOUT);break;
      case BOT_CMDCLASS_LUA   :ok=DoLua   (MOUT);break;
	  case BOT_CMDCLASS_SCRIPT_CTRL:ok=DoScript(MOUT);break;
     }
     if(!ok) break;//
     if(CMD->Wait && IsCommandComplette(CMD->Wait)) break;
     if(ok>0)
     {
      CurrentCommand+=ok;
	  it+=ok;
     }
     else
     {
      CurrentCommand+=(ok+1);
	  it+=(ok+1);
      break;
     }
     
 }
 if(CurrentCommand>=(DWORD)Commands.size() || it>=end)
 {
	 Reset();
	 Deactivate();
 }
}

void   CBotScript::Reset()
{
    CurrentCommand=0;
}

LPCSTR CBotScript::TryTranslateCMD(DWORD CMD,bool DoError)
{
	return NULL;
}

bool CBotScript::IsCommandComplette(const DWORD ID)
{
	return true;
}

void CBotScript::AI_ERROR(const CBaseBot *Bot)
{
}

#include "BotMessages.h"

#define MAKESTRUCT(a,b) BOTCMD::a *b;b=(BOTCMD::a*)In.InData->Data;
#define MAKECOM #include "BotCMDs/MakeCom.h"

#include "BotCMDs/CMD_Move.h"

int CBotScript::DoScript(const BotCMDPacked &In)
{
	//#define com
	CBaseBot *BotOne;
	switch(In.CMD)
	{
	case BOT_CMD_ALLERT_TRIG:
		 MAKESTRUCT(CMD_ALLERT_TRIG,c1)
         #undef com
         #define com c1
		 if(com->GetTrigger())
			 ((CBotScript*)com->GetTrigger())->Activate(ValidBot);
		 return 1;
    case BOT_CMD_WAIT_TRIG:
		 MAKESTRUCT(CMD_ALLERT_TRIG,c2)
         #undef com
         #define com c2
		 if(com->GetTrigger()) if(((CBotScript*)com->GetTrigger())->Enabled)return -1;
	     return 1;
	case BOT_CMD_USEBOT:
		 MAKESTRUCT(CMD_USEBOT,c3)
         #undef com
         #define com c3
         BotOne=GetBotByName(com->GetName());
		 if(BotOne){ SetMyBot(BotOne);return 1;}
		 else if(com->GetWait())return -1;
		 else
		 return 1;
	case BOT_CMD_EXEC_CONSOLE:
		{
		 MAKESTRUCT(CMD_EXEC_CONSOLE,c10)
         #undef com
         #define com c10
		 if(In.Bot)
			 In.Bot->Exec(com->GetText());
		 return 1;
		}
	case BOT_CMD_WAIT:
		 MAKESTRUCT(CMD_WAIT,c4)
		 #undef com
         #define com c4
		 if(!TimerStartedAt)
			 TimerStartedAt=Time::Time();
		 if(Time::Time()-TimerStartedAt<com->GetTime()) return com->GetReturn();
		 else
		 {
			 TimerStartedAt=0;
			 return 1;
		 }
	}
	LastError.Format("undefined inscript command %d",In.Message);
	return 0;
}

int CBotScript::DoAttack(const BotCMDPacked &In)
{
	switch(In.CMD)
	{
	case BOT_CMD_FIRE:
         MAKESTRUCT(CMD_FIRE,c1)
		 #undef com
         #define com c1
		 if(In.Bot)
         In.Bot->Fire(c1->GetType());
		 return 1;

	}
	LastError.Format("undefined attack command %d",In.Message);
	return 0;
}
int CBotScript::DoAction(const BotCMDPacked &In)
{
	switch(In.CMD)
	{
	case BOT_CMD_SAY:
		{
		  MAKESTRUCT(CMD_SAY,c1)
		  #undef com
          #define com c1
			  if(In.Bot)In.Bot->Say(c1->GetText());
		}
		return 1;
	}
	LastError.Format("undefined action command %d",In.Message);
	return 0;
}
int CBotScript::DoLua   (const BotCMDPacked &In)
{
	//switch(In.CMD)
	{
	}
	LastError.Format("undefined LUA command %d",In.Message);
	return 0;
}

#undef MAKESTRUCT

void    CBotScript::AISHADERTOKENERROR(LPCSTR err,...)
{
    Print2Console(SYS_AI,"Token %s - %s",CurrentToken.c_str(),err);
}

DWORD CBotScript::Line(LPCSTR Line)
{
    DWORD     dwVal;
    bVector3f v3Val;
	bVector3f v3ValZERO;
	CBaseBot *B1,*BotOne;

    CParser Parser;

    Parser.StartParseBuffer(Line,Istrlen(Line));
    Parser.GetToken(false);
    GString Token2,Token3;
    GString Token=Parser.token;
	Token.MakeLower();
	DWORD IDToken=AICMDBSP.GetIDFor(Token.c_str(),(DWORD)Token.length());

    BOTCMD::CMD_MOVE_TO      c1(0,10);
    BOTCMD::CMD_MOVE_DIR     c2;
    BOTCMD::CMD_LOOK_TO      l1;
    BOTCMD::CMD_LOOK_AT      l2;
	BOTCMD::CMD_ALLERT_TRIG  ctrig1;
	BOTCMD::CMD_WAIT_TRIG    ctrig2;
	BOTCMD::CMD_CALL_TRIG    ctrig3;
	BOTCMD::CMD_WAIT         cwait;
	BOTCMD::CMD_USEBOT       cusebot;
	BOTCMD::CMD_FIRE         cfire;
    CurrentToken=Token;
    switch(IDToken)
    {
    case BOT_CMD_MOVE_TO         : 
        Parser.GetToken(false);
        Token2=Parser.token;
		Token2.MakeLower();
        dwVal=PATH::GetMarkerByName(Token2);
		{
		 DWORD Speed=10;
		 if(Parser.GetToken(false))
		 {
			 if(strcmp(Parser.token,"tele")==0) Speed=0xFFFFFF;
			 else
			 Speed=atoi(Parser.token);
		 }
        c1.SetWayPoint(dwVal,Speed);		 
		}
		if(Parser.GetToken(false))
			c1.SetWayPoint(dwVal,atoi(Parser.token));
        AddCMD(c1);
        return 1;
    case BOT_CMD_MOVE_DIR        :
         Parser.GetToken(false);
         Token2=Parser.token;
		 Token2.MakeLower();
              if(Token2=="forward" ) c2.Forse(1); 
         else if(Token2=="backward") c2.Forse(2); 
         else if(Token2=="left"    ) c2.Forse(3);   
         else if(Token2=="right"   ) c2.Forse(4); 
         else if(Token2=="up"      ) c2.Forse(5);  
         else if(Token2=="down"    ) c2.Forse(6);
         else if(Token2=="stop"    ) c2.Forse(11);
         else if(Token2=="stoprot" ) c2.Forse(10);
         if(Parser.GetToken(false))
         {
             Token2=Parser.token;
             if(Token2=="0" || Token2=="stop") c2.SetStop();
         }
         AddCMD(c2);
        return 1;
    case BOT_CMD_CHANGE_MOVE_TYPE:
    case BOT_CMD_DO_JUMP         :
    case BOT_CMD_GET_TO          :
    case BOT_CMD_LOOK_TO         :
         Parser.GetToken(false);
         l1.SetDirrection(Math::VectorFromString(Parser.token));
         if(Parser.GetToken(false))
             l1.SetSpeed(atof(Parser.token));
         AddCMD(l1);
         break;
    case BOT_CMD_LOOK_AT         :
         Parser.GetToken(false);
         BotOne=GetBotByName(Parser.token);
         if(BotOne)l2.SetWayPoint ((DWORD)BotOne);
         else      l2.SetWayTarget(Parser.token);
         if(Parser.GetToken(false))
         {
            l1.SetSpeed(atof(Parser.token));
         }
         AddCMD(l2);
        break;

//BOT CMD CLASS ATTACK

    case BOT_CMD_FIRE            :
         Parser.GetToken(false);
         Token2=Parser.token;
		 cfire.SetType(atoi(Token2));
		 AddCMD(cfire);
         return 1;
    case BOT_CMD_FIRE_TO         :
    case BOT_CMD_FIRE_TARGET     :
    case BOT_CMD_AIM             :
    case BOT_CMD_CHANGE_WEAPON   :
    case BOT_CMD_RELOAD_WEAPON   :
    case BOT_CMD_CHANGE_AIM_TYPE :
    case BOT_CMD_DROP_WEAPON     :

//BOT CMD ACTION

    case BOT_CMD_KICK            :
    case BOT_CMD_PUSH            :
    case BOT_CMD_GIVE            :
    case BOT_CMD_TAKE            :
    case BOT_CMD_USE             :
    case BOT_CMD_USE_ON          :
    case BOT_CMD_UNUSE           :
    case BOT_CMD_DO_ANIMATION    :
    case BOT_CMD_SAY             :
		Parser.GetTokenToLineEnd();
		Token2=Parser.token;
		{
		BOTCMD::CMD_SAY csay(Token2); 
		AddCMD(csay);
		}
		break;
    case BOT_CMD_KILL            :
    case BOT_CMD_HEAL            :
    case BOT_CMD_SUPPORT         :
    case BOT_CMD_DEVASTATE       :
    case BOT_CMD_COVERTHAT       :AISHADERTOKENERROR("not implimented");break;

//BOT CMD SCRIPT CONTROL
	case BOT_CMD_ALLERT_TRIG     :
        Parser.GetToken(false);
        Token2=Parser.token;
		Token2.MakeLower();
        dwVal=(DWORD)GetTriggerByName(Token2);
        ctrig1.SetTrigger(dwVal);
        AddCMD(ctrig1);
        return 1;
	case BOT_CMD_WAIT_TRIG     :
        Parser.GetToken(false);
        Token2=Parser.token;
		Token2.MakeLower();
        dwVal=(DWORD)GetTriggerByName(Token2);
        ctrig2.SetTrigger(dwVal);
        AddCMD(ctrig2);
        return 1;
	case BOT_CMD_CALL_TRIG     :
        Parser.GetToken(false);
        Token2=Parser.token;
		Token2.MakeLower();
        dwVal=(DWORD)GetTriggerByName(Token2);
        //ctrig3.SetTrigger(dwVal);
        //AddCMD(ctrig3); // triger emulated!! _!_
		ctrig1.SetTrigger(dwVal);  // call
        AddCMD(ctrig1);
		ctrig2.SetTrigger(dwVal);   //wait
        AddCMD(ctrig2);
        return 1;
	case BOT_CMD_WAIT:
        Parser.GetToken(false);
        Token2=Parser.token;
		Token2.MakeLower();
        dwVal=atoi(Token2);
        cwait.SetTime(dwVal);
		if(Parser.GetToken(false))
		{
          Token2=Parser.token;
		  if(Token2.length()>0)
           cwait.SetReturn(atoi(Token2));
		}
        AddCMD(cwait);
		break;
	case BOT_CMD_MAKEMARKER:
		 Parser.GetToken(false);
         Token2=Parser.token;
		 PATH::AddMarker(Token2,v3ValZERO);	
         if(Parser.GetToken(false))
		 {
			 Token3=Token2;
			 Token2=Parser.token;
			 if(Token2=="at")
			 {
			   Parser.GetTokenToLineEnd();
			   PATH::UpdateMarkerInfo(PATH::GetMarkerByName(Token3),Math::VectorFromString(Parser.token));
			 }
			 else
			 if(Token2=="on")
			 {
			   if(Parser.GetToken(false))
			   {
				   B1=GetBotByName(Parser.token);
				   if(B1)
					   B1->MakeMyMarker(PATH::GetMarkerByName(Token3));
				   else
					AISHADERTOKENERROR("target bot not set for 'makemarker on'");
			   }
			 }
			 else
			  AISHADERTOKENERROR("undefined parameter for makemarker");
		 }
		 break;
	case BOT_CMD_USEBOT:
	  	 Parser.GetToken(false);
         Token2=Parser.token;
		 cusebot.SetName(Token2);
         Parser.GetToken(false);
         cusebot.SetWait(strcmp(Parser.token,"1")==0);
         AddCMD(cusebot);
		 //CBaseBot *B1;B1=GetBotByName(Token2);
		 //if(!B1)AISHADERTOKENERROR("Bot %s not found",Token2.c_str());
		 //else   SetMyBot(GetBotByName(Token2));	 
		 break;

	case BOT_CMD_EXEC_CONSOLE:
		{
		 BOTCMD::CMD_EXEC_CONSOLE bm;
		 Parser.GetToken(false);
         Token2=Parser.token;
		 bm.SetText(Token2);
         AddCMD(bm);
		}
		 break;

    default : AISHADERTOKENERROR("not valid token");
    };
  return 0;
}