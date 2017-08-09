#include "stdafx.h"	

#include "QuestScene.h"
#include "QuestGUI.h"
#include "QuestTalker.h"

#include "lua_functions.h"

//#define Profile(a) {out->Insert(#a);out->Insert("-");}_Profilers::CProfilerLocker PLOCK(&a);


DWORD SYS_QUEST=0;

int CQuestGraph::FrameSpan=0;

Draw::CPBuffer     CQuestScene::BackBuffer;


int ZMaterial;

float fRDepthLen=1;

bool ZGridInited=false;

float *CurrentZData=NULL;
float *ReadyZData=NULL;

#include "DepthInit.h"

ZGridData *ZDataGrid=NULL;

 WORD     *zdinds   =NULL;
DWORD      zdindsize[0xFF];
DWORD      zoffs    [0xFF];

Draw::CVBOElement      ZDataVerts;
Draw::CBaseRenderIndex ZDataInds;


DWORD BackTexture=0;

float ReadZ(const int x,const int y)
{
	if(x<0 || y<0)return 0;
	if(CurrentZData)
	return CurrentZData[x+(y<<10)];
	else return 0;
}

float  CQuestGraph::ReadZ(const int x,const int y)
{
	return ::ReadZ(x,y);
}

void InRenderZData()
{
	if(lua::getdword("q_showdepth"))
	{
      RENDER->EnableWrite(LAYER_COLOR);
	  RENDER->EnableVSShader("ztex.cg");
	  Textures::DisableTexturing();
	  Textures::BindTexture(BackTexture,0);
	}
	else
	  RENDER->DisableWrite(LAYER_COLOR);
	RENDER->CullFace(CULL_NONE);
	RENDER->EnableWrite(LAYER_DEPTH);
	RENDER->SetZMode(DSF_DEPTH_LEQUAL);

	Draw::CRenderVertex RV;
	Draw::CRenderStream StreamOne;
	StreamOne.SetVBOID(&ZDataVerts);
	StreamOne.SetStride(sizeof(ZDataGrid[0]));
	StreamOne.SetVertex(0);
	StreamOne.SetTex(0,(LPVOID)sizeof(bVector3f));
	ZDataInds.SetQua();
	RV.SetIndex(&ZDataInds);
	RV.AddStream(StreamOne);
	DEF_RENDER *Driver=RENDER;
	Driver->StartSession();

	int Off=0;
	float t=1;
	bVector4f C(t,t,t,0);
	Driver->SetIMColor(C);
	for(int i=lua::getdword("q_b");i<12;++i)
	{
		if(!zdindsize[i])break;
		RV.ModStream(0)->SetVertex((LPVOID)(zoffs[i]*sizeof(ZDataGrid[0])));
		RV.ModStream(0)->SetTex(0,(LPVOID)(sizeof(bVector3f)+zoffs[i]*sizeof(ZDataGrid[0])));
		ZDataInds.SetLen(zdindsize[i]);
		Driver->FlushRV(&RV);
		Off+=zdindsize[i];
	}
	Driver->EndSession();
	Driver->DisableVSShader();
}

