//---------------------------------------------------------------------------




#include "ProjectSpace.h"
#include "netutils.h"



//---------------------------------------------------------------------------
DWORD   _ip_from_name(LPCSTR Name)
{
 hostent *hp;
 hp = gethostbyname(Name);
 if(!hp)return 0;
 DWORD ret;
 memcpy(&ret,hp->h_addr, hp->h_length);
 delete hp;
 return ret;
}

char thisIDtosIPBuf[4][24];
DWORD CX=0;

LPCSTR  _ip_to_string(DWORD IP)
{
 typedef struct { BYTE a1,a2,a3,a4;}_NA;
 _NA *na=(_NA*)&IP;
 if(CX>3) CX=0;
 sprintf_s(thisIDtosIPBuf[CX],24,"%d.%d.%d.%d",na->a1,na->a2,na->a3,na->a4);
 return thisIDtosIPBuf[CX++];
}

LPCSTR  _ip_to_name  (DWORD IP)
{
 return "UNKNOWN";
}



