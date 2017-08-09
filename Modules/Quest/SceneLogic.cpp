#include "stdafx.h"	

#include "QuestScene.h"
#include "QuestGUI.h"
#include "QuestTalker.h"

#include "lua_functions.h"

CQuestScene::CQuestScene()
{
	DrivedActor=0;
}

CQuestScene::~CQuestScene()
{
	lua::doVstring("quest.scenes.%s=nil",Name.c_str());
	for(CQActors::iterator it=Actors.begin();it!=Actors.end();++it)
		if((*it)){delete (*it);*it=NULL;}

	for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)
		it->Drop();

	for(CQQuads::iterator it=BackScene.begin();it!=BackScene.end();++it)
		it->Drop();

	for(CQQuads::iterator it=FrontScene.begin();it!=FrontScene.end();++it)
		it->Drop();

	for(CQDQuads::iterator it=ZScene.begin();it!=ZScene.end();++it)
		it->Drop();

}

void ClearQuestScenes()
{
    lua::dostring("quest.scenes=nil");
    lua::dostring("quest.scenes={}");
	lua::dostring("quest.scenes.NULL={}");
    lua::dostring("function tmp_quest_activate() quest.SetScene(\"NULL\");end;");
    lua::dostring("quest.scenes.NULL.activate=tmp_quest_activate");
    lua::dostring("tmp_quest_activate=nil");
}

void AddQuestSceneToLua(LPCSTR FileName,bool AddRem=true)
{
	GString FN=FileName;FN.MakeLower();FileName=FN;
    lua::doVstring("quest.scenes.%s=nil",FileName);
    if(!AddRem)return;
	lua::doVstring("quest.scenes.%s={}",FileName);
	lua::doVstring("quest.scenes.%s.Name=\"%s\"",FileName,FileName);
    lua::doVstring("function tmp_quest_reload() quest.ReloadScene(\"%s\");end;",FileName);
    lua::doVstring("quest.scenes.%s.reload=tmp_quest_reload",FileName);
    lua::doVstring("tmp_quest_reload=nil",FileName);

    lua::doVstring("function tmp_quest_activate() quest.SetScene(\"%s\");end;",FileName);
    lua::doVstring("quest.scenes.%s.activate=tmp_quest_activate",FileName);
    lua::doVstring("tmp_quest_activate=nil",FileName);

    lua::doVstring("function tmp_quest_z() quest.utils.loadZmodel(\"%s\",1);quest.z_as_model=1;end;",FileName);
    lua::doVstring("quest.scenes.%s.showZ=tmp_quest_z",FileName);
    lua::doVstring("tmp_quest_z=nil",FileName);

    lua::doVstring("quest.scenes.%s.valid=0",FileName);
}

void ParseQuestCatalog()
{
 ClearQuestScenes();
 SearchFile search("Quest/Scenes/*",0,true);
 LPCSTR sname;
 for(sname=search.FirstFile(); sname; sname=search.NextFile())
 {
  LPCSTR name=sname;
  while(name[0]=='.')name++;
  if(name[0]!=0)
  if(!strstr(name,"."))
   {
    if(stricmp(name,"__def")!=0)
      AddQuestSceneToLua(name);
   }
 }
}

bool CQuestScene::Load(LPCSTR FileName)
{
/*	MyXRot=-1;
	MyFov=50;
	ModelsScale.Set(0.1f,0.1f,0.1f);
*/
	Name=FileName;
	Name.MakeLower();
	FileName=Name;
	GString FN="Quest/Scenes";FN=FN+"/"+FileName+"/";
	//init lua


	bool Sys=true;
	{
		CQuestQuad D;
	    ZScene.push_back(D);
		BackScene.push_back(D);
	}
	{
		CQuestQuad &D=ZScene[0];
		Sys&=D.LoadZ(FN+"Z.zga");
		D.SetPos(0,0);
	}
	{
		CQuestQuad &D=BackScene[0];
		Sys&=D.LoadColor(FN+"color.tga");
		D.SetPos(0,0);
	}
	{
//		Active.Load(FN);
//		lua::dofile(FN+"start.lua");
	}

	CQuestGraph::FrameSpan=0;
	AddQuestSceneToLua(FileName);
    lua::doVstring("quest.scenes.%s.valid=%d",FileName,Sys?1:0);
    this->sFileName=FileName;
    SetName(FileName);
	return Sys;
};

