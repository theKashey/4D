/*===========================================================================
    Заголовок: Controls.cpp
    Автор: kashey
    Информация: работа с некими обьекта. рендер основных элементов
===========================================================================*/
#include "stdafx.h"
#include "GUI.h"

bool CBaseElement::ShowHint(LPCSTR T,const float X,const float Y)
{
	if(!T)T=GetHint();
	bVector3f S(640,480,0);
	bVector4f C(1,1,1,1);
	bVector3f Off(X,Y,0);
	guifaceTEXT(Off,S,0,T,0,C,1);
	return true;
}

void CButton::Render()
{
	//if(!StateTexture) 
	StateTexture=0;
	if(IsFocused())StateTexture=TexID[1];
	//if(IsHolded())StateTexture=TexID[2];
	if(!StateTexture)   StateTexture=TexID[0];
	CBaseElement::Render();
	guifaceRECT(Position,Box,StateTexture,Color,Rep);
	guifaceTEXT(Position,Box,Aligment,Caption,Font,FontColor);
}

void CSwitch::Render()
{
	//if(!StateTexture) 
	StateTexture=0;
	if(IsOn())     StateTexture=TexID[1];
	if(!StateTexture)   StateTexture=TexID[0];
	CBaseElement::Render();
	guifaceRECT(Position,Box,StateTexture,Color,Rep);
	guifaceTEXT(Position,Box,Aligment,Caption,Font,FontColor);
}

int CButton::OnMouseUp     (const float X,const float Y,DWORD &KeyState)
{
	UnlockMouse();
	if(IsMouseIn(X,Y)) 
		OnMouseClick(X,Y,KeyState);
	return 1;
}
int CButton::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
	LockMouse();
	return 1;
}


void CTextArea::Render()
{
	CBaseElement::Render();
	guifaceTEXT(Position,Box,Aligment,Text,Font,Color,FontHeight);
}

void CImage::Render()
{
	CBaseElement::Render();
	guifaceRECT(Position,Box,TextureID,Color,Rep,Offset,Blend);
}

void CImage::SetBlend(const DWORD _Blend)
{ Blend=_Blend;
}

void CScrollBar::Render()
{
	if(sTYPE==SB_HORIZONTAL)
	fPosDELTA_SIZE=(GetMAX()-GetMIN())/Box[0];
	else
	fPosDELTA_SIZE=(GetMAX()-GetMIN())/Box[1];

	CBaseElement::Render();
	bVector3f PositionP=Position;
	if(sTYPE==SB_HORIZONTAL)
	PositionP[0]+=fPosition-5;//*fPosDELTA_SIZE; 
	else
    PositionP[1]+=fPosition-5;//*fPosDELTA_SIZE; 
	bVector3f BoxP;
	if(sTYPE==SB_HORIZONTAL)
	BoxP.Set(10,15,0);
	else
	BoxP.Set(15,10,0);
   if(sTYPE==SB_HORIZONTAL)
    PositionP[1]-=(-Box[1]+BoxP[1])*0.5f;
   else
    PositionP[0]-=(-Box[0]+BoxP[0])*0.5f;
	bVector3f Rep;
	Rep.Set(1,1,1);
	guifaceRECT(Position,Box,TextureIDL,Color,Rep);
	guifaceRECT(PositionP,BoxP,TextureIDP,Color,Rep);
}

int CScrollBar::OnMouseMove   (const float X,const float Y,DWORD &KeyState)
{
	if(Locked)
	{
	 CWindow *PW=(CWindow *)GetParent();
	 bVector3f OL=gui_getoldmouse();
	 bVector3f CL=gui_getcurmouse();
	 if(sTYPE==SB_HORIZONTAL)
	 fPosition+=(CL[0]-OL[0]);
	 else
	 fPosition+=(CL[1]-OL[1]);
	 if(fPosition<0)      fPosition=0;
	 if(sTYPE==SB_HORIZONTAL){
	 if(fPosition>Box[0]) fPosition=Box[0];}else
	 if(fPosition>Box[1]) fPosition=Box[1];
	}
	return 0;
}
int CScrollBar::OnMouseUp     (const float X,const float Y,DWORD &KeyState)
{
   UnlockMouse();
   Locked=false;
   return 0;
}
int CScrollBar::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
   LockMouse();
   Locked=true;
   return 0;
}

