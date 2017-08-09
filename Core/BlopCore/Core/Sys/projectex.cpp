/*===========================================================================
    Заголовок: PrjectEx.cpp
    Автор: Kashey
    Информация: Ex header

===========================================================================*/
//---------------------------------------------------------------------------




#include "ProjectSpace.h"
#include "ProjectEx.h"
#include <time.h>



//---------------------------------------------------------------------------


void blopTimeStamp::SetNOW()
{
 tm* rtime;
 const time_t rt=time(NULL);
 rtime=localtime(&rt);
 YEAR   =rtime->tm_year %10;
 Month  =rtime->tm_mon;
 Day    =rtime->tm_mday;
 Hour   =rtime->tm_hour;
 Minute =rtime->tm_min;
 Second =rtime->tm_sec;
}


namespace Time
{

static double THISTIME=0;


static double PerformanceFrequency=0;
static LARGE_INTEGER _PerformanceFrequency;
static LARGE_INTEGER _Time,_StartTime;
static DWORD StartTimeTime=0;

static bool TIMEINITED=false;

static void DOTIMEINIT()
{
	if(!TIMEINITED)
	{
		QueryPerformanceFrequency(&_PerformanceFrequency);
        PerformanceFrequency=1000.0/_PerformanceFrequency.QuadPart;
        QueryPerformanceCounter(&_StartTime);
		StartTimeTime=timeGetTime();
		TIMEINITED=true;
	}
}


void SetThisTime()
{
	THISTIME=Time::Get();
}

double Get()
{
 DOTIMEINIT();
 QueryPerformanceCounter(&_Time);
 double DT=(PerformanceFrequency*(_Time.QuadPart -_StartTime.QuadPart));
 static double LASTDT=DT;
 if(DT-LASTDT<0.0000001) DT=LASTDT;
 else LASTDT=DT;
 return DT;
}

DWORD  GetD()
{
 DOTIMEINIT();
 return timeGetTime()-StartTimeTime;
}

double Time()
{
	return THISTIME;
}

}



SmartVector<GString> ConsoleClasses;
ConsoleUtil* DefaultConsole;
int MaxConsoleClass=0;


//int RegisterNewConsoleClass(LPCSTR Name,int Class)
void inRegisterNewConsoleClass(DWORD &SYS,LPCSTR VName,LPCSTR Name,int Class)
{
	if(Class==-1) Class=++MaxConsoleClass;
	//if(MaxConsoleClass<=Class)MaxConsoleClass=Class+2; 
	ConsoleClasses.SetSize(MaxConsoleClass+1);
	ConsoleClasses[Class].HardEmpty();
    ConsoleClasses[Class]=Name;
	lua::setdword(VName,Class);
	//return Class;
	SYS=Class;
}

DWORD WINAPI  DoPrint(DWORD ID)
{
  LPSTR STR=(LPSTR)ID;
  DefaultConsole->Printf("%s",STR);	
  delete []STR;
  return 0;
}

void Print2Console(DWORD System,LPCSTR TextMessage)
{
   if(DefaultConsole && TextMessage)
   {
	   
	   LPCSTR szError=TextMessage;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=TextMessage;
	   DefaultConsole->Printf("%s",(LPCSTR)Str);
   }
}


void Error2Console(DWORD System,LPCSTR TextMessage)
{
   if(DefaultConsole)
   {
	   LPCSTR szError=TextMessage;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=TextMessage;
	   DefaultConsole->Printf("ERR:%s",(LPCSTR)Str);
   }
}


void Warning2Console(DWORD System,LPCSTR TextMessage)
{
   if(DefaultConsole)
   {
	   LPCSTR szError=TextMessage;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=TextMessage;
	   DefaultConsole->Printf("WRN:%s",(LPCSTR)Str);
   }
}

namespace inTHREAD
{

void Print2Console(DWORD System,LPCSTR Format,...)
{
   if(DefaultConsole)
   {
	   DOFORMAT(Err,Format);
	   LPCSTR szError=Err;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=Err;
	   char *S=new char[Str.length()+1];
	   strcpy_s(S,Str.length()+1,Str);
	   QueueUserAPC(PAPCFUNC(DoPrint), QueryMainThreadHandle(),(DWORD)S);
   }
}

void Error2Console(DWORD System,LPCSTR Format,...)
{
   if(DefaultConsole)
   {
	   DOFORMAT(Err,Format);
	   LPCSTR szError=Err;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=Err;
	   char *S=new char[Str.length()+1];
	   strcpy_s(S,Str.length()+1,Str);
	   QueueUserAPC(PAPCFUNC(DoPrint), QueryMainThreadHandle(),(DWORD)S);
   }
}

void Warning2Console(DWORD System,LPCSTR Format,...)
{
   if(DefaultConsole)
   {
	   DOFORMAT(Err,Format);
	   LPCSTR szError=Err;
       GString Str;
       if(System!=(DWORD)-1)
       {
	     GString &CN=ConsoleClasses[System];
	     if(CN.c_str()==NULL)CN="ROOT";
	     Str=CN+":"+szError;
       }else Str=Err;
	   char *S=new char[Str.length()+1];
	   strcpy_s(S,Str.length()+1,Str);
	   QueueUserAPC(PAPCFUNC(DoPrint), QueryMainThreadHandle(),(DWORD)S);
   }
}

};