void CQuestScene::ReadCamInfo(bool Read)
{
 if(Read)
  {
   lua::dostring("SceneSetCam=nil");
   GString FName;FName=FName+"Quest/Scenes/"+sFileName+"/scripts/cameraset.lua";
   lua::dofile(FName);
  }
 lua::dostring("SceneSetCam();win_updateviewport=1;");
}


void CQuestScene::Activate()
{
  ReadCamInfo(true);
  lua::dostring("SceneStart=nil");
  lua::dostring("SceneTick=nil");
  GString FName;FName="Quest/Scenes/"+sFileName+"/scripts/activate.lua";
  lua::dofile(FName);  


  QluaObjects();

  FName="Quest/Scenes/"+sFileName+"/scripts/objectdef.lua";
  lua::dofile(FName);



  lua::dostring("DefineObjects()");
  lua::dostring("DefineObjects=nil");

  FName="Quest/Scenes/"+sFileName+"/scripts/main.lua";
  lua::dofile(FName);

  lua::dostring("SceneStart()");
  lua::dostring("SceneStart=nil");
}
void CQuestScene::Deactivate()
{
  lua::dostring("SceneStop=nil");
  GString FName;FName=FName+"Quest/Scenes/"+sFileName+"/scripts/deactivate.lua";
  lua::dofile(FName);  
  lua::dostring("SceneStop()");
  lua::dostring("SceneStop=nil");
  lua::dostring("SceneTick=nil");
}


void pushCam() {World()->Camera()->Select2D();}

void popCam () {World()->Camera()->Select3D();}


void CQuestScene::Update(void)
{
}

void CQuestScene::RenderZ()
{
	static MakeProfile(PZ     ,"Z");
    RenderList.clear();
	Profile(PZ);
	popCam();
	for(CQDQuads::iterator it=ZScene.begin();it!=ZScene.end();++it)
		it->Render();
	popCam();   
}

void CQuestScene::RenderBack()
{
	static MakeProfile(PBack  ,"Back");
	Profile(PBack);
	pushCam();
	RenderList.clear();
	RENDER->DisableWrite(LAYER_DEPTH);
	RENDER->GetOverlay()->SetWorkLevel(1);
	for(CQQuads::iterator it=BackScene.begin();it!=BackScene.end();++it)
	{
		const int List=it->Render();
		if(List) RenderList.push_back(List);
	}
	RENDER->GetOverlay()->Render();
	RENDER->GetOverlay()->SetWorkLevel(0);
    popCam();
}

void CQuestScene::DrawModels(const bool ZGrid)
{
//	float zsin;
//	zsin=acos(FloorNormal[2])*180/PI;
}

#include "coregraphics.h"
#pragma comment (lib,"OpenGL32.Lib")

extern bVector3f PX;
extern int ZMaterial;

void CQuestScene::RenderModel(const bool ZGrid)
{

/*glPushMatrix();
glTranslatef(PX[0],PX[1],PX[2]);
static Draw::Skin::SkinnedMesh *M=NULL;
if(!M)
	{
	  M=new Draw::Skin::SkinnedMesh("Quest\\Scenes\\s0_model\\korotkov.fsm");
	  //M->Load();
	}

        RENDER->EnableWrite(LAYER_COLOR);
        RENDER->EnableWrite(LAYER_DEPTH);
     	RENDER->CullFace(CULL_NONE);	   
	    RENDER->SetZMode(DSF_DEPTH_LEQUAL);
        RENDER->EnableVSShader("skin.cg");
        Textures::DisableTexturing();
        Textures::BindTexture(ZMaterial,0);
        M->render();
        RENDER->DisableVSShader();

glPopMatrix();
*/
        RENDER->EnableWrite(LAYER_COLOR);
        RENDER->EnableWrite(LAYER_DEPTH);
     	RENDER->CullFace(CULL_NONE);	   
	    RENDER->SetZMode(DSF_DEPTH_LEQUAL);
        RENDER->EnableVSShader("skin.cg");
        Textures::DisableTexturing();
        Textures::BindTexture(ZMaterial,0);
		for(CQActors::iterator it=Actors.begin();it!=Actors.end();++it)
			(*it)->RenderFront();
        //M->render();
        RENDER->DisableVSShader();

#if 0
	{
	BackBuffer.Enable();
 	popCam();
	RENDER->EnableWrite(LAYER_DEPTH);
	RENDER->SetZMode(DSF_DEPTH_LEQUAL);
	RENDER->CullFace(CULL_FRONT);
/*	glViewport(0,1024-768,1024,768);
	glScissor(0,1024-768,1024,768);
*/
	RENDER->ForseClear(RENDER_CLEAR_DEPTH);
    DrawModels(false);
    BackBuffer.Disable();
	}
	{
		RENDER->DisableWrite(LAYER_COLOR);
		RENDER->ForseClear(RENDER_CLEAR_STENCIL);
		/*
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		*/
	    DrawModels(ZGrid);
		//glDisable(GL_STENCIL_TEST);
		RENDER->DisableWrite(LAYER_COLOR);
	}
#endif
}

