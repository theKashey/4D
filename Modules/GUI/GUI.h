#ifndef _CGUI_H_
#define _CGUI_H_

//ALIGN

#define ALIGN_NONE           0  //no align
#define ALIGN_CLIENT         1   //align to whole parent space
#define ALIGN_LEFT           2   //align to left parent edge
#define ALIGN_RIGHT          3   //align to right parent edge
#define ALIGN_TOP            4   //align to top parent edge 
#define ALIGN_BOTTOM         5   //align to bottom parent edge
#define ALIGN_CENTER         6   //align to center of parent
#define ALIGN_TOP_CLIENT     7   //align to parent top and width
#define ALIGN_BOTTOM_CLIENT  8   //align to parent bottom and width
#define ALIGN_LEFT_CLIENT    9   //align to parent left and height
#define ALIGN_RIGHT_CLIENT   10  //align to parent right and height

//WINDOW STYLES

#define sWS_NULL               0  // null style
#define sWS_TRACKABLE          1  // window can be moved
#define sWS_SIZEABLE           2  // can be resized
#define sWS_DIALOG             3  // have close button
#define sWS_NON_RECTANGLE      4  // is not rectangle
#define sWS_SINGLE             5  // have just a caption
#define sWS_CLEAR              6  // clear window




class CImage;

//Mouse pointer
//Define mouse as image and offset of window(x,y) and mouse pos

class DLLTYPE CMousePointer
{
	bVector3f Off;
    DWORD     Blend;
public:
	CImage  *Mouse;
	CMousePointer();
	~CMousePointer();
	void SetOffset(BYTE x,BYTE y);
	void Render();
	bVector3f GetSize();
	void SetPosition(const bVector3f &P);
	static CMousePointer &GetDefMousePointer();
    void SetBlend(const DWORD B);
};

extern CMousePointer  DefMousePointer;

//base gui class

class DLLTYPE CBaseGUI // base element
{
protected:
   bool Visible;                //is visible
   bool ActionResiver;			//is action element(to remove RTCI)
   CMousePointer *Mouse;		//this element mouse
   GString        LuaName;
public: 
   virtual void   SetLuaName(LPCSTR N);
   LPCSTR         GetLuaName();
   virtual        void LuaValidate();
   virtual        void LuaExport();
   //mouse operations
   virtual CMousePointer *GetMouse();
   void SetMouse(CMousePointer *M);
   CBaseGUI();
   virtual ~CBaseGUI();
   virtual void Stop();  //destructor wrapper

   //propertys
   bool IsVisible      ();
   bool IsActionResiver();

   bool SetVisible      (const bool V);
   bool TogleVisible    ();
   bool SetActionResiver(const bool A);
};


class CBaseElement;


//define of tick func(tick performed in render)
typedef void(*BaseElemTickFunc)(CBaseElement *BE);

//Then element become focused it can
#define BRING_ONCE 1    //move to top, but once
#define BRING_NONE 0	//never moves
#define BRING_ALLWAYS 2	//allways moves

#define DEF_UNNAMED "unnamed" // default component name

//base element . layer 2

class DLLTYPE CBaseElement: public CBaseGUI  //base class
{
protected:
   bVector3f Position;		  //position
   bVector3f Box;			  //size box
   bool      Enabled;		  //enabled
   bool      Focused;		  //focused
   BYTE      ElementAling;	  //parent align
   bVector4f Color;			  //color
   BaseElemTickFunc TickFunc;  // tick func

   CBaseElement *Parent;	  //parent
   GString   fComponentName;  //name
   BYTE      BringMode;		  //brign mode
   GString   Hint;			  //hint
   bool      fShowHint;		  //show hint
   
public:
	virtual void    DoExport(){} //layer init
	//hint
	virtual bool    IsShowHint ()const        ;
	virtual void    SetShowHint(const bool S) ;
	virtual void    SetHint    (LPCSTR H)     ;
	virtual LPCSTR  GetHint    ()const        ;
	virtual bool    ShowHint   (LPCSTR T,const float X,const float Y); 

