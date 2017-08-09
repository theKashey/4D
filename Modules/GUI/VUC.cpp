/*===========================================================================
    Заголовок: VUC.cpp
    Автор: kashey
    Информация: Врапер для гуи. Создание админ интерфейса
	            Рендер профайлера
				Инициализация самого GUI
===========================================================================*/

#include "stdafx.h"
#include "ProjectGraphic.h"
#include "GUI.h"
#include "VUC.h"

CGUIManager *MyGUI=NULL;

DWORD GetGUITexture(LPCSTR Name)
{
	return Textures::LoadTexture(GString(Name)+".tga");
	return 0;
}

bVector3f EXPos;

void MouseTickTime(CBaseElement *Text)
{
	GString T;
	T.Format("X:%d Y:%d B1:%d",(int)MyGUI->GetMousePosition()[0],(int)MyGUI->GetMousePosition()[1],MyGUI->GetMouseKey());
	((CTextArea*)Text)->SetText(T);
}

void AdminWinSetPos   (CBaseElement *Win)
{
	//Win->SetPosition(20,0);
	EXPos.Set(0,30,0);
}

void NextWinSetPos   (CBaseElement *Win)
{
	Win->SetPosition(EXPos[0],EXPos[1]);
	EXPos+=Win->GetSize();
}


void GUI::RenderGUI()
{
	//return;
	//try
	static MakeProfile(guiC,"gui Clear");
	static MakeProfile(guiR,"gui Compile");

	{
		{ Profile(guiC );guifaceClear();}
		{ Profile(guiR );if(MyGUI)MyGUI->Render();}
		
	}
	//catch(...)
	{
		//out->Insert("gui heap break\n");
	}
}

void GUI::GrabMouse()
{
}


CWindow *AdminWin=NULL;



//#include "RenderEntity.h"

CWindow *Win2=NULL;
CWindow *ProfileWin=NULL;
//CRenderEntityD *RE=NULL;
CListBox *profLB;

int BOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	AdminWin->SetVisible(AdminWin->IsVisible()?false:true);
	return 1;
}

int ProfOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	ProfileWin->SetVisible(ProfileWin->IsVisible()?false:true);
	return 1;
}

SmartVector<_Profilers::CFormatedProgress> PVector;

void proftf   (CBaseElement *Win)
{
   static float LastTime=0;
   const float CT=Time::Get();
   if(CT-LastTime>3000 || CT-LastTime<-10)
   {
	   LastTime=CT;
   }else return;
   profLB->Clear();
   profLB->SetFontHeight(10);
   if(PVector._Allocated()==0)PVector.SetSize(100);
   PVector.SetSize(GetFormatedProfileTree(PVector.begin(),PVector._Allocated()),false);
   int i;
   GString Buf;
   char Stride[64];
   for(i=0;i<PVector.size();++i)
   {
     int j=0;
	 for(j=0;j<PVector[i].Offset;++j)Stride[j]=' ';
	 Stride[j]=0;
	 if(PVector[i].UsedPersent*1000>2)
	 {
	  Buf.Format("%s%-20s - %f%%%%",Stride,PVector[i].Name.c_str(),PVector[i].UsedPersent*100.0f);
      profLB->AddLine(Buf);
	 }
   }
   profLB->AddLine("--- end ---");
}


CWindow *REWIN;

int BRSOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	REWIN->SetVisible(REWIN->IsVisible()?false:true);
	return 1;
}

int BR2SOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
//	RE->SetVisible(RE->IsVisible()?false:true);
	return 1;
}


int AISOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	BlopBaseObject* Obj=REF_GetObjectByName("AIv1");
	if(Obj)
	{
	    	CObjectID TT,NULLTT;
			TT=Obj->GetID();
			SendUpSignal(CObjectIDTwin(TT,NULLTT),bCMD_UPDATE,0,(LPVOID)NULL);
	}
	return 1;
}

int AI1SOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	BlopBaseObject* Obj=REF_GetObjectByName("AIv1");
	if(Obj)
	{
	    	CObjectID TT,NULLTT;
			TT=Obj->GetID();
			SendUpSignal(CObjectIDTwin(TT,NULLTT),bCMD_STOP,0,(LPVOID)NULL);
	}
	return 1;
}