void RenderZData()
{
	if(ReadyZData!=CurrentZData)
	{
	 DWORD realX=lua::getdword("win_width");
	 DWORD realY=lua::getdword("win_height");
	 static DWORD RRXY=0;DWORD _RRXY=realX*realY;
	 if(!ZDataGrid || RRXY!=_RRXY){ if(ZDataGrid)delete []ZDataGrid;ZDataGrid=new ZGridData[realX*realY];}
	 if(!zdinds    || RRXY!=_RRXY){ if(zdinds   )delete []zdinds   ;zdinds   =new  WORD[realX*realY*7];}
	 bool RVXTC=false;
	 if(RRXY!=_RRXY)RVXTC=true;
	 RRXY=_RRXY;
	 //create z data...
	 DWORD Time[16]; Time[0]=Time::Get();
//	 float inx,iny;
	 float finalMatrix[16];
	 di_matrixprep(finalMatrix);
	 float deltainx=1024.0f/((float)realX);if(deltainx<1)deltainx=1;
	 float deltainy=768.0f /((float)realY);if(deltainy<1)deltainy=1;

	 realX=1024.0f/deltainx;
	 realY=768.0f /deltainy;
 	 const float fzdRezX=1.0f/realX;
	 const float fzdRezY=1.0f/realY;//x=1024/y; y*x=1024;y
     di_ProcessPack(realX,realY,deltainx,deltainy,fzdRezX,fzdRezY);
	 Time[1]=Time::Get();
	ZDataVerts.SetType(VBO_STATIC_GEOM);
	ZDataVerts.SetSize(sizeof(ZDataGrid[0])*realX*realY);
	ZDataVerts.SybData(ZDataGrid,sizeof(ZDataGrid[0])*realX*realY,0);
	 Time[2]=Time::Get();
	WORD cindex=0;
	DWORD indSize=0;
	DWORD WindSize=0;
	if(!ZDataInds.IsSet() || RVXTC)
	{
	zdindsize[0]=0;
	zdindsize[1]=0;
	int OffIndex=0;
	for(DWORD y=0;y<realY-1;++y)
	{
	 DWORD x=0;
	 for(;x<realX-1;++x)
	 {
#define V(a,b) (x+a)+(y+b)*realX-OffIndex
	  if(cindex>0)indSize+=4;
	  else{

	  zdinds[indSize++]=V(0,0);
	  zdinds[indSize++]=V(1,0);
	  zdinds[indSize++]=V(1,1);
	  zdinds[indSize++]=V(0,1);
	  }
	 }
	 x=0;
	  if(V(realX,2)>=0xFFFF)
	  {
		 zoffs[cindex]=OffIndex;
 		 OffIndex=0;
 		 OffIndex=V(0,1);
		 zdindsize[cindex]=indSize-WindSize;
		 if(WindSize==0)
		 {
			 DWORD miss=0;
			 Print2Console(SYS_QUEST,"So set is %f%% for first time",RenderTech::triCheckCashe(zdinds,indSize,miss));
			 //RenderTech::IndexToList(zdinds,indSize);
			 //Print2Console(SYS_QUEST,"So set is %f%% for second time",RenderTech::triCheckCashe(zdinds,indSize,miss));
			 //DWORD f=bOpenFile("1024strip.dat","wb+");
			 //bWriteFile(zdinds,indSize*sizeof(*zdinds),1,f);
			 //bCloseFile(f);
		 }
		 WindSize=indSize;
		 cindex++;
		 zdindsize[cindex]=0; 
	  }
#undef V
	}
	}
	Time[3]=Time::Get();
	if(!ZDataInds.IsSet() || RVXTC)
	{
	 zdindsize[cindex]=indSize-WindSize;
	 zdindsize[cindex+1]=0;
	 WindSize=indSize;
	 ZDataInds.SetType(VBO_STATIC_INDEX);
	 ZDataInds.SetSize(sizeof(zdinds[0])*WindSize);
	 ZDataInds.SybData(zdinds,sizeof(zdinds[0])*WindSize,0);
	}
	Time[4]=Time::Get();
	Print2Console(SYS_QUEST,"z level generation : V:%d|%d I:%d|%d\n",Time[1]-Time[0],Time[2]-Time[1],Time[3]-Time[2],Time[4]-Time[3]);
	}
	ReadyZData=CurrentZData;
	InRenderZData();
	
}



CQuestQuad::CQuestQuad()
{
	QType=QD_NONE;
	glList=0;
	Material=0;
}
CQuestQuad::~CQuestQuad()
{
	Drop();
}

void CQuestQuad::Drop()
{
}

bool CQuestQuad::LoadZ(LPCSTR Name)
{
	DWORD f=bOpenFile(Name,"rb");
	if(!f) return false;
	// FORMAT
	// 4b w,h
	// 4b z32
	// 4b zfar,znear
	// 4b zdx,zdy
	// data...
	bReadFile(&W    ,4,1,f);
	bReadFile(&H    ,4,1,f);
	DWORD z32;
	bReadFile(&z32  ,4,1,f);
	bReadFile(&zNear,4,1,f);
	bReadFile(&zFar ,4,1,f);
	bReadFile(&z32  ,4,1,f);
	bReadFile(&z32  ,4,1,f);

	Data.SetType(VBO_DATA_STORE);
	Data.SetSize(W*H*4);
	bReadFile(Data.Lock(),W*H*4,1,f);
	Data.Unlock();
	Data.Unbind();
	bCloseFile(f);
	QType=QD_Z;
	return true;
}


bool CQuestQuad::LoadColor(LPCSTR Name)
{
	QType=QD_COLOR;	 
	W=1024;
	H=768;
	Material=Textures::LoadTexture(Name);
	return true;

}