//#include "coregraphics.h"
//#pragma comment (lib,"OpenGL32.Lib")


void CQuestScene::RenderMisc()
{
	static MakeProfile(PMisc  ,"Misc");
	if(lua::getdword("gui.QPanel.Admin.SFloor.On")==0){}
	else
	{
		//glColor3f(1,0,0);
        RENDER->SetIMColor(bVector4f(1,0,0,0));
        Textures::DisableTexturing();
        RENDER->CullFace(CULL_NONE);
        RENDER->DisableVSShader();
        RENDER->SetZMode(DSF_DEPTH_LEQUAL);
		/*glBegin(GL_QUADS);
	    int i;
		for(i=0;i<4;++i)
//			glVertex3fv(FloorPoints[i]);
		glEnd();

		glLineWidth(5);
		glBegin(GL_LINES);
		for(i=0;i<4;++i)
		{
//		 glVertex3fv(FloorPoints[i]);
//		 glVertex3fv(FloorPoints[i]+FloorNormal*10);
		}
		glEnd();*/
		
	}
}

void CQuestScene::RenderFront(void)
{
    static MakeProfile(PFront ,"Front");
	Profile(PFront);
	pushCam();

	//glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
	RENDER->SetBlending(BLEND_OFF);

	/*BackBuffer.Bind(glFullScreenTex);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glCallList(glFullQuadList);
	glDisable(GL_STENCIL_TEST);
	BackBuffer.Unbind();
	*/
	RenderList.clear();
	//glDepthMask(0);
	for(CQQuads::iterator it=FrontScene.begin();it!=FrontScene.end();++it)
		it->Render();
	popCam();
}


void InCamLocked(CQuestScene *For)
{
    static int CamoldStat=0;
	if(lua::getdword("gui.QPanel.Admin.CamFix.On")==1)
	{ 
        if(CamoldStat==0)
            For->ReadCamInfo(true);
        CamoldStat=1;
		World()->Camera()->EnableChanges(false);
	}
	else 
    {
		World()->Camera()->EnableChanges(true);
        CamoldStat=0;
    }
}

DWORD CQuestScene::Tick(float T)
{
 lua::dostring("SceneTick();");
 return 1;
}

DWORD q_showallobjects=0;
GString q_selected;

void CQuestScene::Render(void)
{
	static MakeProfile(PRender,"Render");
	Profile(PRender);
    InCamLocked(this);
    Tick(1.0);
    SetRenderKeys();
	RenderZ();
	if(lua::getdword("quest.hide_color")==0)
	  RenderBack();
	RenderModel();
  	RenderFront();
	RenderMisc();
	popCam();

	//CHECK VARS
	q_showallobjects=lua::getdword("quest.showallobjects");
	q_selected      =lua::getstring("quest.showobjects_selected");
}

void CQuestScene::SetFloor(bVector3f *V,bVector3f Nrm)
{
//	FloorNormal=Nrm;
//	for(int i=0;i<4;i++)
//	FloorPoints[i]=V[i];
}

