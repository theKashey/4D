#include "stdafx.h"
#include "QuestGUI.h"
#include "QuestScene.h"
#include "projectai.h"

CMousePointer *MouseOpenDoor=NULL;
CMousePointer *MouseGet     =NULL;
CMousePointer *MouseTalk    =NULL;


CMousePointer *MakeMouse(const int M)
{
	switch(M)
	{
	case 0:return NULL;
	case 1:return MouseOpenDoor;
	case 2:return MouseGet;
	case 3:return MouseTalk;
    default: return NULL;
	}
}
            
CBackImage::CBackImage():CImage()
{
	if(!MouseOpenDoor)
	{
		MouseOpenDoor=new CMousePointer();
		MouseOpenDoor->Mouse->SetTexture(GetGUITexture("GUI/mouse/opendoor"));
        MouseOpenDoor->Mouse->SetSize(30,30);
	    MouseOpenDoor->Mouse->SetColor(1,1,1,1);
		MouseOpenDoor->SetOffset(15,15);

		MouseGet     =new CMousePointer();
		MouseGet     ->Mouse->SetTexture(GetGUITexture("GUI/mouse/get"));
        MouseGet     ->Mouse->SetSize(30,30);
	    MouseGet     ->Mouse->SetColor(1,1,1,1);
		MouseGet     ->SetOffset(8,8);

		MouseTalk    =new CMousePointer();
		MouseTalk    ->Mouse->SetTexture(GetGUITexture("GUI/mouse/talk"));
        MouseTalk    ->Mouse->SetSize(30,30);
	    MouseTalk    ->Mouse->SetColor(1,1,1,1);
		MouseTalk    ->SetOffset(15,15);
	}
	SetPosition(0,0);
	SetSize(640,480);
	SetTexture(0);
	Enable();
    SetName("QuestScene");
}
void CBackImage::Render()
{
    bVector3f S(640,480,0);
	bVector4f C(1,1,1,1);
	bVector3f Off(00,100,0);
	guifaceTEXT(Off,S,0,MText,0,C,1);
	Off[1]+=30;
	guifaceTEXT(Off,S,0,MText2,0,C,1);
}

int florpoint=0;
bVector3f florpoints[0xF];

QActiveAdmin *AAdmin;


int BSaveOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	CQuestGraph::FrameSpan=-1;
	CQuestGraph::Entry->GetCurrentScene()->Save();
	return 0;
}

int BAAOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	AAdmin->SetVisible(AAdmin->IsVisible()?0:1);
	return 0;
}

int BSayOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	GString Cmd;
	Cmd.Format("sayTextkam{\nusebot soldir1\nsay \"hm #1\"\n}\n");

	AI::ParseAIScript(Cmd,Cmd.length()); 
	AI::RunScript("sayTextkam");
	return 0;
}

int CBackImage::OnMouseMove(const float X,const float Y,DWORD &KeyState)
{
     static MakeProfile(Qg,"Quest QUI");
     Profile(Qg);

    float rx=CGUIManager::GetMousePosition()[0];
	float ry=CGUIManager::GetMousePosition()[1];

	static float oldx=0;
	static float oldy=0;
    if(lua::getdword("gui.QPanel.Admin.CamFix.On")==0)
    {
	  if(fabs(oldx-rx)>30 || fabs(oldy-ry)>30)  {  oldx=rx;oldy=ry;}
	  if(rx!=oldx || ry!=oldy)World()->Camera()->SetLookAtAdd(-(oldx-rx)/4.0,-(oldy-ry)/4.0,0);
      oldx=rx;oldy=ry;

      int FS,SL;
	  if(lua::getdword("input.iskeyon(\"w\")")) FS=1;
	  else if(lua::getdword("input.iskeyon(\"s\")")) FS=2;
	  else FS=0;

	  	   if(lua::getdword("input.iskeyon(\"a\")")) SL=1;
	  else if(lua::getdword("input.iskeyon(\"d\")")) SL=2;
	  else SL=0;
	  if(SL || FS)
		  World()->Camera()->SetPosAdd(World()->Camera()->GetMoveShift(FS,SL,1)*(lua::getdword("quest.movespeed")+1));
      if(lua::getdword("input.iskeyon(\"q\")"))
          World()->Camera()->SetPosAdd(bVector3f(0, (float)(lua::getdword("quest.movespeed")+1),0));
      if(lua::getdword("input.iskeyon(\"z\")"))
          World()->Camera()->SetPosAdd(bVector3f(0,-(float)(lua::getdword("quest.movespeed")+1),0));
    }


    if(!CQuestGraph::Entry->GetCurrentScene())return 0;
	
	DWORD C=0;
    float rx2=    CGUIManager::GetMousePosition()[0];
	float ry2=480-CGUIManager::GetMousePosition()[1];

	CGUIManager::AdjustFromGUI(&rx2,&ry2,0,0);
    float z;
    static DWORD guiZ=VARS::GetVarByName("qui.sZ").ID;
    z=VARS::fvars(guiZ   );

    DWORD RGBA;
    static DWORD guiRGBA=VARS::GetVarByName("qui.sRGBA").ID;
    RGBA=VARS::fvars(guiRGBA   );

    //RENDER->ReadPixelf(rx2,ry2,1,1,grDEPTH_COMPONENT,z);
   // RENDER->ReadPixel(rx2,ry2,1,1,grRGBA_COMPONENT,RGBA);

	static CQuestActiveElement OldAE;
	CQuestActiveElement AE=CQuestGraph::Entry->GetCurrentScene()->GetActiveByColor(rx2,ry2,RGBA);
	if(!AE.Empty())
     {
      //lua::doVstring("q_selectedsurface=models.assemblers.scene.S[%d]",(RGBA>>24));
      GString Q="<";Q=Q+AE.GetName()+">";
      GString qa;qa.Format("%f-%X",z,RGBA);
	  SetHint(Q+qa);
	  if(!OldAE.Equals(AE)) 
	  {
	    if(!OldAE.Empty())
	     OldAE.OnMouseOut();
		AE.OnMouseIn();
		OldAE=AE;
      }
     }
	 else
	 {
	 GString qa;qa.Format("%f-%X",z,RGBA);
	 SetHint(qa);
	 if(!OldAE.Empty())
		 {
	       OldAE.OnMouseOut();
	       OldAE=AE;
		 }
	 }
	return 1;
}

