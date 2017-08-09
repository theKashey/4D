#include "stdafx.h"
#include "BaseBot.h"
#include "aiplugin.h"

CAIPlugIn::CAIPlugIn(void)
{
	SetName("AIv1");
}

CAIPlugIn::~CAIPlugIn(void)
{
}

bool CAIPlugIn::Start()
{
 AI::CreateAI();
 return true;
}

DWORD CAIPlugIn::OnMessage(const CBlopMessage &Message)
{
    switch(Message.Msg)
    {
    case bCMD_UPDATE : AI::RetouchScripts();return 0;
    case bCMD_RESTART: AI::RetouchScript("common");return 0;
    case bCMD_STOP   : AI::RetouchScripts("STOP");return 0;
    };
    return 0;
}

void CAIPlugIn::Stop(){}

DWORD CAIPlugIn::Tick(float Time)
{
	AI::CBotScript::RunActive();
	return 0;
};

CAIPlugIn *MyPlugIn=NULL;

PBlopBaseObject GetAI()
{if(!MyPlugIn)MyPlugIn=new CAIPlugIn;
 return MyPlugIn;
}

PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN 
    MAKE_PLUGIN (GetAI,"AI",6,true);
    END_PLUGIN
}
