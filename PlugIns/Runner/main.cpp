#include "windows.h"
#include "USEIMPORT.h"
//#include "projectspace.h"
#include "stdio.h"


void PrintLine(LPCSTR Line)
{
    printf("%s\n",Line);
}

typedef bool(*__InitBlop)(BlopStartInformation &);
__InitBlop _InitBlop;
typedef bool(*__BlopTick)();
typedef void(*__BlopEnd)();
__BlopTick _BlopTick,_StartBlop;
__BlopEnd _BlopEnd;


bool bTouchLibrary(LPCSTR L)
{
 LoadLibrary(L);
 return true;
}

//#pragma comment (lib,"blopcore.lib")

int main () 
{
    printf("staring...");
	BlopStartInformation BST;
    BST.Log.ConOut=PrintLine;
    BST.Log.LogEnabled  = true;
    BST.Log.UseLogFile  = true;
    BST.Log.UseScreenOut= true;
    BST.Modules.Render=(LPCSTR)1;
    BST.Modules.Window=(LPCSTR)1;
    char _Buf[0xFF];
	LPSTR Buf=_Buf;
    ZeroMemory(_Buf,sizeof(_Buf));
	strcpy(Buf,"Sound.dll");Buf+=strlen(Buf)+1;
    //strcpy(Buf,"AI.dll");Buf+=strlen(Buf)+1;
    strcpy(Buf,"GUI.dll");Buf+=strlen(Buf)+1;
    //strcpy(Buf,"quest.dll");Buf+=strlen(Buf)+1;

    //strcpy(Buf,"Modules/TokaPH.dll");Buf+=strlen(Buf)+1;
    //strcpy(Buf,"Modules/Sound.dll");Buf+=strlen(Buf)+1;
    //strcpy(Buf,"Modules/AI.dll");
    BST.Modules.AddModules=_Buf;
	BST.Window.hInstance=(DWORD)GetModuleHandle(0);
    printf("initblop...\n");
  
#if _DEBUG
	HMODULE hm=LoadLibrary("BlopCore.dll");
#else
	HMODULE hm=LoadLibrary("BlopCoreR.dll");
#endif
	if(!hm)return 0;
    _InitBlop=(__InitBlop)GetProcAddress(hm,"?InitBlop@@YA_NAAUBlopStartInformation@@@Z");
	_StartBlop=(__BlopTick)GetProcAddress(hm,"?StartBlop@@YA_NXZ");
	_BlopTick=(__BlopTick)GetProcAddress(hm,"?BlopTick@@YA_NXZ");
	_BlopEnd =(__BlopEnd)GetProcAddress(hm,"?BlopEnd@@YAXXZ");
    if(!_InitBlop)return 0;

	bool ret=false;
	ret=bTouchLibrary("Modules/Sound/OpenAL32.dll");
    ret=bTouchLibrary("Modules/Sound/ogg.dll");
	ret=bTouchLibrary("Modules/Sound/vorbis.dll");
	ret=bTouchLibrary("Modules/Sound/vorbisfile.dll");
	ret=bTouchLibrary("Modules/Sound/EaxAC3.dll");
	

	if(!_InitBlop(BST))return 0;
	if(!_StartBlop())return 0;
    while(_BlopTick());
    //_BlopEnd();

	printf("end blop...");
//	sprintf("%s",NULL);
	return 0;
}