//---------------------------------------------------------------------------

#ifndef ProjectSpaceH
#define ProjectSpaceH

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x400
#include <windows.h>
//#include <afxsock.h>
#define bzero ZeroMemory
#define strcasecmp _stricmp

typedef const char CSTR; 
#ifndef LPCBYTE
typedef const BYTE *LPCBYTE;
#endif

typedef const DWORD CDWORD;
typedef const int   cint  ;

#pragma warning (disable : 4251)
#pragma warning (disable : 4244)
#pragma warning (disable : 4275)
//FORCE:MULTIPLE
#pragma warning (disable : 4006)
#pragma warning (disable : 4075)
#pragma warning (disable : 4088)
#pragma warning (disable : 4786)
#pragma warning (disable : 4311)

typedef void *BPVOID;
typedef const BPVOID BPCVOID;

#include "Core\Errors\ErrorCodes.h"
#include "Core\Sys\theads.h"

//#define DEBUG

#ifdef DEBUG
#include "assert.h"
#define blp_ASSERT(a) { if(!a) assert(a); }
#else
#define blp_ASSERT(a)
#endif

#define DLLTYPE _declspec(dllexport)
#define DLL_TYPE //_declspec(dllexport)

#include <stdio.h>

#ifndef	BINLINE
#define BINLINE inline
#define BFINLINE __forceinline
#endif

#define dMIN(a,b) (a<b?a:b)
#define dMAX(a,b) (a>b?a:b)
template <class T,class G>
inline T MIN(const T &t,const G &g){return dMIN(t,g);}
template <class T,class G>
inline T MAX(const T &t,const G &g){return dMAX(t,g);}


//WINIE MEMORY STUFF
#ifndef _BLOP_NO_WINNIE

#include "Core/Sys/winnie_alloc/winnie_alloc.h"
//NOTE: now it do not throw std::bad_alloc, and do not use new_handler
inline void *operator new(size_t size)
{return Winnie::Alloc(size);}

inline void *operator new[](size_t size)
{ return Winnie::Alloc(size);}

inline void operator delete(void *p)
{ if (p) Winnie::Free(p);}

inline void operator delete[](void *p)
{ if (p) Winnie::Free(p);}

#endif

enum CORE_STATES
{
  CS_GLOBALSTOP=1
};
DWORD DLLTYPE GetCoreState(DWORD VAL);

//------------------------------------
//#include "ProjectDotNet.h"




#include "Core\Sys\ProjectEx.h"


template <int size,typename T1,typename T2>
void static_copy_bytes(T1 *to,const T2 *fm)
{struct char_array{	char data[size];};
*(reinterpret_cast<      char_array *(to)) = *(reinterpret_cast<const char_array *(fm));
}

template <int count,typename T>
void static_copy_entries(T *to,const T *fm)
{struct entry_array{T data[count];};
*(reinterpret_cast<      entry_array *(to)) = *(reinterpret_cast<const entry_array *(fm));
}

//#ifndef BLOP_NO_LUA

//#include "../Lua/luascript.h"
#include "../Lua/miniLua.h"

#include "Core/WorldDef.h"

HANDLE DLLTYPE QueryMainThreadHandle();
//#endif

//SmartVector<CBaseObject*> *CreateStoredObjects();

///FORCE:MULTIPLE

extern DWORD SYS_ENGINE;

//---------------------------------------------------------------------------
#endif
