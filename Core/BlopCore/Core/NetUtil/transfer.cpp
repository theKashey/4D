/*===========================================================================
    Заголовок: Transport.cpp
    Автор: Kashey
    Информация: Level socket based transport

===========================================================================*/
//---------------------------------------------------------------------------


#include "ProjectSpace.h"
#include "Transfer.h"
#include "..\Sys\XMLmsg.h"

//---------------------------------------------------------------------------

DWORD SYS_TSYS=0;

DWORD BlopTransfer::Link(DWORD _Socket,DWORD _Type,sockaddr_in &_Addr,BlopBaseObject     *_Driver)
{
 Driver=_Driver;
 Socket=_Socket;
 Addr  =_Addr;
 Type  =_Type;
 Stock.Clear();
 if(!R_EVENT) R_EVENT=CreateEvent(NULL,0,0,NULL);
 return 1;
}

DWORD BlopTransfer::CreateSocket(DWORD _Type,WORD PortFrom, DWORD IPTo,WORD PortTo,BlopBaseObject     *_Driver)
{
 Driver=_Driver;
 Type=_Type;

 /*if(Type==BLOP_TRANSFER_TPC)
  Socket=socket(AF_INET, SOCK_STREAM, 0);else
 if(Type==BLOP_TRANSFER_UDP)
  Socket=socket(AF_INET, SOCK_DGRAM, 0);
 if(Socket==INVALID_SOCKET){ Driver->ConsoleOut->Printf("Unable to create socket"); return BERR_SOCKET_INVALID_CREATE;}
 Addr.sin_family      = AF_INET;
 Addr.sin_port        = htons(PortFrom);
 Addr.sin_addr.s_addr = htonl(INADDR_ANY);
 if(bind(Socket,(sockaddr*)&Addr, sizeof(sockaddr_in)) == SOCKET_ERROR){ Driver->ConsoleOut->Printf("Unable to bind");return BERR_SOCKET_BIND_ERROR;}
 Addr.sin_family      = AF_INET;
 Addr.sin_port        = htons(PortTo);
 Addr.sin_addr.s_addr = (IPTo);
 if(connect(Socket,(sockaddr*)&Addr, sizeof(sockaddr_in)) == SOCKET_ERROR){ Driver->ConsoleOut->Printf("Unable to connect");return BERR_SOCKET_CONNECT_ERROR;}
 if(!R_EVENT) R_EVENT=CreateEvent(NULL,0,0,NULL);
 */
 return 0;
}

DWORD BlopTransfer::SendMSG(const BlopMessage* Msg,DWORD DumpType)
{
 switch(DumpType)
 {
  case DUMP_NORMAL: Stock.Add(Msg); return 1;
  case DUMP_ALL   : Stock.Add(Msg); return Dump();
  case DUMP_THIS  : return _SendMSG(Msg);
 }
 return -1;
}

DWORD BlopTransfer::Dump()
{
 if(Stock.NumMessages<=0) return 0;
 Stock.ToBegin();
 const BYTE* Pointer;
 for(Pointer=Stock.ToBegin();Pointer;Stock.Next())_SendMSG((LPCBlopMessage)Pointer);
 Stock.Clear();
 return 1;
}

DWORD BlopTransfer::_SendMSG(const BlopMessage* Msg)
{
 if(!Msg) return -1;
 char BUFFER[MAX_BMSG_SIZE+1];
 switch(TargetSystem)
 {
  case SYSTEM_FLASH:
         return LoSend(BUFFER,MSG2XML(BUFFER,*Msg));
  default:
         BUFFER[0]='1';
         memcpy(&BUFFER[1],Msg,Msg->size());
         return LoSend(BUFFER,Msg->size()+1);
 }
}

DWORD BlopTransfer::LoSend(char* Buf,DWORD size)
{
 int sret=send(Socket,Buf,size,0);
// Driver->ConsoleOut->Printf(Buf+5);
// Driver->ConsoleOut->Printf("Sended %d bytes",sret);
 return sret;
}

DWORD MSG2MSG(BlopMessage* Msg,char *Buffer,DWORD &MSize,DWORD BSize)
{
 if(BSize<4) return false;
 //check message type
 if(Buffer[0]=='<') // XML message
 {
  if(!XML2MSG(Buffer,*Msg/*,MSize,BSize*/)) return 0;
  return strlen(Buffer)+1;
 }
 else // MSG5 message
 {
  Msg->Assign(*((BlopMessage*)&Buffer[1]));
  DWORD MS=Msg->size();
  return MS+1;
 }
}

bool  BlopTransfer::IsMessageInTail(BlopMessage* Msg,DWORD &Size)
{
 if(!ReadTailLen) return false;
 DWORD UsedLen;
 if((UsedLen=MSG2MSG(Msg,ReadTail,Size,ReadTailLen)))
 {
  ReadTailLen-=UsedLen;
  if(ReadTailLen)
   strncmp(ReadTail,ReadTail+UsedLen,ReadTailLen);
  else
   ReadTail[0]=0;
   return true;
 }
 return false;
}

void  BlopTransfer::ChargeRead()
{
 bzero(&R_OVERLAPPED,sizeof(R_OVERLAPPED));
 ResetEvent((HANDLE)R_EVENT);
 R_OVERLAPPED.hEvent=(HANDLE)R_EVENT;
 if(ReadTailLen)
 strncmp(BUFFER,ReadTail,ReadTailLen);
 BUFFERLen=ReadTailLen;
 ReadTailLen=0;
 ReadFile((HANDLE)Socket,BUFFER+BUFFERLen, sizeof(BUFFER)-BUFFERLen, 0, &R_OVERLAPPED);
}

bool  BlopTransfer::TryReadMSG(BlopMessage* Msg,DWORD &Size)
{
 if(!GetOverlappedResult((HANDLE)Socket, &R_OVERLAPPED, &ReadTailLen, FALSE)){return false;} //_!_
// Driver->ConsoleOut->Printf("Readed %d bytes",ReadTailLen);
 DWORD UsedLen=MSG2MSG(Msg,BUFFER,Size,ReadTailLen+BUFFERLen);
 ReadTailLen=ReadTailLen+BUFFERLen-UsedLen;
 if(ReadTailLen)
  memcpy(ReadTail,BUFFER+UsedLen,ReadTailLen);
 else
  ReadTail[0]=0;

 return UsedLen>0;
}

DWORD BlopTransfer::ReadMSG(BlopMessage* Msg,DWORD &Size,DWORD TimeOut)
{
 if(IsMessageInTail(Msg,Size)) return true;
 ChargeRead();
 int ret=WaitForSingleObject((HANDLE)R_EVENT,TimeOut);
 if(ret==WAIT_TIMEOUT || ret==-1) return 0;
 return TryReadMSG(Msg,Size);
}

DWORD BlopTransfer::CheckSimpleCommands(const BlopMessage* Msg)
{
 if(!Msg) return false;
 switch(Msg->Cmd)
 {
  case BLOP_ERROR      : Print2Console(SYS_TSYS,errErrorToString(Msg->AsDword()));return 1;
  case BLOP_HINT       : Print2Console(SYS_TSYS,errErrorToString(Msg->AsDword()));return 1;
  case BLOP_OK         : Print2Console(SYS_TSYS,errErrorToString(Msg->AsDword()));return 1;
  case BLOP_OK_NO_PRINT: return 1;

 }
 return 0;
}


