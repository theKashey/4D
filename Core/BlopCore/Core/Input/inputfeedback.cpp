#include "ProjectSpace.h"
#include "inputfeedback.h"

void CInputFeedBack::SetName(LPCSTR F,...)
{
	FORMAT(Name,F);
}

CInputFeedBack::CInputFeedBack(void){ SetName("whole");Parent=NULL;}
CInputFeedBack::~CInputFeedBack(void){}
bool CInputFeedBack::AllowCrossfall(){return true;}

void CInputFeedBack::SetParent(CInput *P){ASSERT("InputParent allready exists" && (Parent==NULL));Parent=P;}

void CInputFeedBack::OnMouseMove(float x,float y,DWORD Key){};
void CInputFeedBack::OnMouseUp  (float x,float y,DWORD Key){};
void CInputFeedBack::OnMouseDown(float x,float y,DWORD Key){};

void CInputFeedBack::OnKeyChar  (DWORD Key){};
void CInputFeedBack::OnKeyDown  (DWORD Key){};
void CInputFeedBack::OnKeyUp    (DWORD Key){};
void CInputFeedBack::InpStart   (){};
void CInputFeedBack::Tick(){};




static CInput* DefaultInput=NULL;

//format // x,y,key
#define toXY  \
DWORD   XY = lua::getdword("input.mousexy");\
float   x  = HIWORD(XY);\
float   y  = LOWORD(XY);\
float   key= lua::getdword("input.mousekeystate");//lua_tonumber(L, 3);

static int inp_m_down(lua_State *L)
{
  toXY
  if(DefaultInput)
	  DefaultInput->OnMouseDown(x,y,key);
  return 0;
}

static int inp_m_move(lua_State *L)
{
  toXY
  if(DefaultInput)
	  DefaultInput->OnMouseMove(x,y,key);
  return 0;
}

static int inp_m_up(lua_State *L)
{
  toXY
  if(DefaultInput)
	  DefaultInput->OnMouseUp(x,y,key);
  return 0;
}

#define toKEY DWORD key=lua_tonumber(L, 1);

static int inp_k_up(lua_State *L)
{
	toKEY
  if(DefaultInput)
	  DefaultInput->OnKeyUp(key);
  return 0;
}

static int inp_k_down(lua_State *L)
{
	toKEY
  if(DefaultInput)
	  DefaultInput->OnKeyDown(key);
  return 0;
}

static int inp_k_on(lua_State *L)
{
  DWORD key=lua_tonumber(L, 1);
  if(lua_isstring(L,1)) key=lua_tostring(L,1)[0];
  if(DefaultInput)
	  lua_pushnumber(L,DefaultInput->IsKeyOn(key));
  else
	  lua_pushnumber(L,false);
  return 1;
}

CInput::CInput():BaseObject(REFC_CORE,REFB_INTERFACE,"INPUT")
{
//	SetName("Input");
	ASSERT("Input redirector allready exists" && (DefaultInput==NULL));
    DefaultInput=this;
	lua::dostring("input={};");

	lua::luafunction("mousedown",inp_m_down,"input");
	lua::luafunction("mousemove",inp_m_move,"input");
	lua::luafunction("mouseup"  ,inp_m_up  ,"input");

	lua::luafunction("keyup"    ,inp_k_up  ,"input");
	lua::luafunction("keydown"  ,inp_k_down,"input");
	lua::luafunction("iskeyon"  ,inp_k_on  ,"input");

    lua::dostring("input.mousexy       =0;");
    lua::dostring("input.mousekeystate =0;");
    lua::dostring("input.keystate      =0;");
    lua::dostring("input.mouse_button1 =0;");
    lua::dostring("input.mouse_button2 =0;");
    lua::dostring("input.mouse_button3 =0;");


	ZeroMemory(Keys,sizeof(Keys));
}
CInput::~CInput()
{
}

static void SetMousePos(float x,float y)
{
	static float oldx=-1;
    static float oldy=-1;
	static bool  oldinit=false;
	if(!oldinit)
	{
		oldx=x;
		oldy=y;
		oldinit=true;
	}
	//set real xy
	//lua::doVstring("input.x=%f;input.y=%f;",x,y);
    lua::setfloat("input.x",x);
    lua::setfloat("input.y",y);
	//set offset xy
	//lua::doVstring("input.offx=%f;input.offy=%f;",oldx-x,oldy-y);
	//set virtual xy
	float x640=x*640.0f/World()->Camera()->GetWidth();
	float y480=y*480.0f/World()->Camera()->GetHeight();
    lua::setfloat("input.mouse_x640",x640);
    lua::setfloat("input.mouse_y480",y480);

    //lua::doVstring("input.mouse_x640=%f;input.mouse_y480=%f;",x640,y480);
}

static void SetMouseKeys(DWORD Key,int UpDown)
{
  if(!UpDown)return;
  if(Key!=(DWORD)-1)
  {
   static DWORD LastKeyState=-1;
   ASSERT(!(LastKeyState==-1 && Key==-1));
   if(LastKeyState==-1)LastKeyState=Key;
   if(UpDown==-1)
   {
    if(Key==0)Key=LastKeyState;
    Key=~Key;
    Key&=LastKeyState;
   }
   else Key|=LastKeyState;
   lua::doVstring("input.keystate      =%d;",Key);
   lua::doVstring("input.mouse_button1 =%d;",(Key & 0x01)?1:0);
   lua::doVstring("input.mouse_button2 =%d;",(Key & 0x02)?1:0);
   lua::doVstring("input.mouse_button3 =%d;",(Key & 0x04)?1:0);
   LastKeyState=Key;
   /*Print2Console(0,"ks:%d(%d,%d,%d)",Key,lua::getdword("input.mouse_button1"),
                                         lua::getdword("input.mouse_button2"),
                                         lua::getdword("input.mouse_button3"));
   */
  }
}


bool CInput::AddInput      (CInputFeedBack* obj)
{
	Inputs.push_back(obj);
	obj->SetParent(this);
	return true;
}
bool CInput::RemoveInput   (CInputFeedBack* obj)
{
	return true;
}
bool CInput::SetActiveInput(CInputFeedBack* obj)
{
	return true;
}

void CInput::OnMouseMove(float x,float y,DWORD Key)
{
	SetMousePos(x,y);SetMouseKeys(Key,0);
};
void CInput::OnMouseUp  (float x,float y,DWORD Key)
{
	SetMousePos(x,y);SetMouseKeys(Key,-1);
};
void CInput::OnMouseDown(float x,float y,DWORD Key)
{
	SetMousePos(x,y);SetMouseKeys(Key,1);
};

void CInput::OnKeyChar  (DWORD Key)
{
};
void CInput::OnKeyDown  (DWORD Key)
{
	BYTE KeyC=fupcase(Key&0xFF);
	Keys[KeyC]=true;
};
void CInput::OnKeyUp    (DWORD Key)
{
	BYTE KeyC=fupcase(Key&0xFF);
	Keys[KeyC]=false;
};
bool CInput::IsKeyOn    (DWORD Key)   
{
	BYTE KeyC=fupcase(Key&0xFF);
	return Keys[KeyC]==true;
};

bool CInput::Start()
{
	return true;
}
DWORD CInput::Tick(float Time)
{
	return 0;
}
