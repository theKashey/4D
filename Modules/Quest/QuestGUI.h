#ifndef _QUEST_GUI_H
#define _QUEST_GUI_H

#include "../GUI/GUI.h"

class CBackImage:public CImage
{
	GString MText,MText2;
public:
	CBackImage();
	void Render();
	int OnMouseDown(const float X,const float Y,DWORD &KeyState);
	int OnMouseMove(const float X,const float Y,DWORD &KeyState);
};

class QActiveAdmin:public CWindow
{
public:
	QActiveAdmin();
};

class QAdmin:public CWindow
{
public:
	QAdmin();
};

class GUIWindow:public CWindow
{
public:
	GUIWindow();
};

#endif