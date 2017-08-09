#include "stdafx.h"
#include "BaseBot.h"
#include "../Parser/Parser.h"


using namespace AI;

//_AICMDBSP CBotScript::AICMDBSP;

typedef SmartVector<int> _CMDPack;
_CMDPack   CMDPack;
_CMDPack   TrigPack;

BYTE      TRL1=0;
BYTE      TRL2[0xFF];


void FinishTrigers()
{
  TRL1=0;
  ZeroMemory(TRL2,sizeof(TRL2));
  TRL1=TrigPack.size();
  BYTE TTR[0xFF];
  ZeroMemory(TTR,sizeof(TTR));
  for(_CMDPack::iterator it=TrigPack.begin();it!=TrigPack.end();++it) 
  {
      BYTE Class=GET_BOT_CMD_CLASS(*it);
      TTR[Class]++;
  }
  for(int i=0;i<0xFF;++i)if(TTR[i]>0)TRL1=i+1;
}

inline void inAddCMD(DWORD ID,LPCSTR Name)
{
 static GString Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();CBotScript::AICMDBSP.PushAs(b,Base.v_str(),(DWORD)strlen(a))
 ADDBSP(ID,Name);
 CMDPack.push_back(ID);
#undef ADDBSP
}

inline void inAddTrig(DWORD ID,LPCSTR Name)
{
 static GString Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();CBotScript::TRIGCMDBSP.PushAs(b,Base.v_str(),(DWORD)strlen(a))
 ADDBSP(ID,Name);
 //for(_CMDPack::iterator it=TrigPack.begin();it!=TrigPack.end;++it) if(*it==ID) return;
 TrigPack.push_back(ID);
#undef ADDBSP
}

void AI::AddCMD(LPCSTR Name,DWORD ID){inAddCMD(ID,Name);}