bVector3f PX;

int CBackImage::OnMouseDown(const float X,const float Y,DWORD &KeyState)
{
	//lua::setdword("gui.QPanel.Admin.CamFix.On",1);
	//return 0;
   if(!CQuestGraph::Entry->GetCurrentScene())return 0;
	
	DWORD C=0;
    float rx2=    CGUIManager::GetMousePosition()[0];
	float ry2=480-CGUIManager::GetMousePosition()[1];

	CGUIManager::AdjustFromGUI(&rx2,&ry2,0,0);
    float z;
    static DWORD guiZ=VARS::GetVarByName("qui.sZ").ID;
    z=VARS::fvars(guiZ   );
	bVector3f SX(rx2,ry2,z);
	bVector3f WX;
	World()->Camera()->ScreenToClient(SX,WX);
	PX=WX;

    DWORD RGBA;
    static DWORD guiRGBA=VARS::GetVarByName("qui.sRGBA").ID;
    RGBA=VARS::fvars(guiRGBA   );
	CQuestActiveElement AE=CQuestGraph::Entry->GetCurrentScene()->GetActiveByColor(rx2,ry2,RGBA);
     {
      lua::doVstring("q_selectedsurface=models.assemblers.scene.S[%d]",(RGBA>>24));
      GString Q="<";Q=Q+lua::getstring("q_selectedsurface")+">";
	  GString qa;qa.Format("%f",z);
	  SetHint(Q+qa);
     }
    return 0;
}


GUIWindow::GUIWindow():CWindow()
{	  
	SetName("QPanel");
  	SetPosition(0,0);
	SetSize(640,480);
	AddElement(new CBackImage()); 
	AddElement(new QAdmin()); 
	AAdmin=new QActiveAdmin();
	AddElement(AAdmin);
	SetCaption("QUEST");
	CGUIManager::GetGUI()->AddElementToBottom(this);
}

QAdmin::QAdmin():CWindow()
{
//    SetAsWindow();
	SetName("Admin");
	SetPosition(0,20);
	SetSize(50,20);
	CSwitch *SW=new CSwitch();
    SW->SetName("ZGridOn");
	SW->SetPosition(0,0);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	AddElement(SW);

	SW=new CSwitch();
    SW->SetName("FlorSel");
	SW->SetPosition(10,0);
	SW->SetSize(10,10);	
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	AddElement(SW);

	SW=new CSwitch();
    SW->SetName("SFloor");
	SW->SetPosition(20,0);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	AddElement(SW);

	SW=new CSwitch();
    SW->SetName("ZGridColor");
	SW->SetPosition(10,10);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	AddElement(SW);

	SW=new CSwitch();
	SW->SetName("ZInit");
	SW->SetPosition(00,10);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	AddElement(SW);

	SW=new CSwitch();
	SW->SetName("CamFix");
	SW->SetPosition(20,10);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	SW->SetOn();
	AddElement(SW);

	SW=new CSwitch();
	SW->SetName("Teleport");
	SW->SetPosition(30,0);
	SW->SetSize(10,10);
	SW->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	SW->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	SW->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	SW->SetShowHint(true);SW->SetHint(SW->GetName());
	SW->SetOn(false);
	AddElement(SW);

	CButton *B=new CButton();
	B->SetName("SaveScene");
	B->SetPosition(30,10);
	B->SetSize(10,10);
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetShowHint(true);B->SetHint(B->GetName());
	B->SetOnClick(BSaveOnClick);
	AddElement(B);

	B=new CButton();
	B->SetName("SaySome");
	B->SetPosition(40,10);
	B->SetSize(10,10);
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetShowHint(true);B->SetHint(B->GetName());
	B->SetOnClick(BSayOnClick);
	AddElement(B);

	B=new CButton();
	B->SetName("OpenActiveWindow");
	B->SetPosition(40,0);
	B->SetSize(10,10);
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetShowHint(true);B->SetHint(B->GetName());
	B->SetOnClick(BAAOnClick);
	AddElement(B);
	
}

class CAList:public CListBox
{
public:
   void Render()
   {
//	   profLB->Clear();
  //     profLB->SetFontHeight(10);
	   CListBox::Render();
   }
};

QActiveAdmin::QActiveAdmin():CWindow()
{
	SetPosition(400,0);
	SetSize(280,400);
	SetVisible(false);
	Enable();
	SetName("QAA");
	SetCaption("Quest active items admin");
	SetStyle(sWS_TRACKABLE);
	SetBackground(GetGUITexture("GFX/WindowBack"));
	/*CImage *Img=new CImage();

	Img->SetPosition(0,0);
	Img->SetSize(GetSize());
	Img->SetTexture(GetGUITexture("GFX/WindowBack"));
	AddElement(Img);*/
}
