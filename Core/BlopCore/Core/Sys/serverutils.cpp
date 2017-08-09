/*===========================================================================
    Заголовок: ServerUtils.cpp
    Автор: Kashey
    Информация: Console

===========================================================================*/
//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProjectSpace.h"
#include "ServerUtils.h"
#include <time.h>

//---------------------------------------------------------------------------

ConsoleUtil::ConsoleUtil(BlopLogStartup &BLS,LPCSTR _StartString)
{
 StartString=_StartString;
 LogEnabled  =BLS.LogEnabled;
 UseLogFile  =BLS.UseLogFile;
 UseScreenOut=BLS.UseScreenOut;
 ConOut      =BLS.ConOut;
 FileOpened  =false;
 AddTime     =false;
}

ConsoleUtil::~ConsoleUtil()
{
 CloseLogFile();
}

void ConsoleUtil::Printf(LPCSTR Format,...)
{
 DynamicLocker DL(Locker);

 if(!LogEnabled) return;
 if(!FileOpened && UseLogFile)
  if(!OpenLogFile()) UseLogFile=false;


 /*va_list ap;
 va_start(ap, Format);
 vsprintf(szError, Format, ap);
 va_end(ap);
 */
 DOFORMAT(Err,Format);
 LPCSTR szError=Err;
 tm* rtime;
 const time_t rt=time(NULL);
 rtime=localtime(&rt);
 GString Res;
 if(AddTime)
	 Res.Format("%04u/%02u/%02u;%02u:%02u:%02u >> %s",
                  rtime->tm_year, rtime->tm_mon, rtime->tm_mday,
                  rtime->tm_hour, rtime->tm_min, rtime->tm_sec,szError);
 else
     Res=szError;
 GString OutString=/*StartString+":"+*/Res;
 if(UseLogFile)
 {
  if(OutString[OutString.length()]=='!' &&
     OutString[OutString.length()-1]=='\n')
          fprintf(FileID,"%s",OutString.c_str());
  else
          fprintf(FileID,"%s\n",OutString.c_str());
  fflush(FileID);
 }
 if(UseScreenOut)
 {
  ConOut(OutString);
 }
}

bool ConsoleUtil::OpenLogFile()
{
 if(fopen_s(&FileID,StartString+"Blop.log","w"))FileID=NULL;
 if(FileID)FileOpened=true;
 return FileID>0;
}

void ConsoleUtil::CloseLogFile()
{
 fclose(FileID);
 FileOpened=false;
}
