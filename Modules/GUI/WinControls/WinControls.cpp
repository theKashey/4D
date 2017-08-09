/*===========================================================================
    Заголовок: WinControls.cpp
    Автор: kashey
    Информация: Тракабл окошки
===========================================================================*/

#include "stdafx.h"
#include "../GUI.h"
#include "WinControls.h"

int CWindowCaption::OnMouseMove   (const float X,const float Y,DWORD &KeyState)
{
	if(Locked)
	{
	 CWindow *PW=(CWindow *)GetParent();
	 bVector3f OL=gui_getoldmouse();
	 bVector3f CL=gui_getcurmouse();
	 PW->Move(CL[0]-OL[0],CL[1]-OL[1]);
	}
	return 0;
}
int CWindowCaption::OnMouseUp     (const float X,const float Y,DWORD &KeyState)
{
   UnlockMouse();
   Locked=false;
   return 0;
}
int CWindowCaption::OnMouseDown   (const float X,const float Y,DWORD &KeyState)
{
   LockMouse();
   Locked=true;
   return 0;
}