int AI2SOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	BlopBaseObject* Obj=REF_GetObjectByName("AIv1");
	if(Obj)
	{
	    	CObjectID TT,NULLTT;
			TT=Obj->GetID();
			SendUpSignal(CObjectIDTwin(TT,NULLTT),bCMD_RESTART,0,(LPVOID)NULL);
	}
	return 1;
}



void BREVERSE(bool &Dst){if(Dst)Dst=false;else Dst=true;}

int BR3SOnClick(CActionElement* Sender,const float X,const float Y,DWORD &KeyState)
{
	if(Sender)
	{
/*		if(Sender->GetNameRef()=="showc")
			BREVERSE(RE->ShowCreatures);
		else
		if(Sender->GetNameRef()=="showl")
			BREVERSE(RE->ShowLights);
		else
		if(Sender->GetNameRef()=="showe")
			BREVERSE(RE->ShowEntitys);
		else
		if(Sender->GetNameRef()=="showt")
			BREVERSE(RE->ShowTalks);
		else
		if(Sender->GetNameRef()=="showw")
			BREVERSE(RE->ShowWaypoints);
		else
		if(Sender->GetNameRef()=="showm")
			BREVERSE(RE->ShowEmiters);
/**/
	}
	return 1;
}


void LBSelCall(CBaseElement *Caller,DWORD ElementID)
{
//	RE->SetVisible(RE->IsVisible()?false:true);
	//switch(ElementID)
	{
	//case 0: RE->SetVisible(~RE->IsVisible());
	}
}

void  GUI::EnableGUIIterations(const int Type)
{
	if(Type)
	{
        MyGUI->EnableFullActions(true);
		MyGUI->EnableDrawMouse  (true);
	}
	else
	{
        MyGUI->EnableFullActions(false);
		MyGUI->EnableDrawMouse  (false);
	}
}

//#include "GamePanel/GamePanel.h"

