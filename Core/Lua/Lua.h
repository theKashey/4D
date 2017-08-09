// Lua.h : main header file for the Lua DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CLuaApp
// See Lua.cpp for the implementation of this class
//

class CLuaApp : public CWinApp
{
public:
	CLuaApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