bVector3f CQuestScene::ModPosByFloor(const float *V)
{
/*    if(FloorNormal[2]==0 || FloorNormal.IsConsiderZero())return V;
	float H=V[2];
	float D[4];
	for(int i=0;i<4;++i)
	D[i]=FloorPoints[i][0]*FloorNormal[0]+FloorPoints[i][1]*FloorNormal[1]+FloorPoints[i][2]*FloorNormal[2];
	H=-(V[0]*FloorNormal[0]+V[1]*FloorNormal[1]-D[0])/FloorNormal[2];
	return bVector3f(V[0],V[1],H);
	*/
	return V;
}

void FirstAK(BYTE &AK,bool *UseBuf)
{
 UseBuf+=AK;
 for(;AK<0xFF;++AK,++UseBuf)if(!(*UseBuf))return;
}

BYTE NextAK(BYTE &AK,bool *UseBuf)
{
 ++AK;
 BYTE RETAK=AK;
 UseBuf+=AK;

 for(;AK<0xFF;++AK,++UseBuf)if(!(*UseBuf))break;
 if(RETAK>250)
 {
	 RETAK=0;
	 AK=0;
 }
 return RETAK;
}


void CQuestScene::SetRenderKeys()
{
 bool UseBuf[0xFF];
 ZeroMemory(UseBuf,sizeof(UseBuf));
 UseBuf[0]=1;
#define CHECK_IT(it) if(it->IsStatic() && it->Active() && it->Visibled())UseBuf[it->GetAlphaKey()]=true;
  {for(CQObjects::Citerator it=Objects.begin();it!=Objects.end();++it)CHECK_IT(it) }
 // may be where is const actors??
  {for(CQActors::Citerator  it= Actors.begin();it!= Actors.end();++it)CHECK_IT((*it)) }
 //next set keys for others objects
 BYTE AK=0;
 FirstAK(AK,UseBuf);
 --AK;
#undef CHECK_IT
#define CHECK_IT(it) if(!(it)->IsStatic() && (it)->Visibled()){BYTE USEAK=NextAK(AK,UseBuf);if(!USEAK)break;(it)->UseKey(USEAK);}else (it)->UseKey(0);
if(q_showallobjects==2)
 {
 for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)CHECK_IT(it)
 for(CQActors::iterator  it= Actors.begin();it!= Actors.end();++it)CHECK_IT((*it))
 }
else
 {
#undef CHECK_IT
#define CHECK_IT(it) if(!(it)->IsStatic() && it->Active() && (it)->Visibled()){BYTE USEAK=NextAK(AK,UseBuf);if(!USEAK)break;(it)->UseKey(NextAK(AK,UseBuf));}else (it)->UseKey(0);

 for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)CHECK_IT(it)
 for(CQActors::iterator  it= Actors.begin();it!= Actors.end();++it)CHECK_IT((*it))
 }
#undef CHECK_IT
}

static bool iSelected;

bool MkSelObject(CQuestObject* it,int I,int IND=0)
{
    static double ONE255=1.0/0xFF;
    if(!(it)->Visibled())return 0;
	 if(iSelected)
		 RENDER->SetIMColor(bVector4f(1,0,0,it->GetAlphaKey()*ONE255));
	 else
         RENDER->SetIMColor(bVector4f(1,1,1,it->GetAlphaKey()*ONE255));
	 if(IND)
	 (*CQuestScene::ObjectAliasName)[I]=IND;
	return true;
}

bool CQuestScene::ModAlphaForObject(const DWORD I,Draw::Skin::Surface *For)
{
  static MakeSectionProfile(pr1,"CQuestScene::ModAlphaForObject");
  static MakeSectionProfile(pr2,"CQuestScene::ModAlphaForObject2");
  Profile(pr1);
  //find object
  static double ONE255=1.0/0xFF;
 /* if(q_showallobjects==2)
  {  
     if(q_selected==For->GetGName())
		 RENDER->SetIMColor(bVector4f(1,0,0,(I+1)*ONE255));
	 else
         RENDER->SetIMColor(bVector4f(1,1,1,(I+1)*ONE255));
     return true;
  }
 */
  const GString &Name=For->GetGName();
  if(q_showallobjects==3)
   iSelected=true;
  else
   iSelected=q_selected==Name;
  int AliasObject=0;
  if((int)I<ObjectAliasName->size())
  {
   AliasObject=(*ObjectAliasName)[I];
   if(AliasObject>0)
	{
	if(	Objects[AliasObject-1].IsYouSurface(Name))
	 return MkSelObject(&Objects[AliasObject-1],I,AliasObject);
	}else
   if(AliasObject<0)
		if(	Actors[-AliasObject+1]->IsYouSurface(Name))
	 return MkSelObject(Actors[-AliasObject+1],(int)I,-AliasObject);
  }
  //find objects
   {Profile(pr2);
   int i=1;
  for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it,++i)
  if((it)->IsYouSurface(Name))
    return MkSelObject(it,I,i);
   i=-1;
   for(CQActors::iterator it=Actors.begin();it!=Actors.end();++it,--i)
   if((*it)->IsYouSurface(Name))
     return MkSelObject(*it,-(int)I,i);
   }
  RENDER->SetIMColor(bVector4f(1,1,1,0));
  return q_showallobjects>0;
}