	//name
    void             SetName   (LPCSTR Name)  ;
	virtual  LPCSTR  GetName   ()const        ;
	const   GString &GetNameRef()             ;
	//font
	virtual void SetFontHeight(const float H);

    CBaseElement();
	virtual ~CBaseElement(){};

	//parent
    CBaseElement *GetParent()               ;
	void          SetParent(CBaseElement *P);

	//tick
	void  SetTickFunc(BaseElemTickFunc f);

	//props
	bool  IsEnabled   ()const;
	bool  IsFocused   ()const;
	void  SetFocus    (const bool F);
	BYTE  GetBringMode();
	void  SetBringMode(BYTE T);

	//on focus lost event
	virtual void OnFocusLost();

	//is mouse in this element
	virtual bool  IsMouseIn(const float X,const float Y);

	//position and size
	virtual void  SetPosition(const float X,const float Y,const float Z=0);
	virtual void  SetPosition(const bVector3f &Pos)                       ;
	void  Move       (const float X=0,const float Y=0,const float Z=0);
	const bVector3f &GetPosition()      ;
	      bVector3f  GetPosition()const ;
		  bVector3f &GetPosRef  ()      ;

	virtual void  SetSize    (const float X,const float Y,const float Z=0);
	virtual void  SetSize    (const bVector3f &Pos)                       ;
	const bVector3f &GetSize    ()     ;
	      bVector3f  GetSize    ()const;
		  bVector3f &GetSizeRef ()     ;

  //enable
	void  Enable   ();
	void  Disable  ();

	void  SetAlign(const BYTE A);
	BYTE  GetAlign()            ;

	void SetColor(const float R,const float G,const float B,const float A=1);
	const bVector4f &GetColor();
	//render just ticks
	virtual void Render();
	//return element under mouse
	virtual CBaseElement* GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp=NULL);
};

//action element
//resives mouse and keyboard events

class DLLTYPE CActionElement : public CBaseElement //action level. hold user feedback
{
public:
	virtual void   DoExport();//layer init
	CActionElement();
	//keyboard
	virtual int OnKeyPress(const DWORD Key);// on WM_CHAR
	virtual int OnKeyUp   (const DWORD Key);// on WM_UP
	virtual int OnKeyDown (const DWORD Key);// on WM_DOWN

	//user defined mouse callbacks
    typedef int(*mousecallback)(CActionElement* Sender,const float X,const float Y,DWORD &KeyState);
    mousecallback m_onmove  ;
	mousecallback m_onup    ;
	mousecallback m_ondown  ;
	mousecallback m_onclick ;
	mousecallback m_onDclick;

	//mouse
	virtual int OnMouseMove   (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseClick  (const float X,const float Y,DWORD &KeyState); 
	//NOT WORKING!!
	virtual int OnMouseDClick (const float X,const float Y,DWORD &KeyState);

	//system stack save
	static void SetMouseDest(const float *XYZ);
	static void SetMouseDest(const float X,const float Y);

	//drag'n'drop
	virtual int OnStartDrag   ()                                     ;
	virtual int OnDragOver    (CBaseElement* Draged,CBaseElement *On);
	virtual int OnDragEnd     (CBaseElement* Draged,CBaseElement *On);

	void SetOnClick(mousecallback event);
};


//GUI Container
// not a GUI Class but stores elements

typedef SmartVector<CBaseElement*> CElementList;
class DLLTYPE CGUIContainer
{
protected:
 CElementList   Elements;
 CActionElement *Active;
 CBaseElement   *Selected;
 bool            SelectSortOn;
 bVector3f       ScreenOffset;
 void            MoveElementToTop(CElementList::iterator it);
 void            MoveElementToBottom(CElementList::iterator it);
 virtual void    ContainerAddElement(CBaseElement *Area);
 virtual void    ContainerAddElementToBottom(CBaseElement *Area);
public:
	void            MoveElementToTop(CBaseElement *Elem);   // bring element to top
	virtual LPCSTR GetName();
    void SetItemOffsetX(const float X);	//sets offset of element in local space(aka scroll bar)
	void SetItemOffsetY(const float Y);


