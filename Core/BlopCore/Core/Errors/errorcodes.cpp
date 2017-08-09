/*===========================================================================
    Заголовок: DLLManager.cpp
    Автор: Kashey
    Информация: ERROR CODES

===========================================================================*/
//---------------------------------------------------------------------------




#include "ProjectSpace.h"
#include "ErrorCodes.h"

//---------------------------------------------------------------------------

bool SOCKET_ERROR_STRING(DWORD ErrorCode,LPSTR CharBuf,LPDWORD Capapsy)
{
 switch(ErrorCode)
 {
  case BERR_SOCKET_INVALID_CREATE    :strcpy_s(CharBuf,0xFF,"UNABLE TO CREATE SOCKET");     return true;
  case BERR_SOCKET_BIND_ERROR        :strcpy_s(CharBuf,0xFF,"UNABLE TO BIND SOCKET");       return true;
  case BERR_SOCKET_CONNECT_ERROR     :strcpy_s(CharBuf,0xFF,"UNABLE TO CONNECT TO SOCKET");       return true;  
  case BERR_SOCKET_ERROR_LISTEN      :strcpy_s(CharBuf,0xFF,"UNABLE TO LISTEN SOCKET");     return true;
  case BERR_SOCKET_ERROR_READ        :strcpy_s(CharBuf,0xFF,"UNABLE TO READ FROM SOCKET");  return true;
  case BERR_SOCKET_ERROR_WRITE       :strcpy_s(CharBuf,0xFF,"UNABLE TO WRITE TO SOCKET");   return true;
  default                            :strcpy_s(CharBuf,0xFF,"UNKNOWN SOCKET ERROR");        return true;
 };
}

bool ACCBASE_ERROR_STRING(DWORD ErrorCode,LPSTR CharBuf,LPDWORD Capapsy)
{
 switch(ErrorCode)
 {
  case BERR_ACCBASE_CANT_GET_PASSWD   :strcpy_s(CharBuf,0xFF,"UNABLE TO GET PASSWORD FROM DATABASE"); return true;
  case BERR_ACCBASE_CANT_SET_PASSWD   :strcpy_s(CharBuf,0xFF,"UNABLE TO SET PASSWORD TO DATABASE");   return true;
  case BERR_ACCBASE_WRONG_PASSWD      :strcpy_s(CharBuf,0xFF,"WRONG PASSWORD FOR THIS ACCAUNT");      return true;
  case BERR_ACCBASE_NEW_PASSWD_SET    :strcpy_s(CharBuf,0xFF,"NEW PASSWORD BEIN SET");                return true;
  case BERR_ACCBASE_GOOD_PASSWD       :strcpy_s(CharBuf,0xFF,"PASSWORD PASS");                        return true;
  default                             :strcpy_s(CharBuf,0xFF,"UNKNOWN ACCBASE ERROR");                 return true;
 };
}

bool CONGUARD_ERROR_STRING(DWORD ErrorCode,LPSTR CharBuf,LPDWORD Capapsy)
{
 switch(ErrorCode)
 {
  case BERR_CONGUARD_SEND             :strcpy_s(CharBuf,0xFF,"PRIMA SEND FAILED IN CONGUARD");       return true;
  case BERR_CONGUARD_READ             :strcpy_s(CharBuf,0xFF,"PRIMA READ FAILED IN CONGUARD");       return true;
  case BERR_CONGUARD_CMD              :strcpy_s(CharBuf,0xFF,"WRONG CMD IN CONGUARD");               return true;
  case BERR_CONGUARD_ACCEPT           :strcpy_s(CharBuf,0xFF,"ACCEPT FAILED IN CONGUARD");           return true;
  case BERR_CONGUARD_UNFD             :
  default                             :strcpy_s(CharBuf,0xFF,"UNKNOWN CONNECTION GUARD ERROR");       return true;
 };
}


bool errErrorToString(DWORD ErrorCode,LPSTR CharBuf,LPDWORD Capapsy)
{
     if(ErrorCode>BERR_SOCKET_BEGIN     && ErrorCode<BERR_SOCKET_END)
        return SOCKET_ERROR_STRING                        (ErrorCode,CharBuf,Capapsy);
else if(ErrorCode>BERR_ACC_BASE_BEGIN   && ErrorCode<BERR_ACC_BASE_END)
        return ACCBASE_ERROR_STRING                       (ErrorCode,CharBuf,Capapsy);
else if(ErrorCode>BERR_CONGUARD_BEGIN   && ErrorCode<BERR_CONGUARD_END)
        return CONGUARD_ERROR_STRING                      (ErrorCode,CharBuf,Capapsy);
 return false;
}

char err_System_String[0xFF];

LPCSTR errErrorToString(DWORD ErrorCode)
{
 DWORD Cap=sizeof(err_System_String);
 errErrorToString(ErrorCode,err_System_String,&Cap);
 return err_System_String;
}