DWORD glFullQuadList=0;
DWORD glFullScreenTex=0;

bool  NeedDrawZ()
{
	bool wr= lua::getdword("r_winrestart")>0;
	bool dc= lua::getdword("r_depthcleared")>0;
	return wr||dc;
}

int CQuestQuad::Render()
{
    static LastDepthIsModel=0;
	switch(QType)
	{
		case QD_NONE :return 0;
		case QD_COLOR:
			          RENDER->GetOverlay()->QuadT(Material,0,0,640,480,v4ONE,1,0.75,0,0,BLEND_ADD);
					  BackTexture=Material;
			          return 0;
		case QD_Z    :
            if(lua::getdword("quest.z_as_model")==1 && CQuestGraph::zmesh)
            {
                  //if(CQuestGraph::zmesh)
       {
       static MakeProfile(pr1,"Prepare");
       static MakeProfile(prr1,"1");
       static MakeProfile(prr2,"2");
       static MakeProfile(prr3,"3");
       static MakeProfile(prr4,"4");
       static MakeProfile(prr5,"5");
       static MakeProfile(prr6,"6");
       static MakeProfile(pr2,"Render");
        {Profile(pr1);
        //RENDER->ForseClear(RENDER_CLEAR_DEPTH);
        {Profile(prr1);
        lua::setdword("render.clearstyle",RENDER_CLEAR_COLOR | RENDER_CLEAR_STENCIL | RENDER_CLEAR_DEPTH);
        }
        {Profile(prr2);
        RENDER->EnableWrite(LAYER_COLOR);
        RENDER->EnableWrite(LAYER_DEPTH);
        }
        {Profile(prr3);
     	RENDER->CullFace(CULL_NONE);	   
	    RENDER->SetZMode(DSF_DEPTH_LEQUAL);
        }
        {Profile(prr4);
        //RENDER->EnableVSShader("ztex.cg");
		}
        {Profile(prr5)
         Textures::DisableTexturing();}
        {Profile(prr6)
         Textures::BindTexture(ZMaterial,0);
		}
        }
                   
        {
        Profile(pr2);
        CQuestGraph::zmesh->render();
        }
        RENDER->DisableVSShader();
        return 1;
       }
            }
            else
			        if(ReadyZData!=CurrentZData || NeedDrawZ() || lua::getdword("q_showdepth"))
					{
						if(NeedDrawZ())ReadyZData=NULL;
						RENDER->SetZMode(DSF_DEPTH_LEQUAL);
						RENDER->EnableWrite(LAYER_DEPTH);
						RENDER->DisableWrite(LAYER_COLOR);
						CurrentZData=(float*)Data.BufferOffset(0);
						RENDER->ForseClear(RENDER_CLEAR_DEPTH);
						lua::setdword("render.clearstyle",RENDER_CLEAR_COLOR | RENDER_CLEAR_STENCIL);
						RenderZData();						
						ZGridInited=false;
						RENDER->SetZMode(DSF_DEPTH_OFF);
						RENDER->EnableWrite(LAYER_COLOR);
					}
					return 0;
	}
	return 0;
}

//CQuestScene
void CQuestScene::Save()
{
}



GUIWindow *mGUIWindow=NULL;

Draw::Skin::SkinnedMesh *CQuestGraph::zmesh=NULL;



CQuestGraph::CQuestGraph()
{
  CurrentScene=0;
  LastScene   =0;
  zmesh       =0;
  RegisterNewConsoleClass(SYS_QUEST,"Quest",-1);
}

CQuestGraph::~CQuestGraph()
{
}

void CQuestGraph::Update(void)
{
}

int CQuestGraph::Draw(CNodeHistory *H)
{
	static MakeProfile(PQRender,"QuestRender");
	Profile(PQRender);

	if(CurrentScene!=LastScene)
		RENDER->ForseClear(RENDER_CLEAR_DEPTH);

	LastScene=CurrentScene;

	CQuestGraph::FrameSpan++;
    static int LastCurrentScene=-1;

	if(CurrentScene)
    {
		if(Scenes[CurrentScene-1])
		{
          if(LastCurrentScene!=CurrentScene)
          {
              if(LastCurrentScene>0)
               if(Scenes[LastCurrentScene-1])
                Scenes[LastCurrentScene-1]->Deactivate();
			  //clear zmesh
			  if(CQuestGraph::zmesh)delete CQuestGraph::zmesh;
              CQuestGraph::zmesh=NULL;
              Scenes[CurrentScene-1]->Activate();
          }
          LastCurrentScene=CurrentScene;
	      Scenes[CurrentScene-1]->Render();
		  return 1;
		}
    }
	return 0;	
}   


