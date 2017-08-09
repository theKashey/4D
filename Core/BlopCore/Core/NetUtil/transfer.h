//---------------------------------------------------------------------------

#ifndef TransferH
#define TransferH
#include "ProjectSpace.h"
//---------------------------------------------------------------------------
#define BLOP_TRANSFER_TPC      1
#define BLOP_TRANSFER_UDP      2
#define BLOP_TRANSFER_UDP_Q    3

#define DUMP_NORMAL            0
#define DUMP_ALL               1
#define DUMP_THIS              2

#define SYSTEM_FLASH           1
#define SYSTEM_WINSOCK         2

struct NetAddr
{
 DWORD ViaIP;
 DWORD ViaPort;
 DWORD ToIp;
 DWORD ToPort;
};

class BlopTransfer
{
public:
 BlopBaseObject  *Driver;
 DWORD            TargetSystem;
 DWORD            Type;
 SOCKET           Socket;
 sockaddr_in      Addr;
 OVERLAPPED       R_OVERLAPPED;//,W_OVERLAPPED;
 HANDLE           R_EVENT     ;//,W_EVENT;

 BlopMessageStock Stock;

 char       BUFFER  [MAX_BMSG_SIZE+1];
 DWORD      BUFFERLen;
 char       ReadTail[0xFF];
 DWORD      ReadTailLen;
 
 DWORD Link(DWORD _Socket,DWORD _Type,sockaddr_in &_Addr,BlopBaseObject     *_Driver);
 DWORD CreateSocket(DWORD _Type,WORD PortFrom, DWORD IPTo,WORD PortTo,BlopBaseObject *_Driver);

 DWORD CheckSimpleCommands(const BlopMessage* Msg);

 DWORD SendMSG(const BlopMessage* Msg,DWORD DumpType=DUMP_NORMAL);
 DWORD _SendMSG(const BlopMessage* Msg);
 DWORD Dump();
 DWORD ReadMSG(BlopMessage* Msg,DWORD &Size,DWORD TimeOut);
 void  ChargeRead();
 bool  TryReadMSG(BlopMessage* Msg,DWORD &Size);

 bool  IsMessageInTail(BlopMessage* Msg,DWORD &Size);

 DWORD LoSend(char* Buf,DWORD size);
 DWORD LoRead(char* Buf);
 BlopTransfer()
 {R_EVENT=0;BUFFERLen=ReadTailLen=0;BUFFER[0]=ReadTail[0]=0;}

};
#endif