void GUI::CreateGUI()
{
	MyGUI=new CGUIManager();
    MyGUI->SetMouseDraw(false);

	DefMousePointer.Mouse->SetTexture(GetGUITexture("GFX/2d/mouse"));
    DefMousePointer.Mouse->SetSize(15,15);
	DefMousePointer.Mouse->SetColor(1,1,1,1);

	MyGUI->EnableFullActions(false);
	MyGUI->EnableDrawMouse  (false);


	CArea *Area=new CArea();
	Area->SetName("GameLevel");
	MyGUI->AddArea(Area);

    CImage *Img;
	
	 Area->Disable();

	//return;

    //word area
	Area=new CArea();
	Area->SetName("work");
    MyGUI->AddArea(Area);
//	RE=new CRenderEntityD();
//	RE->SetVisible(false);
//	Area->AddElement(RE);

	

    CButton *B=new CButton();
	B->SetPosition(0,0);
	B->SetSize(20,20);
	B->SetCaption("");// no caption
	B->SetTexture (0,GetGUITexture("GFX/MISC/openadmin"));
	B->SetOnClick(BOnClick);
	B->SetHint("Open Admin window");
	B->SetShowHint(true);
	Area->AddElement(B);

	CWindow *Win=new CWindow();
	Win->SetCaption("survival configuration track bar");
	Win->SetTickFunc(AdminWinSetPos);
	Win->SetName("AdminWindow");
	Win->SetSize(620,20);
	Win->SetPosition(20,00);
	//Win->SetStyle(sWS_TRACKABLE);// | sWS_DIALOG);
	Win->SetVisible(false);
	Area->AddWindow(Win);
	AdminWin=Win;

	Img=new CImage();
	Img->SetPosition(0,0);
	Img->SetSize(620,20);
	Img->SetColor(1,1,1,0.5f);
	Img->SetRepeat(1,1);
	Img->SetTexture(GetGUITexture("GFX/WindowBack"));
	Img->SetAlign(ALIGN_CLIENT);
	Img->Disable();
	Win->AddElement(Img);

	B=new CButton();
	B->SetPosition(0,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("RS");
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BRSOnClick);
	Win->AddElement(B);

	B=new CButton();
	B->SetPosition(30,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("aiRUN");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(AISOnClick);
	Win->AddElement(B);

	B=new CButton();
	B->SetPosition(70,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("aiSTOP");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(AI1SOnClick);
	Win->AddElement(B);

	B=new CButton();
	B->SetPosition(100,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("aiTOUCH");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(AI2SOnClick);
	Win->AddElement(B);

	B=new CButton();
	B->SetPosition(140,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("aiLINKW");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
//	B->SetOnClick(AIW3SOnClick);
	Win->AddElement(B);

	B=new CButton();
	B->SetPosition(180,0);
	B->SetSize(20,20);
	B->SetShowHint(true);B->SetHint("PROFILER");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(ProfOnClick);
	Win->AddElement(B);

	//------------------------------------------------------
	//------------------------------------------------------
	//------------------------------------------------------

	CWindow *Win2=new CWindow();
	Win2->SetCaption("RE SET");
	Win2->SetTickFunc(NextWinSetPos);
	Win2->SetName("REwindow");
	Win2->SetSize(640,20);
	Win2->SetPosition(20,80);
	//Win->SetStyle(sWS_TRACKABLE);// | sWS_DIALOG);
	Win2->SetVisible(false);
	Area->AddWindow(Win2);
	REWIN=Win2;
	B=new CButton();
	B->SetPosition(0,0);
	B->SetSize(40,20);
	B->SetCaption("SHOW");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR2SOnClick);
	Win2->AddElement(B);
	B=new CButton();
	B->SetName("showc");
	B->SetPosition(40,0);
	B->SetSize(40,20);
	B->SetCaption("C");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);
	B=new CButton();
	B->SetName("showe");
	B->SetPosition(80,0);
	B->SetSize(40,20);
	B->SetCaption("E");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);	
	B=new CButton();
	B->SetName("showl");
	B->SetPosition(120,0);
	B->SetSize(40,20);
	B->SetCaption("L");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);
	B=new CButton();
	B->SetName("showt");
	B->SetPosition(160,0);
	B->SetSize(40,20);
	B->SetCaption("TLK");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);
	B=new CButton();
	B->SetName("showw");
	B->SetPosition(200,0);
	B->SetSize(40,20);
	B->SetCaption("WAY");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);
	B=new CButton();
	B->SetName("showm");
    B->SetPosition(240,0);
	B->SetSize(40,20);
	B->SetCaption("EM");// no caption
	B->SetTexture (0,GetGUITexture("GFX/Buttons/B1"));
	B->SetTexture (1,GetGUITexture("GFX/Buttons/B2"));
	B->SetTexture (2,GetGUITexture("GFX/Buttons/B3"));
	B->SetOnClick(BR3SOnClick);
	Win2->AddElement(B);

	ProfileWin=new CWindow();
	ProfileWin->SetCaption("Profile");
	ProfileWin->SetTickFunc(NextWinSetPos);
	ProfileWin->SetName("Profile");
	ProfileWin->SetSize(640,400);
	ProfileWin->SetPosition(20,80);
	ProfileWin->SetVisible(false);
	ProfileWin->SetTickFunc(proftf);
	Area->AddWindow(ProfileWin);

	profLB=new CListBox();
	profLB->SetColor(0,0,1,0.6f);
	profLB->SetSize(240,380);
	profLB->SetPosition(400,20);
	profLB->AllowScrollBars(SB_BOTH);
	profLB->SetBackground(GetGUITexture("GFX/WindowBack"));
	profLB->SetColor(1,1,1,0.5f);
	profLB->SetMaxSize(profLB->GetSize());
	profLB->SetAutoSize(true);
	//profLB->

	ProfileWin->AddElement(profLB);


	EnableGUIIterations(true);
}

void GUI::StoreGUI(LPCSTR FileName)
{
 GString gFileName="System/GUI/";
 gFileName+=FileName;
 GString Out;
// if(MyGUI)  MyGUI->Store(&Out);

 FILE *f;
 fopen_s(&f,gFileName,"w+");
 fwrite(Out.c_str(),1,Out.length(),f);
 fclose(f);
}