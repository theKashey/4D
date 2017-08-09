/*===========================================================================
    Заголовок: GUI.cpp
    Автор: kashey
    Информация: главный редиректор гуи. рендер конвеера, спуск эвентов по конвееру
===========================================================================*/

#include "stdafx.h"
#include "GUI.h"
#include "projectgraphic.h"


CMousePointer *CBaseGUI::GetMouse(){return Mouse;}
void CBaseGUI::SetMouse(CMousePointer *M){Mouse=M;}
CBaseGUI::CBaseGUI()
{
  Visible=true;               //visible
  ActionResiver=false;		   //not a action resiver
  SetMouse(&DefMousePointer); //and store basic mouse
}
CBaseGUI::~CBaseGUI()
{
  Stop();
};

void           CBaseGUI::SetLuaName(LPCSTR N)
{
   ASSERT("GUI : Lua Name SET!" && LuaName.GetLength()<1);
   LuaName=N;
   lua::doVstring("%s={}",N);
}
LPCSTR         CBaseGUI::GetLuaName(){return LuaName;}

void CBaseGUI::LuaValidate()
{
}
void CBaseGUI::LuaExport()
{
}


void CBaseGUI::Stop(){};  //destructor wrapper

bool CBaseGUI::IsVisible      (){return Visible;}
bool CBaseGUI::IsActionResiver(){return ActionResiver;}

bool CBaseGUI::SetVisible      (const bool V){ return Visible=V;}
bool CBaseGUI::TogleVisible    (){if(Visible)Visible=false;else Visible=true;return Visible;}
bool CBaseGUI::SetActionResiver(const bool A){ return ActionResiver=A;}

CBaseElement::CBaseElement():CBaseGUI()
{
		 Parent      =NULL;
         Enabled     =true;          
		 ElementAling=ALIGN_NONE;
		 TickFunc    =NULL;
		 Color.Set(1,1,1,1);
		 SetShowHint(false);
		 SetName(DEF_UNNAMED);
		 DoExport();
		 SetBringMode(BRING_NONE);
}

bool  CBaseElement::IsMouseIn(const float X,const float Y)
{
		bVector3f M(X,Y,1);
		bVector3f Delta=M-GetPosition();
		if(Delta[0]<0 || Delta[1]<0)return false;
		Delta-=GetSize();
		if(Delta[0]<0 && Delta[1]<0) return 1;
		   else                      return 0;
}

void  CBaseElement::Move       (const float X,const float Y,const float Z)
{
		Position[0]+=X;
		Position[1]+=Y;
		Position[2]+=Z;
}

bool    CBaseElement::IsShowHint ()const        {return fShowHint;}
void    CBaseElement::SetShowHint(const bool S) {   fShowHint=S;}
void    CBaseElement::SetHint    (LPCSTR H)     {Hint=H;}
LPCSTR  CBaseElement::GetHint    ()const        {return Hint;}
//bool    CBaseElement::ShowHint   (LPCSTR T,const float X,const float Y); 

//name
void    CBaseElement::SetName   (LPCSTR Name) 
{
    if(strstr(Name," "))
	{  ASSERT("GUI Object element name contain space" && FALSE);}
    fComponentName=Name;
    SetHint(Name);
   // SetShowHint(true);
}
LPCSTR  CBaseElement::GetName   ()const        {return fComponentName;}
const   GString &CBaseElement::GetNameRef()             {return fComponentName;}
//font
void CBaseElement::SetFontHeight(const float H){};

//parent
CBaseElement *CBaseElement::GetParent()               {return Parent;}
void          CBaseElement::SetParent(CBaseElement *P){Parent=P;}

//tick
void  CBaseElement::SetTickFunc(BaseElemTickFunc f){TickFunc=f;}

//props
bool  CBaseElement::IsEnabled   ()const{return Enabled;}
bool  CBaseElement::IsFocused   ()const{return Focused;}
void  CBaseElement::SetFocus    (const bool F){Focused=F;if(!F)OnFocusLost();}
BYTE  CBaseElement::GetBringMode(){if(BringMode==BRING_ONCE){BringMode=BRING_NONE;return 1;}else return BringMode;}
void  CBaseElement::SetBringMode(BYTE T){BringMode=T;}

