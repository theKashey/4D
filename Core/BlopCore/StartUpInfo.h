#ifndef _BLOP_STARTUPINFO
#define _BLOP_STARTUPINFO

typedef void(*BlopConsoleOutFunction)(LPCSTR);
struct BlopLogStartup
 {
   BlopConsoleOutFunction ConOut;
   bool LogEnabled;
   bool UseLogFile;
   bool UseScreenOut;
   bool FileOpened;
   BlopLogStartup()
   {
	   LogEnabled   =true;
	   UseLogFile   =true;
       UseScreenOut=true;
	   FileOpened   =false;
   }
 };

struct BlopWindowStartUp
{
	bool CreateWindow;
	DWORD hInstance;
	BlopWindowStartUp()
	{
		CreateWindow=true;
		hInstance   =0;
	}
};

struct BlopModuleStartUp
{
    LPCSTR Window,
           Render;
    LPCSTR AddModules;
           
};

struct BlopStartInformation
{

 DWORD StartType;
 DWORD StartMode;
 DWORD StartData;

 BlopLogStartup         Log;
 BlopConsoleOutFunction CapOut;
 BlopWindowStartUp      Window;
 BlopModuleStartUp      Modules;
};

#endif