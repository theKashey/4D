/*===========================================================================
    Заголовок: Win32Window.cpp
    Автор: Kashey
    Информация: Win 32 window

===========================================================================*/
#include "ProjectSpace.h"
#include "GenericWindow.h"
#include "Win32Window.h"



#pragma comment (lib,"User32.lib")
#pragma comment (lib,"Gdi32.lib")


CWin32Window* WND;

char __inlineWinMessage2StringBuf[0xFF];


LPCSTR Win32MessageToStr(UINT Message)
{
#define MMRET(a,b) case a : strcpy_s(__inlineWinMessage2StringBuf,sizeof(__inlineWinMessage2StringBuf),b);return __inlineWinMessage2StringBuf;
	switch(Message)
	{
	 MMRET(WM_CHAR     ,"WM_CHAR")
	 MMRET(WM_CLOSE    ,"WM_CLOSE")
	 MMRET(WM_DESTROY  ,"WM_DESTROY")
	 MMRET(WM_ACTIVATE ,"WM_ACTIVATE")
	 MMRET(WM_MOUSEMOVE,"WM_MOUSEMOVE");
	default:sprintf_s(__inlineWinMessage2StringBuf,sizeof(__inlineWinMessage2StringBuf),"#%X",Message);return __inlineWinMessage2StringBuf;
	}
#undef MMRET
}

CWin32Window::CWin32Window(BlopWindowStartUp &StartUpInfo,BlopBaseObject *_Driver):CGenericWindow(StartUpInfo,_Driver),ProWin32("WIN32")
{
	WND    =this;
	//SetName("CWin32Window");
//	lua::setdword("win_width" ,Options.Width);
//	lua::setdword("win_height",Options.Height);
	lua::setdword("win_dc" ,(DWORD)MyDC);
//	lua::setdword("win_BPP",(DWORD)Options.BPP);
//	lua::setdword("win_DPP",(DWORD)Options.DPP);
	Input=NULL;
}

CWin32Window::~CWin32Window(void)
{

}

void CWin32Window::Stop()
{

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return WND->WndProc(hWnd,message,wParam,lParam);
}

LRESULT CWin32Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//if(message!=0x84)
	// ConsoleOut->Printf("wnd message: %s",Win32MessageToStr(message));
	int Key=0;
	switch (message) 
	{
	case WM_KEYUP  : OnKeyUp  (wParam); break;
	case WM_KEYDOWN: OnKeyDown(wParam); break; 
    case WM_CHAR   : OnKeyChar(wParam); break;
    case WM_CLOSE  : 
	case WM_DESTROY: 
		             SendUpSignal(CObjectIDTwin(GetRootID(),GetID()),BMSG::CMD_TERMINATE);
		             PostQuitMessage(0);
		             break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
         Key=1;
    case WM_MOUSEMOVE:
		WORD CenterX;CenterX=lua::getdword("win_width")  * 0.5f;
		WORD CenterY;CenterY=lua::getdword("win_height") * 0.5f;
        WORD fwKeys;fwKeys = wParam;        // key flags 
        WORD xPos  ;xPos   = LOWORD(lParam);  // horizontal position of cursor 
        WORD yPos  ;yPos   = HIWORD(lParam);  
		if(!fwKeys && LastMouseKey) OnMouseUp(xPos,yPos,fwKeys);
		else
		{ 
		  if(!LastMouseKey)OnMouseDown(xPos,yPos,fwKeys);
		  OnMouseMove(xPos,yPos,fwKeys);
		}
		LastMouseKey=fwKeys;
        //if(xPos!=CenterX || yPos!=CenterY) SetCursorPos(CenterX,CenterY);
		break;

    case WM_ACTIVATE:  ShowWindow   (hWnd, SW_SHOW);
                       UpdateWindow (hWnd);
					   break; 

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CWin32Window::OnMouseUp  (WORD x,WORD y,WORD Key){if(Input)Input->OnMouseUp  (x,y,Key);}
void CWin32Window::OnMouseDown(WORD x,WORD y,WORD Key){if(Input)Input->OnMouseDown(x,y,Key);}
void CWin32Window::OnMouseMove(WORD x,WORD y,WORD Key){if(Input)Input->OnMouseMove(x,y,Key);}
void CWin32Window::OnKeyChar  (WORD Key)              {if(Input)Input->OnKeyChar  (    Key);}
void CWin32Window::OnKeyUp    (WORD Key)              {if(Input)Input->OnKeyUp    (    Key);}
void CWin32Window::OnKeyDown  (WORD Key)              {if(Input)Input->OnKeyDown  (    Key);}

bool CWin32Window::Start()
{
  int exStyle;
  int Width = lua::getdword("win_width");
  int Height= lua::getdword("win_height");
  ZeroMemory(&WindowClass,sizeof(WindowClass));
  WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = ::WndProc;
  WindowClass.cbClsExtra  = 0;
  WindowClass.cbWndExtra  = 0;
  WindowClass.hInstance   = (HINSTANCE)Options.hInstance;
  WindowClass.hCursor     = LoadCursor (0, IDC_ARROW);
  WindowClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
  WindowClass.lpszClassName = "BlopWindowClass";
  if (!RegisterClass (&WindowClass)) Error2Console(SYS_WIN32,"couldn`t register window class\n");
  if (lua::getdword("win_fullscreen"))
        exStyle = WS_EX_TOPMOST   | WS_EX_APPWINDOW;
  else
        exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  Wnd = 0;
  Wnd = CreateWindowEx(exStyle,"BlopWindowClass", "BlopWindowClass",
                       WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // private for OpenGL( HA!)
                       0,0,
                       Width, Height,
                       0, 0, 0, NULL);
 
  SetCursorPos(Width / 2, Height / 2);
  OnMouseMove(Width / 2, Height / 2,0);
  if (!Wnd) {Error2Console(SYS_WIN32,"couldn`t create window due to %d\n",GetLastError());return false;}
  if (lua::getdword("win_fullscreen"))
  {
	DEVMODE dmScreenSettings;
    // Clear screen settings structure
    ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
    // Set parameters for the screen setting
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = Width;                    // Window width
    dmScreenSettings.dmPelsHeight = Height;                   // Window height
    dmScreenSettings.dmBitsPerPel = 32;               // Window color depth
    dmScreenSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
    // Try to change screen mode to fullscreen
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_FAILED)
	{
      // Couldn't switch to fullscreen
      MessageBox(0, "Unable to switch to fullscreen!", "Error", MB_OK | MB_ICONERROR);
      // Go to windowed mode instead
      lua::setdword("win_fullscreen",false);
    }
  }
  MyDC=::GetDC(Wnd);
  MyDC=::GetDC(Wnd);
  lua::setdword("win_hwnd" ,(DWORD)Wnd);
  lua::setdword("win_dc"   ,(DWORD)MyDC);
  if(!MyDC) 
	   return false;
  ShowWindow (Wnd, SW_SHOW);
  UpdateWindow (Wnd);
  WndStarted=true;
  return true;
}

DWORD CWin32Window::Tick (float Time)
{
	Profile(ProWin32);
		while(IsMessages())//GetMessage(&Message, 0, 0, 0))
		{
			//if(!GetMessage(&Message, 0, 0, 0))
			//	SendUpSignal(CObjectIDTwin(GetRootID(),thisID),CMD_TERMINATE);
			TranslateMessage (&Message);
            DispatchMessage  (&Message);
		}
	return 1;
}