//on focus lost event
void CBaseElement::OnFocusLost(){};

//position and size
void  CBaseElement::SetPosition(const float X,const float Y,const float Z){Position.Set(X,Y,Z);}
void  CBaseElement::SetPosition(const bVector3f &Pos)                       {Position=Pos;}
const bVector3f &CBaseElement::GetPosition()      {return Position;}
bVector3f  CBaseElement::GetPosition()const {return Position;}
bVector3f &CBaseElement::GetPosRef  ()      {return Position;}

void  CBaseElement::SetSize    (const float X,const float Y,const float Z){Box     .Set(X,Y,Z);}
void  CBaseElement::SetSize    (const bVector3f &Pos)                     {Box     =Pos;}
const bVector3f &CBaseElement::GetSize    ()     {return Box;}
bVector3f  CBaseElement::GetSize    ()const{return Box;}
bVector3f &CBaseElement::GetSizeRef ()     {return Box;}

//enable
void  CBaseElement::Enable   (){Enabled=true;}
void  CBaseElement::Disable  (){Enabled=false;}

void  CBaseElement::SetAlign(const BYTE A){ElementAling=A;}
BYTE  CBaseElement::GetAlign()            {return ElementAling;}

void CBaseElement::SetColor(const float R,const float G,const float B,const float A){ Color.Set(R,G,B,A);}
const bVector4f &CBaseElement::GetColor(){return Color;}
//render just ticks
void CBaseElement::Render(){if(TickFunc) TickFunc(this);};
CBaseElement* CBaseElement::GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp){if(Exp!=this)return this;else return NULL;}







CGUIManager* CGUIManager::GUI=NULL;

CMousePointer  DefMousePointer;

CGUIManager::CGUIManager()
{
	GUI=this;
    //lua::dostring("gui={}");
    SetLuaName("gui");
}
CMousePointer::CMousePointer()
{
	Mouse=new CImage();
    SetOffset(0,0);
    Blend=BLEND_ALPHA;
}

void CMousePointer::SetBlend(const DWORD B){Blend=B;Mouse->SetBlend(Blend);}

CMousePointer::~CMousePointer()
{
	if(Mouse)delete Mouse;
}
void CMousePointer::SetOffset(BYTE x,BYTE y)
{
	Off[0]=x;
	Off[1]=y;
}
void CMousePointer::Render()
{
	Mouse->Render();
}
bVector3f CMousePointer::GetSize()
{
	return Mouse->GetSize()-Off;
}
void CMousePointer::SetPosition(const bVector3f &P)
{
	Mouse->SetPosition(P-Off+bVector3f(1,1,0));
}