void CListBox::AddLine(LPCSTR Line)
{
	CTextArea *Text=new CTextArea();
	Text->SetText(Line);
	Text->SetSize(GetSize()[0]-20,fontheight);
	Text->SetPosition(0,GetNumElements()*fontheight);
	AddElement(Text);
}

void  CListBox::SetMaxSize(const float x,const float y)
{
	MaxSize.Set(x,y,0);
}

void  CListBox::SetMaxSize(const bVector3f &NewSize)
{
	MaxSize=NewSize;
}

void  CListBox::SetAutoSize(int Mode)
{
	AutoSize=Mode;
}




void CListBox::Render()
{
	bVector3f Rep;
	bVector3f bNULL;
	Rep.Set(1,1,1);
	if(AutoSize && Elements.size())//calc size
	{
		bVector3f V=GetSize();
		V[1]=fontheight*Elements.size();
		if(V[1]<MaxSize[1]) this->SetSize(V);
	}
	CScrollBox::RenderPassive();
	guifaceSETRECT(GetPosition(),GetSize(),1);
	guifaceMODRECT(-GetItemOffset(),bNULL);
	int i=-2;
	int Count=0;
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it,++i,++Count)
	{
		if(i>=0)
		{
		 (*it)->SetPosition(0,i*fontheight);
		 (*it)->SetFontHeight(fontheight);
		 if((*it)->IsFocused()){}//guifaceRECT((*it)->GetPosition(),(*it)->GetSize(),13,(*it)->GetColor(),Rep);
		}

		if((*it)->IsVisible()) 
			(*it)->Render();
	}
	guifaceENDRECT();

}

void CScrollBox::RenderPassive()
{
	CActionElement::Render();
	if(fAllowScrollBars==SB_NONE)return;
   // guifaceSETRECT(GetPosition(),GetSize(),1);

	bVector3f sMAX=GetMaxPosition()-GetSize();

	if((fAllowScrollBars & SB_VERTICAL)==SB_VERTICAL && sMAX[1]>0)
	{
		VSlider->SetLineTexture(13);
		VSlider->SetBarTexture(13);
        SetItemOffsetY(VSlider->GetBarPosition());
		VSlider->SetPosition(GetSize()[0]-5+GetItemOffset()[0],GetItemOffset()[1]);
        VSlider->SetSize(5,GetSize()[1]);
		VSlider->SetMAX(sMAX[1]);
	    VSlider->SetVisible(true);
	}
	else
	{ if(VSlider) VSlider->SetVisible(false);
	}
	if((fAllowScrollBars & SB_HORIZONTAL)==SB_HORIZONTAL && sMAX[1]>0)
	{
		HSlider->SetLineTexture(13);
		HSlider->SetBarTexture(13);
        HSlider->SetPosition(GetItemOffset()[0],GetSize()[1]-5+GetItemOffset()[1]);
        HSlider->SetSize(GetSize()[0],5);
		HSlider->SetMAX(sMAX[1]);
		HSlider->SetVisible(true);
	}
	else
	{ if(HSlider) HSlider->SetVisible(false);
	}

    //guifaceSETRECT(BoxStart,BoxEnd,1);
	if(Background)
	{
        guifaceRECT(Position,GetSize(),Background,Color,Rep);
	}
	//guifaceENDRECT();
}


void   CActionElement::DoExport(){} //layer init
CActionElement::CActionElement():CBaseElement()
{
     SetActionResiver(true);
     m_onmove=m_onup=m_ondown=m_onclick=m_onDclick=NULL;
}
	//keyboard