    bool IsSelectSortOn();           //auto sort selection
	void SelectSort(const bool On);
	const bVector3f GetItemOffset();


	CGUIContainer();
	void Clear();
	virtual void DeleteElement(const DWORD ID,const bool DoDelete);
	virtual void DeleteElement(CBaseElement *I,const bool DoDelete);
	DWORD GetNumElements();


	//virtual event override
	virtual int OnKeyPress(const DWORD Key);
	virtual int OnKeyUp   (const DWORD Key);
	virtual int OnKeyDown (const DWORD Key);

	virtual int OnMouseMove   (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseClick  (const float X,const float Y,DWORD &KeyState); 
	virtual int OnMouseDClick (const float X,const float Y,DWORD &KeyState);

	DWORD GetControlUnderMouse(const float X,const float Y);
    CBaseElement* GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp=NULL);

	//update
	virtual void CascadeUpdate();

	virtual CBaseElement*GetActiveElement  ();
    virtual CBaseElement*GetSelectedElement();

	bVector3f GetMaxPosition();

};

#define NUM_BUTTON_IMAGES 4

//CButton
//just a button

// REPEAT - the MAX coords of texture
// OFFSET - the START coords of texture

class DLLTYPE CButton     : public CActionElement   //button
{
protected:
	BYTE    Aligment;
	DWORD   Font;
	bVector4f FontColor;
	GString Caption;
	DWORD   TexID[NUM_BUTTON_IMAGES];
	DWORD   StateTexture;
	bVector3f Rep;
public:
	CButton();
	virtual void   DoExport();
	void SetRepeat (const float X=1,const float Y=1);
	void SetCaption(LPCSTR Cap);

	//texture 0 - passive
	//texture 1 - on mouse on
	//texture 2 - on mouse click

	void SetTexture(const DWORD Num,const DWORD ID);

	void Render();

	virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
	virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);
  //load/save
	//virtual int Store(GString *Out);
};

//Switch aka check box
class DLLTYPE CSwitch     : public CButton
{
	bool On;
public:
	CSwitch();
	//is on and set on
	bool IsOn();
	void SetOn(bool _On=true);
	int OnMouseClick  (const float X,const float Y,DWORD &KeyState);
	void Render();
	virtual void LuaValidate();
};

//TextArea
//NO TEXT INPUT HERE!

class DLLTYPE CTextArea   : public CActionElement  //TextArea
{
protected:
	BYTE    Aligment;
	DWORD   Font;
	GString Text;
	float FontHeight;
public:
	CTextArea();
	void Stop();
	virtual ~CTextArea();
	virtual void   DoExport();
	//text and font height
	void SetText(LPCSTR Tx);
	void SetFontHeight(const float H);
	void Render();
};

//CImage
//like a CButton

class DLLTYPE CImage      : public CActionElement  //Image
{
protected:
	DWORD TextureID;
	bVector3f Rep;
	bVector3f Offset;
    DWORD     Blend;
public:
	virtual void   DoExport();
	CImage();
	void SetTexture(DWORD ID);
	void SetRepeat (const float X=1,const float Y=1);
	void SetOffset (const float X=0,const float Y=0);
	void Render();
    void SetBlend(const DWORD Blend);
};

//ModelImage
//NOT WORKING!

class DLLTYPE CModelImage:public CActionElement
{
protected:
	DWORD Model;
	float State;
public:
	virtual void   DoExport();
	void SetModel(const DWORD M);
	void SetState(const float S);
	float GetState();
	DWORD GetModel();
};

//ScrolBarDef

#ifdef SB_NONE
#undef  SB_NONE
#endif
#define SB_NONE        0   //no scrollbars
#define SB_VERTICAL    1   //only vertical
#define SB_HORIZONTAL  2   //only horozintal
#define SB_BOTH        3   //both
#define SB_AUTO        4   //any. if needed

class DLLTYPE CScrollBar:public CActionElement
{
protected:
	float fMIN;
	float fMAX;
	float fPosition;
	bool  Locked;
	float fPosDELTA_SIZE;
	DWORD TextureIDP,TextureIDL;
	BYTE  sTYPE;
public:
	//set scroll type
    void    SetScrollType(BYTE MODE);