CQuestScene::CglLists *CQuestScene::ObjectAliasName=NULL;

void CQuestGraph::Init(void)
{
    if(!mGUIWindow)
	{
		mGUIWindow=new GUIWindow();
	}
    CQuestScene::ObjectAliasName=new CQuestScene::CglLists();
	CQuestScene::ObjectAliasName->SetSize(1024);
/**/
	/**/
	/*BackBuffer.Create(1024,1024,PBUFFER_2D | PBUFFER_RGB | PBUFFER_32 | PBUFFER_A0 | PBUFFER_DEPTH24);
	{
	 BackBuffer.Enable();
/*	 glClearColor(1,0,0,1);
     glDepthMask(1);
	 glDepthFunc(GL_LEQUAL);
	 glEnable(GL_DEPTH_TEST);
	 glCullFace(GL_FRONT);
     glEnable(GL_TEXTURE_2D);
 	 glEnable(GL_CULL_FACE);	
     glViewport(0,1024-768,1024,768);
/**/
	// BackBuffer.Disable();
	//}

	InitLayer(this);	
    ParseQuestCatalog();
    lua::dostring("quest.currentscene=\"\";");
    lua::dostring("quest.utils={}");
	lua::dostring("quest.hide_color=0");
	lua::dostring("quest.movespeed=1;");
    lua::dostring("quest.z_as_model=1");
    lua::dostring("quest.showallobjects=1");

    QluaINIT();

	lua::dostring("quest.ZFREQ=1");

	ZMaterial=Textures::LoadTexture("Quest/2x2.tga");

	/**/
}



bool SPT=true;

bool CQuestGraph::ReloadScene(LPCSTR FileName,bool Switch)
{
    if(FileName[0]==0 || _stricmp("FileName","null")==0)return LoadScene(FileName,Switch);
	int i=0;
	for(CQScenes::Citerator it=Scenes.begin();it!=Scenes.end();++it,++i)
		if(*it && (*it)->Name==FileName)
		{
			delete *it;
		}
	return LoadScene(FileName,Switch);
}

bool CQuestGraph::LoadScene(LPCSTR FileName,bool Switch)
{
    if(FileName[0]==0 || _stricmp(FileName,"null")==0)
    {
      CurrentScene=0;
      return true;
    }
	int i=0;
	for(CQScenes::Citerator it=Scenes.begin();it!=Scenes.end();++it,++i)
		if(*it && (*it)->Name==FileName)
		{
		  if(Switch)
		  {
			  CQuestGraph::FrameSpan=-2;
			  CurrentScene=i+1;
              lua::doVstring("quest.currentscene=\"%s\";",FileName);
		  }
		  return true;
		}
	CQuestScene *Scene=new CQuestScene();
	bool ret=Scene->Load(FileName);
	if(!ret)
	  delete Scene;
	else
	{
		Print2Console(SYS_QUEST,"loaded scene %s\n",Scene->Name.c_str());
		CQuestGraph::FrameSpan=-2;
		Scenes.push_back(Scene);
		Scene->SetIDPos(Scenes.size());
		if(!CurrentScene || Switch)
        {
			CurrentScene=Scenes.size();
            lua::doVstring("quest.currentscene=\"%s\";",FileName);
        }
	}
	return ret;
}

CQuestScene* CQuestGraph::GetCurrentScene()
{
	if(CurrentScene)
	 return (Scenes[CurrentScene-1]);
	else
	 return NULL;
}

bVector3f  CQuestGraph::ModPosition(const float *V)
{
  if(CurrentScene)
	return GetCurrentScene()->ModPosByFloor(V);
  else
    return V;
}

bVector3f CQuestGraph::AssertPosition(const float *V)
{
	if(!Entry)return V;
	else return Entry->ModPosition(V);
}


CQuestGraph *CQuestGraph::Entry=NULL;
bool CQuestGraph::IsQuestLoaded(){return Entry!=NULL;} 
CQuestGraph* CQuestGraph::MakeEntry(){ Entry=new CQuestGraph();return Entry;}