void AI::LoadBaseCMDS()
{
	inAddCMD(BOT_CMD_MOVE_TO            ,"moveto");
	inAddCMD(BOT_CMD_MOVE_DIR           ,"move");
    inAddCMD(BOT_CMD_LOOK_TO            ,"forceangles");
    inAddCMD(BOT_CMD_LOOK_AT            ,"look");
	inAddCMD(BOT_CMD_CHANGE_MOVE_TYPE   ,"chmove");
	inAddCMD(BOT_CMD_DO_JUMP            ,"dojump");    
	inAddCMD(BOT_CMD_GET_TO             ,"getto");
//BOT CMD CLASS ATTACK
	inAddCMD(BOT_CMD_FIRE               ,"fire");
	inAddCMD(BOT_CMD_FIRE_TO            ,"fireat");
	inAddCMD(BOT_CMD_FIRE_TARGET        ,"fireto");
	inAddCMD(BOT_CMD_AIM                ,"aim");
	inAddCMD(BOT_CMD_CHANGE_WEAPON      ,"useweapon");
	inAddCMD(BOT_CMD_RELOAD_WEAPON      ,"reload");
	inAddCMD(BOT_CMD_CHANGE_AIM_TYPE    ,"reaim");
	inAddCMD(BOT_CMD_DROP_WEAPON        ,"drop_weapon");
//BOT CMD ACTION
	inAddCMD(BOT_CMD_KICK               ,"kick");
	inAddCMD(BOT_CMD_PUSH               ,"push");
	inAddCMD(BOT_CMD_GIVE               ,"give");
	inAddCMD(BOT_CMD_TAKE               ,"take");
	inAddCMD(BOT_CMD_USE                ,"use");
	inAddCMD(BOT_CMD_USE_ON             ,"useon");
	inAddCMD(BOT_CMD_UNUSE              ,"unuse");
	inAddCMD(BOT_CMD_DO_ANIMATION       ,"doanim");
	inAddCMD(BOT_CMD_SAY                ,"say");
	inAddCMD(BOT_CMD_KILL               ,"kill");
	inAddCMD(BOT_CMD_HEAL               ,"heal");
	inAddCMD(BOT_CMD_SUPPORT            ,"support");
	inAddCMD(BOT_CMD_DEVASTATE          ,"deva");
	inAddCMD(BOT_CMD_COVERTHAT          ,"cover");
//BOT CMD SCRIPT CONTROL
    inAddCMD(BOT_CMD_ALLERT_TRIG        ,"calltrig");
	inAddCMD(BOT_CMD_WAIT               ,"wait");
	inAddCMD(BOT_CMD_MAKEMARKER         ,"makemarker");
	inAddCMD(BOT_CMD_USEBOT             ,"usebot");
	inAddCMD(BOT_CMD_WAIT_TRIG          ,"waittrig");
	inAddCMD(BOT_CMD_CALL_TRIG          ,"dotrig");
	inAddCMD(BOT_CMD_PAUSE_TRIG         ,"pause");
	inAddCMD(BOT_CMD_RESUME_TRIG        ,"resume");
	inAddCMD(BOT_CMD_DUP_TRIG           ,"tdup");
	inAddCMD(BOT_CMD_IN_ALLERT          ,"allert");
	inAddCMD(BOT_CMD_EXEC_CMD           ,"exec");
	inAddCMD(BOT_CMD_EXEC_CONSOLE       ,"cmd");
	inAddCMD(BOT_CMD_MOD_VAR            ,"mod");


	

    //commands ok
//*************************************************************
//************ TRIGERS ****************************************
//*************************************************************

    inAddTrig(BOT_TRIG_HEAR_SHOT        ,"Hshot"); 
    inAddTrig(BOT_TRIG_HEAR_FRIEND      ,"Hfriend");  
    inAddTrig(BOT_TRIG_HEAR_ENEMY       ,"Henemy");  
    inAddTrig(BOT_TRIG_HEAR_NEUTAL      ,"Hneutal");  
    inAddTrig(BOT_TRIG_HEAR_SOUND       ,"Hsound");  
    inAddTrig(BOT_TRIG_HEAR_BOOM        ,"Hboom");  

//тригеры зрения

    inAddTrig(BOT_TRIG_SEE_FRIEND       ,"Sfriend");  
    inAddTrig(BOT_TRIG_SEE_ENEMY        ,"Senemy");  
    inAddTrig(BOT_TRIG_SEE_NEUTAL       ,"Sneutral");  
    inAddTrig(BOT_TRIG_SEE_DEAD         ,"Sdead");  
    inAddTrig(BOT_TRIG_SEE_COMBAT       ,"Scombat");  
    inAddTrig(BOT_TRIG_SEE_DANGER       ,"Sdanger");  

//тригеры физики

    inAddTrig(BOT_TRIG_PH_WAVE          ,"Pwave");  
    inAddTrig(BOT_TRIG_PH_SHOT          ,"Pshot");  
    inAddTrig(BOT_TRIG_PH_FALL          ,"Pfall");  
    inAddTrig(BOT_TRIG_PH_KICK          ,"Pkick");  
    inAddTrig(BOT_TRIG_PH_DAMAGE        ,"Pdamage");  
    inAddTrig(BOT_TRIG_PH_LOOSEITEM     ,"Plooseitem");  


//тригеры смерти

    inAddTrig(BOT_TRIG_DEATH_SELF       ,"Dead");  
    inAddTrig(BOT_TRIG_DEATH_PLAYER     ,"KillPlayer");  
    inAddTrig(BOT_TRIG_DEATH_ENEMY      ,"KillEnemy");  
    inAddTrig(BOT_TRIG_DEATH_FRIEND     ,"KillFriend");  
    inAddTrig(BOT_TRIG_DEATH_NEUTAL     ,"KillNeutal");  
    inAddTrig(BOT_TRIG_DEATH_COMMANDER  ,"KillCommander");

//тригеры движения

    inAddTrig(BOT_TRIG_MOVE_CLEAR_PATH  ,"Mclearway");  
    inAddTrig(BOT_TRIG_MOVE_NO_WAY      ,"Mnoway");  
    inAddTrig(BOT_TRIG_MOVE_NEW_WAY     ,"Mnewway");  
    inAddTrig(BOT_TRIG_MOVE_LOOKED_AT   ,"Mlookedat"); 
    FinishTrigers();

};