	virtual void   DoExport();//layer init

	//sets texture for bar and barline
    void SetLineTexture(const DWORD T);
	void SetBarTexture (const DWORD T);



    CScrollBar();

	//MIN,MAX,POSITION. all that i need
	float GetMIN();
	float GetMAX();
	float GetBarPosition();

	void  SetMIN        (const float V);
	void  SetMAX        (const float V);
	void  SetBarPosition(const float V);

	void Render();

	virtual int OnMouseMove   (const float X,const float Y,DWORD &KeyState);
    virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
    virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);

};


//Ok. First normal element
//Allways have 2 scrollbars(hidden) 
//is a window

class DLLTYPE CScrollBox:public CActionElement,public CGUIContainer
{
protected:
  BYTE  fAllowScrollBars;
  CScrollBar *VSlider,*HSlider;
  bVector3f f_XY;
  bVector3f BoxStart;
  bVector3f BoxEnd;
  DWORD     Background;
  bVector3f Rep;
  bVector3f Offset;
public:
//back ground texture
  void    SetBackground(const DWORD b);
  DWORD   GetBackground();

  virtual void LuaValidate();
//image set
  void SetRepeat (const float X=1,const float Y=1);
  void SetOffset (const float X=0,const float Y=0);

//scrollbar def
  virtual LPCSTR GetName();
  void    AllowScrollBars(BYTE MODE);

//position/size tasks
  virtual void  SetPosition(const float X,const float Y,const float Z=0);
  virtual void  SetPosition(const bVector3f &Pos)                       ;

  virtual void  SetSize    (const float X,const float Y,const float Z=0);
  virtual void  SetSize    (const bVector3f &Pos)                       ;

  void SetSBox();

  void SetBoxStart(const bVector3f &Pos);
  void SetBoxEnd  (const bVector3f &Pos);

  void SetBoxStart(const float X,const float Y,const float Z=0);
  void SetBoxEnd  (const float X,const float Y,const float Z=0);


  CScrollBox();

//cascade event

  virtual bool    IsShowHint()const;

  virtual LPCSTR  GetHint()const;

  CMousePointer *GetMouse();



 // virtual void   DoExport(){}
  virtual int OnKeyPress(const DWORD Key);
  virtual int OnKeyUp   (const DWORD Key);
  virtual int OnKeyDown (const DWORD Key);
  const float* GetXYOff(const float X,const float Y);
  virtual int OnMouseMove   (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseClick  (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseDClick (const float X,const float Y,DWORD &KeyState);

  virtual void CascadeUpdate();

  void AddElement(CBaseElement *Area);
  void AddElementToBottom(CBaseElement *Area);

  void RenderPassive();
  void Render(){return RenderPassive();}

  	//virtual int Store(GString *Out);

  CBaseElement* GetElementUnderMouse(const float X,const float Y,CBaseElement* Exp=NULL);
};

typedef void(*CListBoxSelectionCallBack)(CBaseElement *Caller,DWORD ElementID);


//Listbox is a SAME thing that a scrollbox
//it just have strings(AddLine) and auto convert them to AddElement(new TextArea(text))
//also manage size

class DLLTYPE CListBox:public CScrollBox
{
protected:
	DWORD fItemSelected;
	DWORD BackTexture;
	DWORD LastItemSelected;
	//typedef SmartVector<CTextArea*> cListBoxItems;
    //cListBoxItems lItems;
	CListBoxSelectionCallBack SelectionCallBack;
	float fontheight;
	bVector3f MaxSize;
	int       AutoSize;
public:
    CListBox();

	void SetFontHeight(const float H);
	float GetFontHeight();
	void SetSelectionCallBack(CListBoxSelectionCallBack CB);
	void AddLine(LPCSTR Line);
	//void Clear();
	void DeleteLine(DWORD LineID);
	void SelectItem(const DWORD Item);
	DWORD SelectedItem();
	void  SetMaxSize(const float x,const float y);
	void  SetMaxSize(const bVector3f &NewSize);
	void  SetAutoSize(int Mode);

	int OnMouseDown   (const float X,const float Y,DWORD &KeyState);

	void Render();
};


//window
//scrollbox again.
//nothing new
//style determine caption and window buttons.
//normaly - off

class DLLTYPE CWindow:public CScrollBox
{
protected:
	DWORD Style;
	GString Caption;
public:
    CWindow();

	void SetCaption(LPCSTR Text);
	LPCSTR GetCaption();

	void Render();
	void SetStyle(const DWORD Style);

	void Show();

	virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState) ;
};


//Area
//ScrollBox again :)

typedef SmartVector<CWindow*> CWindowList;
class DLLTYPE CArea:public CWindow
{
    bool LikeAWin;
public:
    CArea();
	void AddWindow(CWindow *Wnd);
	bool  IsMouseIn(const float X,const float Y);
    void SetAsWindow(bool On=true);

