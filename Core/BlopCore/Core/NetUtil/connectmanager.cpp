/*===========================================================================
    Заголовок: ConnectManager.cpp
    Автор: Kashey
    Информация: Net connection keeper

===========================================================================*/
//---------------------------------------------------------------------------
#include "ProjectSpace.h"
#include "Transfer.h"

#include "..\Containers\AccauntBase.h"
#include "..\Chilli\Chilli.h"

#include "ConnectManager.h"
#include "..\Sys\HashLib.h"




//---------------------------------------------------------------------------

WORD DEFAULT_CONNECTION_MANAGER_PORT=6070;

DWORD SYS_CM=0;


_DeclareObjectedThreadFunc(CConnectionManager,Loop,conmanLoop)


bool CConnectionManager::Run()
{
 Terminated=0;
 switch(CreateSocket(DEFAULT_CONNECTION_MANAGER_PORT))
 {
  case 1                       :Print2Console(SYS_CM,"Connection manager ready on port %d",DEFAULT_CONNECTION_MANAGER_PORT);break;
  case BERR_SOCKET_BIND_ERROR  :Print2Console(SYS_CM,"Connection manager unable to bind socket");     return false;
  case BERR_SOCKET_ERROR_LISTEN:Print2Console(SYS_CM,"Connection manager unable to listen socket");   return false;

 }
 HANDLE ret=_StartObjectedThread(conmanLoop);
 if(!ret)
  return false;

 return true;
}

void CConnectionManager::CreateConnectGuard(DWORD SocketIn,sockaddr_in &addrUsed)
{
 //if option is FastIn create thread
 //else check in main win
// Print2Console(SYS_CM,"new connection spawned");
 CConnectGuard CG(this,SocketIn,addrUsed);
 //if(FastConnect)
 // Vfork(CG.Run());
 //else
 CG.Run();
}

DWORD CTCPConnectionManager::CreateSocket(WORD port)
{
 Port=port;

 Socket=socket(AF_INET, SOCK_STREAM, 0);
 if(Socket==INVALID_SOCKET) return BERR_SOCKET_INVALID_CREATE;
 ServAddr.sin_family      = AF_INET;
 ServAddr.sin_port        = htons(Port);
 ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 if(bind(Socket,(sockaddr*)&ServAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)return BERR_SOCKET_BIND_ERROR;
 if(listen(Socket,10)==-1)return BERR_SOCKET_ERROR_LISTEN;
 return 1;
}

void CTCPConnectionManager::Loop()
{
 sockaddr_in addr;
 SOCKET sockClient;
 int ASize;
 while(!Terminated)
 {
     AcceptCyclesFull++;
     ASize=sizeof(addr);
     if((sockClient=accept(Socket,(sockaddr*)&addr,&ASize))!=SOCKET_ERROR)
      CreateConnectGuard(sockClient,addr);
 }
}

CConnectGuard::CConnectGuard(CConnectionManager* _Driver,DWORD _Socket,sockaddr_in &_Addr)
{
 Driver=_Driver;
 Transfer.Link(_Socket,BLOP_TRANSFER_TPC,_Addr,Driver);
}

//fails
#define FAIL_SEND     1
#define FAIL_READ     2
#define FAIL_CMD      3
#define FAIL_ACCEPT   4

void CServerConnectReader::TryServerConnectCommands(BlopMessage *Message,SessionLoginInformation &Session)
{
 if(!Message) return;
 if(Message->Cmd==BLOP_CONNECT)
 {
  if(Fstrcmp(Message->Data,"PST")==0)
  {
   DynamicLocker DL(*Driver->AccauntBase);
   Driver->AccauntBase->SetAccPasswd(Session.PlayerID,Message->Data+4);
  }
 }
}

void CConnectGuard::Run()
{
 //send 4 dwords
 DWORD CData[5];
 CData[0]='C';CData[1]=rand();CData[2]=rand();CData[3]=rand();CData[5]=rand();
 BlopMessage *BM=CreateBlopMessage(0,CData,4*5);
 if(!Transfer.SendMSG(BM,DUMP_ALL)){SetFail(FAIL_SEND);return;}
 DeleteBlopMessage(BM);
 char MBIN[1024];
 DWORD BSize=sizeof(MBIN);
 if(!Transfer.ReadMSG((BlopMessage*)MBIN,BSize,CONNECT_LOGIN_TIMEOUT)){ SetFail(FAIL_READ); return;}
 BM=(BlopMessage*)MBIN;
 if(BM->Cmd!=BLOP_CONNECT) { SetFail(FAIL_CMD   ); return;}
 if(!Accept(BM,CData+1)){ SetFail(FAIL_ACCEPT); return;}
// NetAddr NewServer;
 //DWORD S1,S2;
 SessionLoginInformation LI;
// Driver->Chilli->CreateSessionFor(ClientType,AccID,LI);
 LI.PlayerID=AccID;
 char LIb[4+sizeof(LI)];
 memcpy(LIb+4,&LI,sizeof(LI));
 strcpy_s(LIb,0xFF,"ENT");
 BM=CreateBlopMessage(BLOP_CONNECT,&LIb,sizeof(LI)+4);
 Transfer.SendMSG(BM);
// Print2Console(SYS_CM,"Session sended");
 DeleteBlopMessage(BM);
// Print2Console(SYS_CM,"<<");
 Transfer.Dump();
// Print2Console(SYS_CM,">>");
 DWORD tm=CONNECT_LOGIN_TIMEOUT;
 ServerConnector.Link(Driver,&Transfer);
 while(1)
 {
  if(!Transfer.ReadMSG((BlopMessage*)MBIN,BSize,tm)){ SetFail(FAIL_READ); return;}
  BM=(BlopMessage*)MBIN;
  ServerConnector.TryServerConnectCommands(BM,LI);
  tm=1;
 }
}

void CConnectGuard::SetFail(DWORD FType)
{
 BlopMessage *BM;
 switch(FType)
 {
  case FAIL_SEND  : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_SEND))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_SEND);   break;
  case FAIL_READ  : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_READ))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_READ);   break;
  case FAIL_CMD   : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_CMD))   ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_CMD);    break;
  case FAIL_ACCEPT: Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_ACCEPT));BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_ACCEPT); break;
  default         : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_UNFD))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_UNFD);   break;
 }
 Transfer.SendMSG(BM);
 DeleteBlopMessage(BM);
}

