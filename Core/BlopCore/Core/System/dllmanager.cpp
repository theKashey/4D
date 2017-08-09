/*===========================================================================
    Заголовок: DLLManager.cpp
    Автор: Kashey
    Информация:	 DLL manager

===========================================================================*/
#include "ProjectSpace.h"


#include "DLLManager.h"

SmartVector<DLLInfo> CDLLManager::DLLS;

CDLLManager::CDLLManager(void)
{
}

CDLLManager::~CDLLManager(void)
{
}

HANDLE CDLLManager::LoadDLL(LPCSTR Name)
{
	if(!Name)
		return GetModuleHandle(NULL);
	for(int i=0;i<DLLS.size();i++)
		if(DLLS[i].Name==Name && DLLS[i].ID) return DLLS[i].ID;
	DLLInfo Info;
	Info.Name=Name;
	Info.ID  =LoadLibrary(Name);
	if(!Info.ID) return NULL;
	DLLS.push_back(Info);
	return Info.ID;
};

HANDLE CDLLManager::UnloadDLL(LPCSTR Name)
{
	for(int i=0;i<DLLS.size();i++)
	 if(DLLS[i].Name==Name)
	 {
         //UnloadLibrary(DLLS[i].ID)
		 CloseHandle(DLLS[i].ID);
         DLLS[i].ID=0;
		 return (HANDLE)1;
	 }
	return NULL;
}

void*  CDLLManager::LoadFunc(HANDLE DLL,LPCSTR FName)
{
	//if(!DLL) return NULL;
	return (void*)GetProcAddress((HMODULE)DLL,FName);
};

bool            TouchLibrary(LPCSTR Name)
{
	return CDLLManager::LoadDLL(Name)!=NULL;
}

bool            DLLTYPE  UntouchLibrary(LPCSTR Name)
{
	return CDLLManager::UnloadDLL(Name)!=NULL;
}

void *LoadDLLFunc(LPCSTR DLLName,LPCSTR FuncName)
{
	return CDLLManager::LoadFunc(CDLLManager::LoadDLL(DLLName),FuncName);
}

DLLVOIDFUNC DLLTYPE LoadPVoidDLLFunc(LPCSTR DLLName,LPCSTR FuncName)
{
	return (DLLVOIDFUNC)LoadDLLFunc(DLLName,FuncName);
}

DLLbVectorFUNC DLLTYPE LoadbVectorDLLFunc(LPCSTR DLLName,LPCSTR FuncName)
{
	return (DLLbVectorFUNC)LoadDLLFunc(DLLName,FuncName);
}

DLLbVector2FUNC DLLTYPE Loadb2VectorDLLFunc(LPCSTR DLLName,LPCSTR FuncName)
{
	return (DLLbVector2FUNC)LoadDLLFunc(DLLName,FuncName);
}