CQuestActiveElement CQuestScene::GetActiveByColor(const DWORD X,const DWORD Y,const DWORD RGBA)
{
  CQuestActiveElement AE;
  BYTE AC=(RGBA>>24);
if(q_showallobjects==2)
 {
  #define CHECK_IT(it,B) if((it)->Visibled() && (it)->GetAlphaKey()==AC)return CQuestActiveElement((it),B);
  for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)CHECK_IT(it,1)
  for(CQActors::iterator  it= Actors.begin();it!= Actors.end();++it)CHECK_IT((*it),2)
 }
else
 {
 #undef CHECK_IT
 #define CHECK_IT(it,B) if(it->Active() && (it)->Visibled() && (it)->GetAlphaKey()==AC)return CQuestActiveElement((it),B);
  for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)CHECK_IT(it,1)
  for(CQActors::iterator  it= Actors.begin();it!= Actors.end();++it)CHECK_IT((*it),2)
 }
  return AE;
}

bool CQuestScene::DefineObject(LPCSTR Name,DWORD Location)
{
 //for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)
 // if((it)->IsYou(Name)){ it->Use();return true;}

 lua::doVstring("quest.objects.%s={}",Name);
 Objects.push_back(CQuestObject(Name,Location));
 return true;
}

bool CQuestScene::DefineActor(LPCSTR Name,DWORD Location)
{
 lua::doVstring("quest.actors.%s={}",Name);
 CQuestActor *Actor=new CQuestActor();
 Actor->SetName(Name);
 if(!Actor->Define(Name))
 {
	 Error2Console(SYS_QUEST,"Unable to set actor as <%s>",Name);
	 delete Actor;
	 return false;
 }
 Actor->SetLocation(Location);
 Actors.push_back(Actor);
 Actor->SetStackID(Actors.size());
 return true;
}

const CQuestObject* CQuestScene::GetObjectByName(LPCSTR N)const
{
 GString Name=N;
 for(CQObjects::Citerator it=Objects.begin();it!=Objects.end();++it)
   if((it)->IsYou(Name))return it;
 return NULL;
}
CQuestObject* CQuestScene::GetObjectByName(LPCSTR N)
{
 GString Name=N;
 for(CQObjects::iterator it=Objects.begin();it!=Objects.end();++it)
   if((it)->IsYou(Name))return it;
 return NULL;
}

bool CQuestScene::DriveActor  (LPCSTR Name)
{
	GString gName=Name;
	for(CQActors::iterator  it= Actors.begin();it!= Actors.end();++it)
		if((*it)->IsYou(gName))
		{
			if(DrivedActor) DrivedActor->Undrive();
			DrivedActor=*it;
			DrivedActor->Drive();
			return true;
		}

	return false;
}


int  CQuestActiveElement::OnMouseIn   ()
{
   lua::doVstring("quest.objects.%s._onmousein(\"%s\");",GetName(),GetName());
   return 0;
}
int  CQuestActiveElement::OnMouseOut  ()
{
   lua::doVstring("quest.objects.%s._onmouseout(\"%s\");",GetName(),GetName());
   return 0;
}
int  CQuestActiveElement::OnMouseClick()
{
   lua::doVstring("quest.objects.%s._onmouseclick(\"%s\");",GetName(),GetName());
   return 0;
}
int  CQuestActiveElement::OnMouseMove ()
{
   lua::doVstring("quest.objects.%s._onmousemove(\"%s\");",GetName(),GetName());
   return 0;
}