int CActionElement::OnKeyPress(const DWORD Key){return 0;}; // on WM_CHAR
int CActionElement::OnKeyUp   (const DWORD Key){return 0;};	// on WM_UP
int CActionElement::OnKeyDown (const DWORD Key){return 0;};	// on WM_DOWN
//mouse
int CActionElement::OnMouseMove   (const float X,const float Y,DWORD &KeyState){SetMouseDest(X,Y);if(m_onmove  )return m_onmove  (this,X,Y,KeyState);else return 0;};
int CActionElement::OnMouseUp     (const float X,const float Y,DWORD &KeyState){SetMouseDest(X,Y);if(m_onup    )return m_onup    (this,X,Y,KeyState);else return 0;};
int CActionElement::OnMouseDown   (const float X,const float Y,DWORD &KeyState){SetMouseDest(X,Y);if(m_ondown  )return m_ondown  (this,X,Y,KeyState);else return 0;};
int CActionElement::OnMouseClick  (const float X,const float Y,DWORD &KeyState){SetMouseDest(X,Y);if(m_onclick )return m_onclick (this,X,Y,KeyState);else return 0;}; 
//NOT WORKING!!
int CActionElement::OnMouseDClick (const float X,const float Y,DWORD &KeyState){SetMouseDest(X,Y);if(m_onDclick)return m_onDclick(this,X,Y,KeyState);else return 0;};
//drag'n'drop
int CActionElement::OnStartDrag   ()                                     {return 0;}
int CActionElement::OnDragOver    (CBaseElement* Draged,CBaseElement *On){return 0;}
int CActionElement::OnDragEnd     (CBaseElement* Draged,CBaseElement *On){return 0;}

void CActionElement::SetOnClick(mousecallback event){m_onclick=event;}





LPCSTR CGUIContainer::GetName(){return NULL;}
void CGUIContainer::SetItemOffsetX(const float X){ScreenOffset[0]=X;}	//sets offset of element in local space(aka scroll bar)
void CGUIContainer::SetItemOffsetY(const float Y){ScreenOffset[1]=Y;}

bool CGUIContainer::IsSelectSortOn(){return SelectSortOn;}             //auto sort selection
void CGUIContainer::SelectSort(const bool On){SelectSortOn=On;}
const bVector3f CGUIContainer::GetItemOffset(){return ScreenOffset;}

CGUIContainer::CGUIContainer()
{ 
    Active  =NULL;
    Selected=NULL;
	SelectSortOn=false;
}
void CGUIContainer::ContainerAddElement(CBaseElement *Area)
{
	ASSERT(Area);
	ASSERT(Area->GetParent()==NULL);
	Elements.push_back(Area);
}
void CGUIContainer::ContainerAddElementToBottom(CBaseElement *Area)
{
    ASSERT(Area);
	ASSERT(Area->GetParent()==NULL);
	Elements.push_back(Area);
    for(CElementList::iterator it=Elements.end()-2;it!=Elements.begin()+1;--it)
     *(it+1)=*it;
    *(Elements.begin()+2)=Area;
}

void CGUIContainer::Clear()
{
	for(CElementList::iterator it=Elements.begin()+2;it<Elements.end();++it)
	{
		(*it)->Stop();
		delete *it;
	}
	Elements.SetSize(2,false);
}

void CGUIContainer::DeleteElement(const DWORD ID,const bool DoDelete)
{
	if(DoDelete)
		if(Elements[ID]) delete Elements[ID];
	Elements.erasePop(ID);
}