CMousePointer &CMousePointer::GetDefMousePointer()
{
	return DefMousePointer;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*int OnMouseMove   (const float X,const float Y,DWORD &KeyState){return 0;}
int OnMouseUp     (const float X,const float Y,DWORD &KeyState){return 0;}
int OnMouseDown   (const float X,const float Y,DWORD &KeyState){return 0;}
int OnMouseClick  (const float X,const float Y,DWORD &KeyState){return 0;} 
int OnMouseDClick (const float X,const float Y,DWORD &KeyState){return 0;}*/

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*************************CScrollBox**********************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

bVector3f CGUIManager::targMousePosition;
bVector3f CGUIManager::MousePosition;
bVector3f CGUIManager::Mouse3D;
void CActionElement::SetMouseDest(const float *XYZ)
{
    CGUIManager::targMousePosition=XYZ;
}

void CActionElement::SetMouseDest(const float X,const float Y)
{
	CGUIManager::targMousePosition.Set(X,Y,0);
}

void CScrollBox::LuaValidate()
{
  if(LuaName.GetLength()>0)
   for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
       if((*it)->GetLuaName()==0)
        {
         (*it)->SetLuaName(LuaName+"."+(*it)->GetName());
         (*it)->LuaValidate();
        }

}

void CScrollBox::CascadeUpdate()
{
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
	{
		bVector3f NP;
		if((*it)->IsVisible())
		 switch((*it)->GetAlign())
		{
			case ALIGN_NONE    : 
				break;
			case ALIGN_CLIENT  :  
				(*it)->SetPosition(0,0);
				(*it)->SetSize(GetSize());
				break;
			case ALIGN_LEFT    : 
				(*it)->SetPosition(0,(*it)->GetPosition()[1]);
				break;
			case ALIGN_RIGHT   : 
				(*it)->SetPosition(GetSize()[0]-(*it)->GetSize()[0],(*it)->GetPosition()[1]);
				break;
			case ALIGN_TOP     : 
				(*it)->SetPosition((*it)->GetPosition()[0],0);
				break;
			case ALIGN_BOTTOM  : 
				(*it)->SetPosition((*it)->GetPosition()[0],GetSize()[1]-(*it)->GetSize()[1]);
				break;
			case ALIGN_CENTER  :
				NP=(GetSize()-(*it)->GetSize())*0.5f;
				(*it)->SetPosition(NP);
				break;



			case ALIGN_TOP_CLIENT     :   //align to parent top and width
				(*it)->SetPosition((*it)->GetPosition()[0],0);
				(*it)->GetSizeRef()[0]=GetSize()[0];
				break;
			case ALIGN_BOTTOM_CLIENT  :   //align to parent bottom and width
				(*it)->SetPosition((*it)->GetPosition()[0],GetSize()[1]-(*it)->GetSize()[1]);
				(*it)->GetSizeRef()[0]=GetSize()[0];
				break;
			case ALIGN_LEFT_CLIENT    :   //align to parent left and height
				(*it)->SetPosition(0,(*it)->GetPosition()[1]);
				(*it)->GetSizeRef()[1]=GetSize()[1];
				break;
			case ALIGN_RIGHT_CLIENT   :  //align to parent right and height
				(*it)->SetPosition(GetSize()[0]-(*it)->GetSize()[0],(*it)->GetPosition()[1]);
				(*it)->GetSizeRef()[1]=GetSize()[1];
				break;
		}
	}
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*************************CGUIContainer*******************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

int CGUIContainer::OnKeyPress(const DWORD Key)
{
	if(Active)return Active->OnKeyPress(Key);
	else      return 0;
}
int CGUIContainer::OnKeyUp   (const DWORD Key)
{
	if(Active)return Active->OnKeyUp(Key);
	else      return 0;
}
int CGUIContainer::OnKeyDown (const DWORD Key)
{
	if(Active)return Active->OnKeyDown(Key);
	else      return 0;
}

int CGUIContainer::OnMouseMove   (const float X,const float Y,DWORD &KeyState)
{
	if(GetControlUnderMouse(X,Y)!=2) return 0;CActionElement::SetMouseDest(X-Active->GetPosition()[0],Y-Active->GetPosition()[1]);
	return Active->OnMouseMove(X,Y,KeyState);
}
int CGUIContainer::OnMouseUp     (const float X,const float Y,DWORD &KeyState)
{
	if(GetControlUnderMouse(X,Y)!=2) return 0;CActionElement::SetMouseDest(X-Active->GetPosition()[0],Y-Active->GetPosition()[1]);
	return Active->OnMouseUp(X,Y,KeyState);
}
int CGUIContainer::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
	if(GetControlUnderMouse(X,Y)!=2) return 0;CActionElement::SetMouseDest(X-Active->GetPosition()[0],Y-Active->GetPosition()[1]);
	return Active->OnMouseDown(X,Y,KeyState);
}
int CGUIContainer::OnMouseClick  (const float X,const float Y,DWORD &KeyState)
{
	if(GetControlUnderMouse(X,Y)!=2) return 0;CActionElement::SetMouseDest(X-Active->GetPosition()[0],Y-Active->GetPosition()[1]);
	return Active->OnMouseClick(X,Y,KeyState);
}
int CGUIContainer::OnMouseDClick (const float X,const float Y,DWORD &KeyState)
{
	if(GetControlUnderMouse(X,Y)!=2) return 0;CActionElement::SetMouseDest(X-Active->GetPosition()[0],Y-Active->GetPosition()[1]);
	return Active->OnMouseDClick(X,Y,KeyState);
}

void  CGUIContainer::MoveElementToTop(CBaseElement *Elem)
{
  for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
	  if(*it==Elem)
	  {
       MoveElementToTop(it);
	   return;
	  }
}

void  CGUIContainer::MoveElementToTop(CElementList::iterator _it)
{
	CBaseElement* mIT=*_it;
    CElementList::iterator it =Elements.end()-1;
	if(_it==it)return;
	CElementList::iterator end=Elements.begin();
	CElementList::iterator i;
	for(i=_it;i!=it;++i)
	 *(i)=*(i+1);
    *(Elements.end()-1)=mIT;
}

void  CGUIContainer::MoveElementToBottom(CElementList::iterator _it)
{
    ASSERT(FALSE && "NOT SUPPORTED");
	CBaseElement* mIT=*_it;
    CElementList::iterator it =Elements.begin();
	if(_it==it)return;
}

DWORD CGUIContainer::GetControlUnderMouse(const float X,const float Y)
{
	if(IsMouseLocked())		//some element lock control
	{
		if(Active)   return 2;
		if(Selected) return 1;
		return 0;
	}
   int sel=0;
   //for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
   for(CElementList::iterator it=Elements.end()-1;it>=Elements.begin();--it)
   {
	   if((*it)->IsVisible())
	   if((*it)->IsEnabled())
	   if((*it)->IsMouseIn(X,Y))
	   {
		   sel=1;
		   if(Selected && Selected!=*it)
		   	   Selected->SetFocus(false);
		   Selected=*it;
		   Selected->SetFocus(true);
		   if((*it)->IsActionResiver())
			   Active=(CActionElement*)(*it);
		   else Active=NULL;
		   if(IsSelectSortOn())
			   if((*it)->GetBringMode())
			   MoveElementToTop(it);
		   if(Active)
			   return 2;else return 1;
	   }
   }
   if(!sel)
	   if(Selected)
	   {
		   	   Selected->SetFocus(false);
			   Selected=NULL;
	   }
   return 0;
}

CBaseElement* CGUIContainer::GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp)
{
   int sel=0;
   //for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
   for(CElementList::iterator it=Elements.end()-1;it>=Elements.begin();--it)
   {
	   if((*it)!=Exp)
	   if((*it)->IsVisible())
	   if((*it)->IsEnabled())
	   if((*it)->IsMouseIn(X,Y))
	   {
		   return (*it)->GetElementUnderMouse(X,Y,Exp);
	   }
   }
   return 0;
}

