/*===========================================================================
    Заголовок: GenericWindow.cpp
    Автор: Kashey
    Информация: Generic window class

===========================================================================*/
#include "ProjectSpace.h"
#include "GenericWindow.h"

CGenericWindow::CGenericWindow(BlopWindowStartUp &StartUpInfo,BlopBaseObject *_Driver):BaseObject(REFC_CORE,REFB_INTERFACE,"Window")
{
//	Assign(_Driver);
    Options=StartUpInfo;
}

CGenericWindow::~CGenericWindow(void)
{
	Stop();
}