void CGUIContainer::DeleteElement(CBaseElement *I,const bool DoDelete)
{
	int i=0;
    for(CElementList::iterator it=Elements.begin()+2;it<Elements.end();++it,++i)
	{
		if(*it==I){DeleteElement(i,DoDelete);}
	}
}
DWORD CGUIContainer::GetNumElements(){return Elements.size();}
//update
 void CGUIContainer::CascadeUpdate(){};

 CBaseElement* CGUIContainer::GetActiveElement  () { return Active;}
 CBaseElement* CGUIContainer::GetSelectedElement() { return Selected;}



 CButton::CButton():CActionElement()
{
  StateTexture=0;
  ZeroMemory(TexID,sizeof(TexID));
  Rep.Set(1,1,0);
} 
void   CButton::DoExport(){}
void CButton::SetRepeat (const float X,const float Y){ Rep.Set(X,Y,1);}
void CButton::SetCaption(LPCSTR Cap){Caption=Cap;}

void CButton::SetTexture(const DWORD Num,const DWORD ID){TexID[Num]=ID;}


//Switch aka check box
CSwitch::CSwitch():CButton()
{
	On=false;
}
bool CSwitch::IsOn(){return On;}
void CSwitch::SetOn(bool _On)
{
 On=_On;
 if(GetLuaName()!=0)
   lua::doVstring("%s.On=%d",GetLuaName(),On);
}	
int CSwitch::OnMouseClick  (const float X,const float Y,DWORD &KeyState)
{
 SetOn(On?false:true);
 return CButton::OnMouseClick(X,Y,KeyState);
}

void CSwitch::LuaValidate(){SetOn(On);}


//TextArea
CTextArea::CTextArea():CActionElement()
{
	FontHeight=1;
}
void CTextArea::Stop()
{
	Text.Empty();
}
CTextArea::~CTextArea(){Stop();};
void CTextArea::DoExport(){}
void CTextArea::SetText(LPCSTR Tx){Text=Tx;}
void CTextArea::SetFontHeight(const float H){FontHeight=H/20.0f;}

//CImage
//like a CButton


void   CImage::DoExport(){}
CImage::CImage():CActionElement()
{
	TextureID=0;
	Rep.Set(1,1,1);
	Color.Set(1,1,1,1);DoExport();
	Disable();
    Blend=BLEND_NULL;
}
void CImage::SetTexture(DWORD ID){TextureID=ID;}
void CImage::SetRepeat (const float X,const float Y){ Rep.Set(X,Y,1);}
void CImage::SetOffset (const float X,const float Y){ Offset.Set(X,Y,1);}


//ModelImage
//NOT WORKING!

void  CModelImage::DoExport(){}
void  CModelImage::SetModel(const DWORD M){Model=M;}
void  CModelImage::SetState(const float S){State=S;}
float CModelImage::GetState(){return State;}
DWORD CModelImage::GetModel(){return Model;}



void    CScrollBar::SetScrollType(BYTE MODE){sTYPE=MODE;}

void   CScrollBar::DoExport(){}//layer init

void CScrollBar::SetLineTexture(const DWORD T){TextureIDL=T;}
void CScrollBar::SetBarTexture (const DWORD T){TextureIDP=T;}
CScrollBar::CScrollBar():CActionElement()
{
	TextureIDP=TextureIDL=0;
    Locked=false;DoExport();
	SetScrollType(SB_HORIZONTAL);
}

//MIN,MAX,POSITION. all that i need
float CScrollBar::GetMIN()     { return fMIN;}
float CScrollBar::GetMAX()     { return fMAX;}
float CScrollBar::GetBarPosition(){ return (fPosition-GetMIN())*fPosDELTA_SIZE;}

void  CScrollBar::SetMIN        (const float V){ fMIN=V;}
void  CScrollBar::SetMAX        (const float V){ fMAX=V;}
void  CScrollBar::SetBarPosition(const float V){ fPosition=V/fPosDELTA_SIZE+GetMIN();}



//Ok. First normal element
//Allways have 2 scrollbars(hidden) 
//is a window

void    CScrollBox::SetBackground(const DWORD b){Background=b;}
DWORD   CScrollBox::GetBackground(){return Background;} 
//image set
void CScrollBox::SetRepeat (const float X,const float Y){ Rep.Set(X,Y,1);}
void CScrollBox::SetOffset (const float X,const float Y){ Offset.Set(X,Y,1);}