//#define _get(A) Bc++;Bs=Be-Bc;Valid=0;for(x=0;x<Bs;x++) if(Bc[x]==']'){ strncpy(A,Bc,x); Valid=1;for(DWORD y=x+1;y<Bs;y++)if(Bc[y]=='['){Bc=&Bc[y];Valid=2;break;} if(Valid==1)Bc=&Bc[x+2];break;}

void _get(LPSTR A,LPSTR &Bc,LPCSTR Be,BYTE &Valid)
{
 ++Bc;
 int Bs=Be-Bc;
 Valid=0;
 for(int x=0;x<Bs;x++)
  if(Bc[x]==']')
  {
   strncpy_s(A,x+2,Bc,x);
   Valid=1;
   for(int y=x+1;y<Bs;y++)
     if(Bc[y]=='[')
     {
      Bc=&Bc[y];
      Valid=2;
      break;
    }
   if(Valid==1)
    Bc=&Bc[x+2];
   break;
 }
}

bool CConnectGuard::Accept(BlopMessage* Msg,DWORD* Key)
{
 char *Message=Msg->Data;
 // data type
 //[ClientType] [ClientProto] [AccName] [AccPass]

 //get client type
 char CType  [0xFF];
 char CProto [0xFF];
 char AccName[0xFF];
 char AccPass[0xFF];
 ZeroMemory(CType  ,sizeof(CType));
 ZeroMemory(CProto ,sizeof(CProto));
 ZeroMemory(AccName,sizeof(AccName));
 ZeroMemory(AccPass,sizeof(AccPass));
 char *Bc=Message;
 char *Be=Message+strlen(Message);
 BYTE Valid=0;
 Bc++;
 _get(CType,Bc,Be,Valid);   if(!Valid){Print2Console(SYS_CM,"no [ClientType] section found" ); return false;}
 _get(CProto,Bc,Be,Valid);  if(!Valid){Print2Console(SYS_CM,"no [ClientProto] section found"); return false;}
 _get(AccName,Bc,Be,Valid); if(!Valid){Print2Console(SYS_CM,"no [AccName] section found"    ); return false;}
 _get(AccPass,Bc,Be,Valid); if(!Valid){Print2Console(SYS_CM,"no [AccPass] section found"    ); return false;}

 ClientType =FindClientType (CType);
 ClientProto=FindClientProto(CProto);
 AccID      =FindAccID      (AccName);
 return CheckPasswd(AccID,AccPass,Key);
}

DWORD  CConnectGuard::FindClientType (LPCSTR Str)
{
 return 0;
}

DWORD  CConnectGuard::FindClientProto(LPCSTR Str)
{
 return 0;
}

DWORD  CConnectGuard::FindAccID      (LPCSTR Str)
{
 DynamicLocker DL(*Driver->AccauntBase);
// Print2Console(SYS_CM,"loging %s player",Str);
 DWORD iad=Driver->AccauntBase->FindAcc(Str);
// Print2Console(SYS_CM,"Player '%s' have %X id",Str,iad);
 return iad;
}

