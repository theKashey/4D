#include "stdafx.h"
#include "QuestScene.h"

CQuestGraph *defQuest=NULL;

CQuestGraph *CreateQuest()
{
	return defQuest;
}

LUAFUNC(qLoad  ){defQuest->LoadScene  (lua::_tostring(L,1),false);return 0;}
LUAFUNC(qGo    ){defQuest->LoadScene  (lua::_tostring(L,1),true);return 0;}
LUAFUNC(qReload){defQuest->ReloadScene(lua::_tostring(L,1),true);return 0;}
#	pragma comment (lib,"liblua.lib")


void CQuestGraph::InitLayer(CQuestGraph *G)
{
//	using namespace luabind;
    defQuest=G;

    lua::dostring("quest={}");
    lua::luafunction("ReloadScene",qReload,"quest");
    lua::luafunction("SetScene",qGo,"quest");
}
