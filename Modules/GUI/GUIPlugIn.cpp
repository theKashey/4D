#include "stdafx.h"
#include "VUC.h"

extern DWORD SYS_QUEST;

class CGUIPlugIn : public BaseObject
{
public:
	CGUIPlugIn(void):BaseObject(REFC_CORE,REFB_INTERFACE,"GUI"){}
	~CGUIPlugIn(void){}

	bool Start()
	{
		GUI::CreateGUI();
		return true;
	}
	DWORD  Tick (float Time)
	{
		GUI::RenderGUI();
		return 0;
	}
};

PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN
    MAKE_PLUGIN (new CGUIPlugIn,"QUI",0,true);
    END_PLUGIN
}