bool   CConnectGuard::CheckPasswd    (DWORD AID,LPCSTR Passwd,DWORD *Key)
{
 if(!AID) return false;
 DynamicLocker DL(*Driver->AccauntBase);
 DWORD PC=Driver->AccauntBase->CheckPasswd(AID,Passwd,Key);
 BlopMessage *BM;
 switch(PC)
 {
  case 0:  Print2Console(SYS_CM,errErrorToString(BERR_ACCBASE_CANT_GET_PASSWD));BM=CreateBlopMessage(BLOP_ERROR,BERR_ACCBASE_CANT_GET_PASSWD);   break;
  case 1:  Print2Console(SYS_CM,errErrorToString(BERR_ACCBASE_CANT_SET_PASSWD));BM=CreateBlopMessage(BLOP_ERROR,BERR_ACCBASE_CANT_SET_PASSWD);   break;
  case 2:  Print2Console(SYS_CM,errErrorToString(BERR_ACCBASE_WRONG_PASSWD   ));BM=CreateBlopMessage(BLOP_ERROR,BERR_ACCBASE_WRONG_PASSWD);      break;
  case 3:  Print2Console(SYS_CM,errErrorToString(BERR_ACCBASE_NEW_PASSWD_SET ));BM=CreateBlopMessage(BLOP_CONNECT,"PRQ");                        break;
  case 4:  Print2Console(SYS_CM,errErrorToString(BERR_ACCBASE_GOOD_PASSWD    ));BM=CreateBlopMessage(BLOP_OK     ,BERR_ACCBASE_GOOD_PASSWD);     break;
 }
 Transfer.SendMSG(BM);
 DeleteBlopMessage(BM);
 return PC>=3;
}

void CClientConnectReader::TryClientConnectCommands(BlopMessage *Message,LPCSTR Acc,LPCSTR Passw,LPCSTR HPass)
{
 if(!Message) return;
 if(Fstrcmp(Message->Data,"PRQ")==0)
 {
   GString ConString;
   ConString.Format("PST %s",Passw);
   WORD SLen=ConString.GetLength();
   ConString.SetAt(3,0);
   BlopMessage *BM;
   BM=CreateBlopMessage(BLOP_CONNECT,ConString.c_str(),SLen+1);
   Transfer->SendMSG(BM,DUMP_ALL);
   DeleteBlopMessage(BM);
 }
}

CConnectKeeper::CConnectKeeper(BlopTransfer &Tr,BlopBaseObject *_Driver)
{
 Driver=_Driver;
 Transfer=Tr;
 ClientConnector.Link(Driver,&Transfer);
}

void CConnectKeeper::SetFail(DWORD FType)
{
 BlopMessage *BM;
 switch(FType)
 {
  case FAIL_SEND  : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_SEND))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_SEND);   break;
  case FAIL_READ  : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_READ))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_READ);   break;
  case FAIL_CMD   : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_CMD))   ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_CMD);    break;
  case FAIL_ACCEPT: Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_ACCEPT));BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_ACCEPT); break;
  default         : Print2Console(SYS_CM,errErrorToString(BERR_CONGUARD_UNFD))  ;BM=CreateBlopMessage(BLOP_ERROR,BERR_CONGUARD_UNFD);   break;
 }
 Transfer.SendMSG(BM);
 DeleteBlopMessage(BM);
}

DWORD CConnectKeeper::Login(LPCSTR Name,LPCSTR Pass,SessionLoginInformation &SessionID)
{
 char MBIN[1024];
 DWORD BSize=sizeof(MBIN);
 //read sync message
 if(!Transfer.ReadMSG((BlopMessage*)MBIN,BSize,CONNECT_LOGIN_TIMEOUT)){ SetFail(FAIL_READ); return BERR_CONKEEPER_NO_RESPONSE;}
 BlopMessage *BM;
 BM=(BlopMessage*)MBIN;
 char Passwd[1024];
 __Hash1Prepare(Pass,Passwd,(DWORD*)BM->Data+1);
 __Hash1(Passwd,MBIN);
 GString ConString;
 ConString.Format("L[Blop] [TCP_Value1] [%s] [%s]",Name,MBIN);
 BM=CreateBlopMessage(BLOP_CONNECT,ConString.c_str(),ConString.GetLength()+1);
 if(!Transfer.SendMSG(BM,DUMP_ALL)){SetFail(FAIL_SEND);return 1;}
 DeleteBlopMessage(BM);
 DWORD tm=CONNECT_LOGIN_TIMEOUT;
 while(1)
 {
 if(!Transfer.ReadMSG((BlopMessage*)MBIN,BSize,tm))break;
 BM=(BlopMessage*)MBIN;
 tm=1;
 if(Transfer.CheckSimpleCommands(BM)){ continue;}
// if(BM->Cmd!=BLOP_CONNECT) return BERR_CONKEEPER_SYNC_FAILED;
 if(Fstrcmp(BM->Data,"ENT")==0)
 {
  SessionID= *(SessionLoginInformation*)((DWORD*)BM->Data+4);
  Print2Console(SYS_CM,"Session indeficator resived");
 }
 else
  ClientConnector.TryClientConnectCommands(BM,Name,Pass,Passwd);
}
 Print2Console(SYS_CM,"ConMan - leave");
 return 0;
}

DWORD CConnectKeeper::Spawn(DWORD ServerID)
{
 return 1;
}

