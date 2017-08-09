#pragma once

struct DLLInfo
{
	GString Name;
	HANDLE  ID;
};

class CDLLManager
{
	static SmartVector<DLLInfo> DLLS;
public:
	CDLLManager(void);
	~CDLLManager(void);
	static HANDLE LoadDLL(LPCSTR Name);
	static HANDLE UnloadDLL(LPCSTR Name);
	static void*  LoadFunc(HANDLE DLL,LPCSTR FName);
};

typedef void*(*DLLVOIDFUNC)();
typedef void*(*DLLbVectorFUNC)(bVector3f Dir);
typedef void*(*DLLbVector2FUNC)(bVector3f Pos,bVector3f Dir);

bool            DLLTYPE  TouchLibrary(LPCSTR Name);
bool            DLLTYPE  UntouchLibrary(LPCSTR Name);
DLLVOIDFUNC     DLLTYPE  LoadPVoidDLLFunc(LPCSTR DLLName,LPCSTR FuncName); 
DLLbVectorFUNC  DLLTYPE  LoadbVectorDLLFunc(LPCSTR DLLName,LPCSTR FuncName); 
DLLbVector2FUNC DLLTYPE  Loadb2VectorDLLFunc(LPCSTR DLLName,LPCSTR FuncName); 
PVOID           DLLTYPE  LoadDLLFunc(LPCSTR DLLName,LPCSTR FuncName);