	void Render();
    virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState)
     {
     if(LikeAWin)return CWindow::OnMouseDown(X,Y,KeyState);
     else        return CScrollBox::OnMouseDown(X,Y,KeyState);
     }
};

typedef SmartVector<CArea*> CAreaList;

void   DLLTYPE   LockMouse();
void   DLLTYPE   UnlockMouse();
bool   DLLTYPE   IsMouseLocked();


//GUI manager

class DLLTYPE CGUIManager:public CScrollBox
{
	DWORD     MouseKeyState;


	bool       AsseptActions,AsseptMouse,AsseptKeyBoard,
		       DrawMouse;

	void      RenderMouse();
	void      DoUserCtrl();

	static bVector3f targMousePosition;
	static bVector3f MousePosition;
	static bVector3f Mouse3D;
	static    CGUIManager *GUI;
public:
    //CImage    Mouse;
	static CGUIManager* GetGUI();
	
	static const bVector3f &GetMousePosition();
	static const bVector3f &GetTargPosition();
	static const bVector3f &Get3DPosition();
	DWORD GetMouseKey();

	CGUIManager();
	void AddArea(CArea *Area);
	void Render();

	//Mouse

	//Props

	void  SetMouseDraw     (const bool Draw);
	void  EnableDrawMouse  (const bool Enable);
	void  EnableMouse      (const bool Enable);
	void  EnableKeyBoard   (const bool Enable);
	void  EnableActions    (const bool Enable);
	void  EnableFullActions(const bool Enable);

    static void AdjustFromGUI(float *x,float *y,float *w=NULL,float *h=NULL);
    static void AdjustToGUI  (float *x,float *y,float *w=NULL,float *h=NULL);

	//virtual int Store(GString *Out);
	friend class CActionElement;
};


//render functions

void DLLTYPE guifaceClear();
void DLLTYPE guifaceSETRECT(const bVector3f &From,const bVector3f &To,bool Stensil);
void DLLTYPE guifaceMODRECT(const bVector3f &From,const bVector3f &To);
void DLLTYPE guifaceENDRECT();
void DLLTYPE guifaceSetResolution(const float X,const float Y);
void DLLTYPE guifaceSetResMode(const DWORD Mode);
void DLLTYPE guifaceRECT(const bVector3f &Pos,const bVector3f &Size,const DWORD Texture,const bVector4f &Color,const bVector3f &Rep,const DWORD Blend=0);
void DLLTYPE guifaceRECT(const bVector3f &Pos,const bVector3f &Size,const DWORD Texture,const bVector4f &Color,const bVector3f &Rep,const bVector3f &Off,const DWORD Blend=0);
void DLLTYPE guifaceTEXT(const bVector3f &Pos,const bVector3f &Size,const BYTE Aling,LPCSTR Text,const DWORD Font,const bVector4f &Color,const float FH=1);

const bVector3f& gui_getoldmouse();
const bVector3f& gui_getcurmouse();
void      gui_setoldmouse(const bVector3f &Mouse);
void      gui_setcurmouse(const bVector3f &Mouse);

void guiClearDepthStack();

DWORD DLLTYPE GetGUITexture(LPCSTR Name);



void guifaceRENDER();

#endif