bVector3f CGUIContainer::GetMaxPosition()
{
   bVector3f MAX;
   for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
   {
	   bVector3f m=(*it)->GetPosition()+(*it)->GetSize();
	   for(int j=0;j<3;++j)	   if(m[j]>MAX[j])MAX[j]=m[j];
   }
   return MAX;
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*************************CGUIManager*********************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

float Rez640    =0;
float backRez640=0;
float Rez480    =0;
float backRez480=0;

void CGUIManager::Render()
{
    static MakeProfile(gr1,"gui:DoUserCtrl");
    static MakeProfile(gr2,"gui:CascadeUpdate");

    static MakeProfile(pg1,"gui:convert");
    static MakeProfile(pg2,"gui:Render");

        {Profile(pg1);
    backRez640 =640.0f/World()->Camera()->GetWidth();
    backRez480 =480.0f/World()->Camera()->GetHeight();

    Rez640     =World()->Camera()->GetWidth ()/640.0f;
    Rez480     =World()->Camera()->GetHeight()/480.0f;
        }
    {Profile(gr1);DoUserCtrl();}
    {Profile(gr2);CascadeUpdate();}
	if(RENDER->GetOverlay()->IsSkipingFrame())return;
 
    
    {
    Profile(pg2);
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
	{
		if((*it)->IsVisible()) (*it)->Render();
        }        
	RenderMouse();
	{	static MakeProfile(guiRF,"gui::DropRender"); Profile(guiRF);guifaceRENDER();}

    }
}

float constmouse_x=320;
float constmouse_y=240;



static Draw::CVBOElement *MyReadPBO=NULL;

static bool gui_read_this_frame=false;

void gui_ReadPixels(DWORD Tag)
{
    static DWORD cxx=0;
	static DWORD fv_FSPAN_d_20=VARS::GetVarByName("Frame.Span20").ID;
	++cxx;

	if(cxx>VARS::fvars(fv_FSPAN_d_20))cxx=0;
	//else 
		return;
    static Draw::CVBOElement ReadPBO(VBO_DYNAMIC_GRABER);
    MyReadPBO=&ReadPBO;
    static bool ReadPBOInited=false;
    if(!ReadPBOInited)
    {
     ReadPBO.SetType(VBO_DYNAMIC_GRABER);
     ReadPBO.SetSize(4+4);
     ReadPBOInited=true;
    }
    float rx2=    gui_getoldmouse()[0];
    float ry2=480-gui_getoldmouse()[1];
    CGUIManager::AdjustFromGUI(&rx2,&ry2,0,0);

    ReadPBO.Bind();
    RENDER->ReadPixelf(rx2,ry2,1,1,grDEPTH_COMPONENT,(float*)ReadPBO.BufferOffset(0));
    DWORD RGBA=0;
    RENDER->ReadPixel(rx2,ry2,1,1,grRGBA_COMPONENT,(DWORD*)ReadPBO.BufferOffset(4));
    ReadPBO.Unbind();
 }

 void gui_AfterReadPixels(DWORD Tag)
  {
    if(!MyReadPBO)return;
	if(!gui_read_this_frame)return;
	gui_read_this_frame=false;
    DWORD *Data=(DWORD*)MyReadPBO->Lock();
    float z=*(float*)Data;
    DWORD RGBA=(DWORD)*(Data+1);
    MyReadPBO->Unlock();
	if(z<0 || z>1)z=0;
    static DWORD guiZ   =0;
    static DWORD guiRGBA=0;
    if(!guiZ   ) REGFVAR(guiZ         ,"qui.sZ");
    if(!guiRGBA) REGFVAR(guiRGBA      ,"qui.sRGBA");
    VARS::fvars(guiZ   )=z;
    VARS::fvars(guiRGBA)=RGBA;
}

void CGUIManager::DoUserCtrl()
{
    static bool readREG=false;
    if(!readREG)
     {
      RENDER->RegisterCallBack_RightBeforeSwap(grRightBeforeSwapIntrStruct(gui_ReadPixels,0));
      RENDER->RegisterCallBack_RightAfterSwap(grRightBeforeSwapIntrStruct(gui_AfterReadPixels,0));
      readREG=true;
     }

     static MakeProfile(gr1,"lua");
     static MakeProfile(grMD,"MouseDown");
     static MakeProfile(grMU,"MouseUp");
     static MakeProfile(grMM,"MouseMove");
    if(AsseptActions)
	{
		if(AsseptMouse)
		{
		  float x;
		  float y;
          gui_setoldmouse(MousePosition);
		  DWORD KeyState;
              {Profile(gr1);
		  x=lua::getfloat("input.mouse_x640");
          y=lua::getfloat("input.mouse_y480");

		  KeyState=lua::getdword("input.mouse_button1");          
              }
	      MousePosition[0]=x;
	      MousePosition[1]=y;


		  gui_setcurmouse(MousePosition);
          SetMouseDest(x,y);
             
		  if(GetControlUnderMouse(x,y)==2)
		  {
			  switch(KeyState)
			  {
                  case 0: if(MouseKeyState) {Profile(grMU);Active->OnMouseUp  (x,y,KeyState);   }
                          else              {Profile(grMM);Active->OnMouseMove(x,y,KeyState);}
					   break;
               case 1: if(!MouseKeyState)
                                            {Profile(grMD);Active->OnMouseDown (x,y,KeyState); }
                       else                 {Profile(grMM);Active->OnMouseMove (x,y,KeyState);}
					   break;
			  }
		  }
		  MouseKeyState   =KeyState;
		}
	}
}


void CGUIManager::RenderMouse()
{
   if(DrawMouse)
   {
	guiClearDepthStack();
	CMousePointer *Mouse=&DefMousePointer;
	if(Active)
		Mouse=Active->GetMouse();
	if(!Mouse)
		Mouse=&DefMousePointer;
	Mouse->SetPosition(MousePosition);
	Mouse->Mouse->SetColor(1,1,1,0.99f);
	Mouse->Render();

	if(Active)// && Active->IsShowHint())
	{
		bVector3f Off=MousePosition+Mouse->GetSize();
		Active->ShowHint(NULL,Off[0],Off[1]);
	}

	float rx=MousePosition[0];
    float ry=480-MousePosition[1];
	float z=0;
	bVector3f pos1(rx,ry,z);
	bVector3f pos;
   }
}

CGUIManager* CGUIManager::GetGUI()
{
	if(!GUI)GUI=new CGUIManager();
	return GUI;
}

const bVector3f &CGUIManager::GetMousePosition(){return MousePosition;}
const bVector3f &CGUIManager::GetTargPosition (){return targMousePosition;};
const bVector3f &CGUIManager::Get3DPosition   (){return Mouse3D;}

DWORD CGUIManager::GetMouseKey(){return MouseKeyState;}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*******************************CArea*********************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

void CArea::Render()
{
    if(LikeAWin)
    {
       CWindow::Render();
       return;
    }
	CScrollBox::Render();
	CascadeUpdate();
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
	{
		if((*it)->IsVisible())
		{
			(*it)->Render();
			guiClearDepthStack();
		}
	}
}



bool CArea::IsMouseIn(const float X,const float Y)
{
  return GetControlUnderMouse(X,Y)!=0;
}

//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*****************************CWindow*********************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

void CWindow::Render()
{
	CScrollBox::Render();
    CascadeUpdate();
	guifaceSETRECT(BoxStart,BoxEnd,1);
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it)
	{
		if((*it)->IsVisible()) (*it)->Render();
	}
	guifaceENDRECT();
}

