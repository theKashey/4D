#include "stdafx.h"
#include "QuestScene.h"

extern DWORD SYS_QUEST;

class CQuestPlugIn : public BlopBaseObject
{
public:
	CQuestPlugIn(void){SetName("Quest engine");}
	~CQuestPlugIn(void){}
		
	CQuestGraph *GR;

	bool Start()
	{
		GR=CQuestGraph::MakeEntry();//new CQuestGraph();
		//RENDER->AddObject(GR);
		RENDER->GetRoot()->AddChild(GR);
		Print2Console(SYS_QUEST,"quest module started");
		GR->Init();
		return true;
	}
};

PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN
    MAKE_PLUGIN (new CQuestPlugIn,"QUEST",0,true);
    END_PLUGIN
}