void SetDefaultConsole(ConsoleUtil* Console)
{
   DefaultConsole=Console;
}

/*#undef new
#define new new

PVOID DLLTYPE operator new(size_t nSize, LPCSTR lpszFileName,LPCSTR Function, int nLine)
{
	return malloc(nSize);
	//return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}

void operator delete(PVOID Pointer)//, LPCSTR lpszFileName, LPCSTR Function,int nLine)
{
   free(Pointer);
}


void DLLTYPE operator delete(PVOID Pointer, LPCSTR lpszFileName, LPCSTR Function,int nLine)
{
   free(Pointer);
}
*/

namespace ConsoleSystems
{

#define DOPLUG(a) static DWORD Sys=0;if(!Sys)RegisterNewConsoleClass(Sys,a,NULL);return Sys;

 DWORD Plugin()
 {
   DOPLUG("PLG");
 }

 DWORD Util()
 {
  DOPLUG("Util");
 }
};

namespace VARS
{
 TFVAR::operator TFVAR::cdouble(){return Val;}
 TFVAR::operator TFVAR::pdouble(){return &Val;}
// TFVAR::operator cdword (){return (DWORD)Val;}
 double TFVAR::operator =(double src){Val=src;return Val;}
                                                  

 TSVAR::operator GStringRef(){return Val;}
LPCSTR TSVAR::operator =(LPCSTR src){Val=src;return 0;}

 SmartVector<TFVAR> FVars;
 SmartVector<TSVAR> SVars;

 struct CVarNameRefIn
 {
   GString Name;
   BYTE    Type;
   DWORD   ID;
   static CVarNameRefIn& Make(LPCSTR N,BYTE T,DWORD I)
       {
         static CVarNameRefIn Tmp;
         Tmp.Name=N;
         Tmp.Type=T;
         Tmp.ID  =I;
         return Tmp;
       }
 };

 typedef SmartVector<CVarNameRefIn>  RefVector;
 RefVector NameRef;

 TFVARref fvars(DWORD &V,LPCSTR Name)
 {
  if(!V)
  {
    static TFVAR F;
    FVars.push_back(F);
    V=FVars.size();
    if(Name)
        NameRef.push_back(CVarNameRefIn::Make(Name,1,V));
  }
  return FVars[V-1];
 }
 TSVARref svars(DWORD &V,LPCSTR Name)
 {
 if(!V)
  {
    static TSVAR F;
    SVars.push_back(F);
    V=SVars.size();
    if(Name)
        NameRef.push_back(CVarNameRefIn::Make(Name,2,V));
  }
  return SVars[V-1];
 }


 CVarNameRefOutRef GetVarByName(LPCSTR Nam)
 {
  static CVarNameRefOut Def;
  GString Name=Nam;
  for(RefVector::Citerator it=NameRef.begin();it!=NameRef.end();++it)
      if(it->Name==Name)
          {
           strcpy_s(Def.Name,sizeof(Def.Name),it->Name);
           Def.Type=it->Type;
           Def.ID  =it->ID;
           return Def;
          }
  Def.ID=0; 
  return Def;
 }

 DWORD fv_FPS            =0;
 DWORD fv_FTIME_d_20     =0;
 DWORD fv_FRAMETIME      =0;
 DWORD fv_GAMETIME       =0;
 DWORD fv_TIMEFACTOR     =0;
 DWORD fv_FSPAN_d_20     =0;

 void Init()
 {
   REGFVAR(fv_FPS         ,"FPS");
   REGFVAR(fv_FTIME_d_20  ,"Frame.Time20");
   REGFVAR(fv_FSPAN_d_20  ,"Frame.Span20");
   REGFVAR(fv_FRAMETIME   ,"Frame.Time");
   REGFVAR(fv_GAMETIME    ,"Game.Time"); 
   REGFVAR(fv_TIMEFACTOR  ,"Time.Factor");
 }
};
