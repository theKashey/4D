//---------------------------------------------------------------------------

#ifndef ServerUtilsH
#define ServerUtilsH
//---------------------------------------------------------------------------

#define BLOP_MAIN_SERVER 1
#define BLOP_MAIN_CLIENT 2


#include "StartUpInfo.h"

class  ConsoleUtil
{
 
 SectionLocker Locker;
 BlopConsoleOutFunction ConOut;

 bool LogEnabled;
 bool UseLogFile;
 bool UseScreenOut;
 bool FileOpened;
 bool AddTime;

 GString StartString;
 //GString OutString;

 bool OpenLogFile();
 void CloseLogFile();
 FILE* FileID;
public:
 ConsoleUtil(BlopLogStartup &BLS,LPCSTR _StartString);
~ConsoleUtil();
 void Printf(LPCSTR Format,...);
};

#endif
