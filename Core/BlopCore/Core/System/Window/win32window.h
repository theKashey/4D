#pragma once

class CWin32Window : public CGenericWindow
{
	
    MSG               Message; 
	HDC               MyDC;
	bool              WndStarted;
	HWND              Wnd;
    WNDCLASS WindowClass;

	WORD              LastMouseKey;
	_Profilers::CProfiler         ProWin32;
	

    int IsMessages(){return PeekMessage(&Message, 0, 0, 0, PM_REMOVE);}
public:

	CWin32Window(BlopWindowStartUp &StartUpInfo,BlopBaseObject *_Driver);
	~CWin32Window(void);

	virtual bool Start();
	virtual void Stop ();
	virtual DWORD Tick (float Time);

	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnMouseUp  (WORD x,WORD y,WORD Key);
	void OnMouseDown(WORD x,WORD y,WORD Key);
	void OnMouseMove(WORD x,WORD y,WORD Key);
	void OnKeyChar  (WORD Key);
	void OnKeyDown  (WORD Key);
	void OnKeyUp    (WORD Key);



	//DWORD   GetParameterDWORD(LPCSTR Name);
};
