//---------------------------------------------------------------------------
//#pragma once

//#ifndef unuse_PEX

#ifndef ProjectExH
#define ProjectExH
//---------------------------------------------------------------------------

#pragma pack(push,1)

struct blopTimeStamp
{
 BYTE YEAR;
 BYTE Month;
 BYTE Day;
 BYTE Hour;
 BYTE Minute;
 BYTE Second;

 void SetNOW();
};

#pragma pack(pop)



//#include "Memory\SmartPtr.h"

#include "BlopMessages.h"
#include "MessageCommands.h"
#include "Math\LtlMath.h"
#include "Vectors\VectorBase.h"
#include "SmartVector.h"
#include "ProjectMemory.h"
#include "strings\bStrings.h"
#include "Threads\blopThreads.h"
#include "SectionLocker.h"
#include "ServerUtils.h"
#include "..\blopBaseObject.h"
#include "..\System\DLLManager.h"
#include "..\Loaders\bFiles.h"  //FileSys.h"
#include "..\Sound\SoundMe.h"
#include "Profiler.h"

#include "..\Plugins\Pluginloader.h"

#include "ProjectDividors.h"

#include "Counters\counters.h"

/**/
//#include "Forms\BaseForms.h"

#define AssertIfGREATER(Condition,Message,OnOk,OnFall,RetFall)\
	    if((Condition)>0){ ConsoleOut->Printf(Message,OnOk);RetFall;}\
    	else ConsoleOut->Printf(Message,OnFall);

namespace Time
{
	double DLLTYPE Get();
	DWORD  DLLTYPE GetD();
	double DLLTYPE Time();
	void   SetThisTime();
};
//int   DLLTYPE RegisterNewConsoleClass(LPCSTR Name,int Class);
#define RegisterNewConsoleClass(a,b,c)inRegisterNewConsoleClass(a,#a,b,c)
void  DLLTYPE inRegisterNewConsoleClass(DWORD &SYS,LPCSTR VName,LPCSTR Name,int Class);

void  DLLTYPE Print2Console  (DWORD System,LPCSTR TextMessage);
void  DLLTYPE Error2Console  (DWORD System,LPCSTR TextMessage);
void  DLLTYPE Warning2Console(DWORD System,LPCSTR TextMessage);

template<class A>void  DLLTYPE Print2Console  (DWORD System,LPCSTR Format,A B,...){DOFORMAT(a,Format);Print2Console(System,a)  ;return;}
template<class A>void  DLLTYPE Error2Console  (DWORD System,LPCSTR Format,A B,...){DOFORMAT(a,Format);Error2Console(System,a)  ;return;}
template<class A>void  DLLTYPE Warning2Console(DWORD System,LPCSTR Format,A B,...){DOFORMAT(a,Format);Warning2Console(System,a);return;}


namespace inTHREAD
{
	void  DLLTYPE Print2Console  (DWORD System,LPCSTR Format,...);
    void  DLLTYPE Error2Console  (DWORD System,LPCSTR Format,...);
    void  DLLTYPE Warning2Console(DWORD System,LPCSTR Format,...);
};

#ifdef isTHREAD
#define Print2Console   inTHREAD::Print2Console
#define Error2Console   inTHREAD::Error2Console
#define Warning2Console inTHREAD::Warning2Console
#endif

void  DLLTYPE SetDefaultConsole(ConsoleUtil* Console);

namespace ConsoleSystems
{
  DWORD DLLTYPE Plugin();
  DWORD DLLTYPE Util();
};

namespace VARS
{
 struct DLLTYPE TFVAR
 {
 private:
     double Val;
 public:
     typedef const double cdouble;
     typedef const DWORD cdword;
     typedef double * pdouble;
     operator cdouble();
     operator pdouble();
//     operator cdword ();
     double operator =(double src);
 };

struct DLLTYPE TSVAR
 {
 private:
     GString Val;
 public:
     //operator    LPCSTR()const;
     typedef GString & GStringRef;
     operator GStringRef();
     LPCSTR operator =(LPCSTR src);
 };
 
 typedef TFVAR &TFVARref;
 typedef TSVAR &TSVARref;

 TFVARref DLLTYPE fvars(DWORD &V,LPCSTR Name=NULL);
 TSVARref DLLTYPE svars(DWORD &V,LPCSTR Name=NULL);
 struct CVarNameRefOut
 {
   char  Name[256];
   BYTE  Type;
   DWORD ID;
 };
 typedef const CVarNameRefOut& CVarNameRefOutRef;
 CVarNameRefOutRef DLLTYPE GetVarByName(LPCSTR Name);
 /*
 TDVAR &dvars(DWORD &V,LPCSTR Name=NULL);
 TAVAR &avars(DWORD &V,LPCSTR Name=NULL);
 */

 extern DWORD fv_FPS;
 extern DWORD fv_FTIME_d_20;
 extern DWORD fv_FRAMETIME;
 extern DWORD fv_GAMETIME;
 extern DWORD fv_TIMEFACTOR;
 extern DWORD fv_FSPAN_d_20;

 //float DLLTYPE FrameTime

#define REGFVAR(a,b) VARS::fvars(a,b);

 void Init();
};

#endif

//#endif

