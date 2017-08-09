#include "stdafx.h"

CQuestActor::CQuestActor()
{
	Mesh=NULL;
    Clear();
}
CQuestActor::~CQuestActor()

{
	Clear();

}
void CQuestActor::Clear ()
{
	ClearScriptStack();
	Mesh          = 0;
	Position      .SetZero();
	Orientation   .SetZero();
	Scale         .SetOne ();
	Tag           = 0;
	Drived        = 0;
	CurState      = 0;
	CurAutomate   = 0;
	ScriptDrive   .Empty();

	ScriptStack.clear();
	VectorStackID =0;
	if(Mesh) 
	{
      delete Mesh;
	  Mesh=NULL;
	}
}

void CQuestActor::ClearScriptStack()
{
	for(CQuestScriptPack::iterator it=ScriptStack.begin();it!=ScriptStack.end();++it)
	if(*it)
	{
		delete *it;
		*it=NULL;
	}
    ScriptStack.SetSize(0);
}
bool CQuestActor::Define(LPCSTR _ActorName)
{
	GString ActorName=_ActorName;
	//read actor dirrectory
	//get model name and actor drive script
	GString FilePath=("Quest/Actors/")+ActorName+"/";
	GString ModelFile=FilePath+"model.fsm";
	GString ACModel="QA_";ACModel+=ActorName;
	Mesh=new Draw::Skin::SkinnedMesh();
	if(!Mesh->Load(ModelFile,ACModel))
	{
		delete Mesh;
		return false;
	}
	return true;
    
}

void CQuestActor::SetStackID(DWORD SID)
{
	VectorStackID=SID;
}

bool CQuestActor::RenderFront(DWORD Path)
{
	if(Mesh)
	{
		RENDER->PushMatrix();
		Draw::CTransformer TR;
		TR.SetTRS(this->Position,0xFFFF,this->Orientation,this->Scale);
		RENDER->ApplyTransform(TR);
		Mesh->render();
		RENDER->PopMatrix();
	}
	return true;
}

void CQuestActor::Drive()
{
}
void CQuestActor::Undrive()
{
}