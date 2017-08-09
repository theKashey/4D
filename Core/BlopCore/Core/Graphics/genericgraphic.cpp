/*===========================================================================
    Заголовок: GenericGraphics.cpp
    Автор: Kashey
    Информация:GenericGraphic and unifited geometry driver

===========================================================================*/

//define this if index can be non VBO object
#define INDEX_CAN_BE_LOCAL 1
//define this if you need to validate writing to VBO data
//#define VBO_NEED_WRITE_VALIDATE


//--------------------------------

#include "ProjectSpace.h"
#include "ProjectGraphic.h"
#include "GenericGraphic.h"
#include "BFont.h"
#include "ProjectParticles.h"


Text::CBFont *SystemFont;

// _CDrawableArray              CGenericGraphic::Objects;
// _CDrawableLayer              CGenericGraphic::Layers;
 DEF_RENDER                 *CGenericGraphic::Driver;
 CCamera                     *CGenericGraphic::Camera;
 COverlay                    *CGenericGraphic::Overlay;

 bool                         CGenericGraphic::GRCollapsing=false;

 CSceneNode                   CGenericGraphic::RootNode;

DWORD     SYS_GRAPHIC   =0;
DWORD     SYS_TEXTURES  =0;
DWORD     SYS_GPUSHADERS=0;

CGenericGraphic *DefGraphic=NULL;

DWORD __GetSysTextures()
{
	if(!SYS_TEXTURES)
     RegisterNewConsoleClass(SYS_TEXTURES,"TEXTURES",-1);
	return SYS_TEXTURES;
}

COverlay * CGenericGraphic::GetOverlay(){return Overlay;}
DEF_RENDER*   CGenericGraphic::GetDriver(){return Driver;}

CGenericGraphic::CGenericGraphic(BlopBaseObject *_Driver):BaseObject(REFC_CORE,REFB_GRAPHIC,"GRAPHIC")
{
	Driver=NULL;
	SessionDepth=0;
	if(!SYS_GRAPHIC   )  RegisterNewConsoleClass(SYS_GRAPHIC,"GRAPHIC",-1);
	if(!SYS_GPUSHADERS)  RegisterNewConsoleClass(SYS_GPUSHADERS,"VSHADER",-1);
	__GetSysTextures();
//	Assign(_Driver);
	Print2Console(SYS_GRAPHIC,"Created");
	if(DefGraphic)
	{
		Print2Console(SYS_GRAPHIC,"WARNING - DUPLICATED GRAPHIC");
		ASSERT(false);
	}
	if(!DefGraphic) DefGraphic=this;
	SystemFont=new Text::CBFont(); 
    //InitLayers();
	ProjectParticles=new CBaseParticleSystem();
	Camera=new CCamera;

	lua::dostring("render={};");
	lua::setdword("render.clearstyle",255);
	//lua::dostring("render.clearstyle=256");
	Overlay=new COverlay();
}

bool  CGenericGraphic::Start()
{
	return Init();
}

bool CGenericGraphic::Init()
{
	//extgl_Initialize();
	SystemFont->Create("Data/Fonts/Impact.bmp",10,10);				//  
	SystemFont->SetSymWidth(0.2f);	
	Overlay->Init();
	return true;
}

CGenericGraphic::~CGenericGraphic(void)
{
   CGenericGraphic::GRCollapsing=true;
   delete SystemFont;
   Print2Console(SYS_GRAPHIC,"Deleted");
}

void    CGenericGraphic::StartSession()
{
	SessionDepth++;
}
void    CGenericGraphic::EndSession  ()
{
	SessionDepth--;
	ASSERT(SessionDepth>=0);
	if(SessionDepth==0)
		SessionCleanUp();
}

CCamera& CGenericGraphic::GetCurrentCamera()
{
	return *DefGraphic->Camera;
}

CSceneNode* CGenericGraphic::GetRoot(){return &RootNode;}

//-----------------------------------------------------------------------------



void DLLTYPE Text::DrawSysText(float x,float y,LPCSTR Text,...)
{
	/*char szError[1024];
	va_list ap;
    va_start(ap, Text);
    vsprintf(szError, Text, ap);
    va_end(ap);
	*/
	DOFORMAT(Err,Text);
	LPCSTR szError=Err;
	SystemFont->DrawText(x,y,10,szError);
}




	bool CSceneNode::IsEnabled (){return Enabled;}
	bool CSceneNode::IsVisible (){return Visibled;}
	bool CSceneNode::NeedUpdate(){return fNeedUpdate>0;}

	void CSceneNode::Hide   (){Visibled=false;}
	void CSceneNode::Show   (){Visibled=false;}

	void CSceneNode::Enable (){Enabled=false;}
	void CSceneNode::Disable(){Enabled=false;}


	void CSceneNode::SetUpdate(){fNeedUpdate=1;}
	void CSceneNode::SetAllwaysUpdate(){fNeedUpdate=2;}


	CSceneNode::CNodeHistory CSceneNode::ConstHostory;
	void CSceneNode::Update(CNodeHistory *H)
	{
	  if(fNeedUpdate==1)fNeedUpdate=0;
	  CNodeHistory *Hist=H;
	  if(!H){H=&ConstHostory;H->clear();}
	  const int inSize=H->size();
	  Hist->push_back(ID);
	  for(CSceneNodes::iterator it =Childs.begin();it!=Childs.end();++it)
		  if((*it))(*it)->Update(H);
	  H->SetSize(inSize,false);
	}

	int CSceneNode::Draw  (CNodeHistory *H)
	{
	  CNodeHistory *Hist=H;
	  if(!H){H=&ConstHostory;H->clear();}
	  const int inSize=H->size();
	  H->push_back(ID);
	  int NDraw=0;
	  for(CSceneNodes::iterator it =Childs.begin();it!=Childs.end();++it)
		  if((*it))NDraw+=(*it)->Draw(H);
	  H->SetSize(inSize,false);
	  return NDraw;
	}

	bool CSceneNode::AddChild   (CSceneNode *Child,const DWORD *Passes)
	{
		if(Collapsing)return false;
		if(Child->Parent)return false;
		Child->Parent=this;
		for(CSceneNodes::iterator it =Childs.begin();it!=Childs.end();++it)
			if(!(*it)){(*it)=Child;return true;}
		Childs.push_back(Child);
		return true;
	}

	void CSceneNode::RemoveChild(CSceneNode *Child,const DWORD *Passes)
	{
		if(Collapsing)return;
		for(CSceneNodes::iterator it =Childs.begin();it!=Childs.end();++it)
			if((*it)==Child)
			{
			 (*it)=NULL;
			 return;
			}
	}

	CSceneNode::CSceneNode()
	{
		Enabled=Visibled=false;
		fNeedUpdate=0;
		Parent=NULL;
		Collapsing=false;
	}
	CSceneNode::~CSceneNode()
	{
		Collapsing=true;
		for(CSceneNodes::iterator it =Childs.begin();it!=Childs.end();++it)
			if((*it))delete *it;
		if(Parent)
			Parent->RemoveChild(this);
	}