//scrollbar def
LPCSTR CScrollBox::GetName(){return CActionElement::GetName();} 
void    CScrollBox::AllowScrollBars(BYTE MODE){fAllowScrollBars=MODE;}

//position/size tasks
void  CScrollBox::SetPosition(const float X,const float Y,const float Z){BoxStart.Set(X,Y,Z);CActionElement::SetPosition(X,Y,Z);}
void  CScrollBox::SetPosition(const bVector3f &Pos)                     {BoxStart=Pos;CActionElement::SetPosition(Pos);}

void  CScrollBox::SetSize    (const float X,const float Y,const float Z){BoxEnd.Set(X,Y,Z);CActionElement::SetSize(X,Y,Z);}
void  CScrollBox::SetSize    (const bVector3f &Pos)                     {BoxEnd=Pos;CActionElement::SetSize(Pos);}

void CScrollBox::SetSBox(){SetPosition(GetPosition());SetSize(GetSize());}

void CScrollBox::SetBoxStart(const bVector3f &Pos){BoxStart=Pos;}
void CScrollBox::SetBoxEnd  (const bVector3f &Pos){BoxEnd=Pos;}

void CScrollBox::SetBoxStart(const float X,const float Y,const float Z){BoxStart.Set(X,Y,Z);}
void CScrollBox::SetBoxEnd  (const float X,const float Y,const float Z){BoxEnd.Set(X,Y,Z);}


CScrollBox::CScrollBox():CActionElement(),CGUIContainer()
{
	VSlider=NULL;
    HSlider=NULL;
	AllowScrollBars(SB_NONE);
	VSlider=new CScrollBar();AddElement(VSlider);
	HSlider=new CScrollBar();AddElement(HSlider);
	VSlider->SetScrollType(SB_VERTICAL);
	VSlider->SetVisible(false);
	HSlider->SetVisible(false);
	SetBackground(0);
	Rep.Set(1,1,1);
}

//cascade event

bool    CScrollBox::IsShowHint()const
{
	if(Selected)return Selected->IsShowHint();
	return false;
}

LPCSTR  CScrollBox::GetHint()const
{
	if(Selected)return Selected->GetHint();
	return NULL;
}

CMousePointer * CScrollBox::GetMouse()
{
	if(Selected)return Selected->GetMouse();
	else
		return CActionElement::GetMouse();
}



// void   DoExport(){}
int CScrollBox::OnKeyPress(const DWORD Key) {return CGUIContainer::OnKeyPress(Key);}
int CScrollBox::OnKeyUp   (const DWORD Key) {return CGUIContainer::OnKeyUp   (Key);}
int CScrollBox::OnKeyDown (const DWORD Key) {return CGUIContainer::OnKeyDown (Key);}
const float* CScrollBox::GetXYOff(const float X,const float Y){f_XY.Set(X,Y,0);f_XY+=GetItemOffset()-GetPosition();  return f_XY.cfloat();}
int CScrollBox::OnMouseMove   (const float X,const float Y,DWORD &KeyState){const float* XY=GetXYOff(X,Y);SetMouseDest(XY);return CGUIContainer::OnMouseMove  (XY[0],XY[1],KeyState);}
int CScrollBox::OnMouseUp     (const float X,const float Y,DWORD &KeyState){const float* XY=GetXYOff(X,Y);SetMouseDest(XY);return CGUIContainer::OnMouseUp    (XY[0],XY[1],KeyState);}
int CScrollBox::OnMouseDown   (const float X,const float Y,DWORD &KeyState){const float* XY=GetXYOff(X,Y);SetMouseDest(XY);return CGUIContainer::OnMouseDown  (XY[0],XY[1],KeyState);}
int CScrollBox::OnMouseClick  (const float X,const float Y,DWORD &KeyState){const float* XY=GetXYOff(X,Y);SetMouseDest(XY);return CGUIContainer::OnMouseClick (XY[0],XY[1],KeyState);}
int CScrollBox::OnMouseDClick (const float X,const float Y,DWORD &KeyState){const float* XY=GetXYOff(X,Y);SetMouseDest(XY);return CGUIContainer::OnMouseDClick(XY[0],XY[1],KeyState);}