void CBaseBot::CreateScriptParameters()
{
	static bool SI_L1_init=false;
	if(SI_L1_init)return;
//	AddBotParam();
	SI_L1_init=true;
}

void AI::InitTrigerStructure(CBotTrigers::_CmdPack &Pack)
{
    _ScriptPack PackL2;
    for(int L1=0;L1<TRL1;++L1)
    {
        Pack.push_back(PackL2);
        for(int L2=0;L2<TRL2[L1];++L2)
            Pack[L1].push_back(NULL);//new CBotScript());
    }
}

/*

BOT_CMD_MOVE_TO
BOT_CMD_MOVE_DIR
BOT_CMD_CHANGE_MOVE_TYPE
BOT_CMD_DO_JUMP    
BOT_CMD_GET_TO    
BOT_CMD_LOOK_TO
BOT_CMD_LOOK_AT 

//BOT CMD CLASS ATTACK

BOT_CMD_FIRE
BOT_CMD_FIRE_TO
BOT_CMD_FIRE_TARGET
BOT_CMD_AIM
BOT_CMD_CHANGE_WEAPON
BOT_CMD_RELOAD_WEAPON
BOT_CMD_CHANGE_AIM_TYPE
BOT_CMD_DROP_WEAPON

//BOT CMD ACTION

BOT_CMD_KICK
BOT_CMD_PUSH
BOT_CMD_GIVE
BOT_CMD_TAKE
BOT_CMD_USE
BOT_CMD_USE_ON
BOT_CMD_UNUSE
BOT_CMD_DO_ANIMATION
BOT_CMD_SAY
BOT_CMD_KILL
BOT_CMD_HEAL
BOT_CMD_SUPPORT
BOT_CMD_DEVASTATE
BOT_CMD_COVERTHAT

*/


bool  AI::LoadAIScriptFileT(LPCSTR Name)
{
	DWORD file = bOpenFile(Name,"r",true);
    if(!file)
	{
		Error2Console(SYS_AI,"no script file to load - %s",Name);
		return false;
	}
	DWORD Len=bFileSize(file);
	char *buffer=new char[Len];
	bReadFile((LPVOID)buffer,Len,1,file);
	bCloseFile(file);
    bool ret= ParseAIScriptT(buffer,Len)==0;
    if(ret) Print2Console(SYS_AI,"%s loaded",Name);
    return ret;
}

bool  AI::RunScriptT(LPCSTR Name)
{
	CBotScript *BS=CBotScript::GetTriggerByName(Name);
	if(!BS)return false;
   
	return BS->Activate();
}

DWORD AI::ParseAIScriptT(LPCSTR Buf,DWORD Len)
{
	if(Len==-1)Len=strlen(Buf);
	if(Len<1)return -1;
	CParser Parser;
	GString Token;
	Parser.StartParseBuffer(Buf,Len);
	Parser.GetToken(true);
	GString SName=Parser.token;
    Print2Console(SYS_AI,"Found function:%s",SName.c_str());
    while(1)
    {
 	  	CBotScript *Script=new CBotScript(); 
		Script->SetName(SName);
		Script->MakeTrigger(SName);
	    int Depth=1;
	    if(Parser.GetToken(true))
 	    if(!IS_OPENBRACKET(Parser.token)) {Script->AISHADERTOKENERROR("Warning, '{' expected");return Parser.scriptline;}
	    while( Parser.GetTokenToLineEnd())//GetToken(true) )
	    {
               if(IS_OPENBRACKET (Parser.token)) {Depth++;continue;}
  	      else if(IS_CLOSEBRACKET(Parser.token)) {Depth--;if(Depth<1)break;continue;}
		  Token=Parser.token;
		  //Parser.GetTokenToLineEnd();
		  //Token=Token+" "+Parser.token;
		  if(Token.length()>0)
	       Script->Line(Token);
	    } 
	  Script->Reset();
	  if(!Parser.GetToken(true))break;
	  SName=Parser.token;
	  if(!SName.length())break;
	  Print2Console(SYS_AI,"Found function:%s",SName.c_str());
    }
    Print2Console(SYS_AI,"END");
	return 0;
}