#include "WinControls\WinControls.h"

void CWindow::SetStyle(const DWORD NewStyle)
{
	if(Style) return;


    if((NewStyle & sWS_DIALOG)==sWS_DIALOG)
	{
       CButton *B=new CFormCloser();
       B->SetCaption("X");
	   B->SetSize(20,20);
	   B->SetPosition(0,0);
	   B->SetAlign(ALIGN_RIGHT);
	   AddElement(B);
	}

	if((NewStyle & sWS_TRACKABLE)==sWS_TRACKABLE)
	{
		CTextArea *Text=new CWindowCaption();
		Text->SetText(GetCaption());
		Text->SetSize(200,10);
		Text->SetAlign(ALIGN_TOP_CLIENT);
		AddElement(Text);
		//Text->SetOnMouseDown(WindowTrackerDown);
		//Text->SetOnMouseMove(WindowTrackerMove);
		//Text->SetOnMouseUp  (WindowTrackerUp);
	}

	Style=NewStyle;
}


void CGUIManager::AdjustFromGUI(float *x,float *y,float *w,float *h)
{
  if(x)*x=(*x)*Rez640;if(y)*y=(*y)*Rez480;
  if(w)*w=(*w)*Rez640;if(h)*h=(*h)*Rez480;
}
void CGUIManager::AdjustToGUI(float *x,float *y,float *w,float *h)
{
  if(x)*x=(*x)*backRez640;if(y)*y=(*y)*backRez480;
  if(w)*w=(*w)*backRez640;if(h)*h=(*h)*backRez480;
}