void CScrollBox::AddElement(CBaseElement *Area)
{
	CGUIContainer::ContainerAddElement(Area);
	Area->SetParent(this);
    if(LuaName.GetLength()>0)
    {
      Area->SetLuaName(LuaName+"."+Area->GetName());
      Area->LuaValidate();
    }
}

void CScrollBox::AddElementToBottom(CBaseElement *Area)
{
	CGUIContainer::ContainerAddElementToBottom(Area);
	Area->SetParent(this);
    if(LuaName.GetLength()>0)
    {
      Area->SetLuaName(LuaName+"."+Area->GetName());
      Area->LuaValidate();
    }
}
CBaseElement* CScrollBox::GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp){return CGUIContainer::GetElementUnderMouse(X,Y,Exp);} 



//Listbox is a SAME thing that a scrollbox
//it just have strings(AddLine) and auto convert them to AddElement(new TextArea(text))
//also manage size

CListBox::CListBox():CScrollBox()
{
	BackTexture=0;
	fItemSelected=(DWORD)-1;
	SelectionCallBack=0;
	LastItemSelected=0;
	fontheight=20;
	AutoSize  =0;
}

void  CListBox::SetFontHeight(const float H){fontheight=H;}
float CListBox::GetFontHeight(){return fontheight;}
void  CListBox::SetSelectionCallBack(CListBoxSelectionCallBack CB){SelectionCallBack=CB;}
void  CListBox::DeleteLine(DWORD LineID){DeleteElement(LineID,true);}
void  CListBox::SelectItem(const DWORD Item){fItemSelected=Item;}
DWORD CListBox::SelectedItem(){return fItemSelected;}

int CListBox::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
	SetMouseDest(X,Y);
	int ret=CScrollBox::OnMouseDown(X,Y,KeyState);
	int i=0;
	for(CElementList::iterator it=Elements.begin();it!=Elements.end();++it,++i)
		if((*it)->IsFocused())
		{
			if(SelectionCallBack)SelectionCallBack(this,i);
			break;
		}
	return ret;
}



//window
//scrollbox again.
//nothing new
//style determine caption and window buttons.
//normaly - off

CWindow::CWindow():CScrollBox()
{
	Style=0;
}

void CWindow::SetCaption(LPCSTR Text){Caption=Text;}
LPCSTR CWindow::GetCaption(){return Caption;}

void Render();
void SetStyle(const DWORD Style);

void CWindow::Show()
{
	SetVisible(true);
}

int CWindow::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
	SetMouseDest(X,Y);
	SetBringMode(BRING_ONCE);
	return CScrollBox::OnMouseDown(X,Y,KeyState);
};


//Area
//ScrollBox again :)


CArea::CArea():CWindow()
{
	SelectSort(true);
    SetAsWindow(false);
}
void CArea::AddWindow(CWindow *Wnd){ AddElement(Wnd);}

void CArea::SetAsWindow(bool On){LikeAWin=On;}

//GUI manager


void CGUIManager::AddArea(CArea *Area){ AddElement(Area);}


void  CGUIManager::SetMouseDraw(const bool Draw){DrawMouse=Draw;}
void  CGUIManager::EnableDrawMouse(const bool Enable){DrawMouse     =Enable;}
void  CGUIManager::EnableMouse    (const bool Enable){AsseptMouse   =Enable;}
void  CGUIManager::EnableKeyBoard (const bool Enable){AsseptKeyBoard=Enable;}
void  CGUIManager::EnableActions  (const bool Enable){AsseptActions =Enable;}
void  CGUIManager::EnableFullActions(const bool Enable)
{
    EnableMouse   (Enable);
	EnableKeyBoard(Enable);
	EnableActions (Enable);
}