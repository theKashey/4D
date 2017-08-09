//---------------------------------------------------------------------------

#ifndef ConnectManagerH
#define ConnectManagerH


#define CONNECT_LOGIN_TIMEOUT 2000


class CConnectionManager : public BaseObject
{
public:
 DWORD                 Terminated;

 bool                  FastConnect;
 DWORD                 Socket;
 WORD                  Port;
 sockaddr_in           ServAddr;
 DWORD                 AcceptCyclesFull;
 DWORD                 AcceptCyclesOk;
 DWORD                 AcceptCyclesFail;
 DWORD                 LastAcceptOkTime;
 DWORD                 LastAcceptFailTime;
 DWORD                 LastAcceptTime;

 CAccauntBase         *AccauntBase;
 CChilli              *Chilli;

 CConnectionManager(BlopUtilsPack &BU):BlopBaseObject(BU){};
 void SetAccauntBase(CAccauntBase *AB){ AccauntBase=AB;}
 void SetChilli     (CChilli *CH)     { Chilli=CH;}

 bool                Run();
 virtual DWORD       CreateSocket(WORD port){return 0;};
 virtual void        Loop()=0;
 void                CreateConnectGuard(DWORD SocketIn,sockaddr_in &addrUsed);
};

class CTCPConnectionManager: public CConnectionManager
{
public:
 CTCPConnectionManager(BlopUtilsPack &BU):CConnectionManager(BU){};
 DWORD               CreateSocket(WORD port);
 void                Loop();
};

class CServerConnectReader
{
public:
 CConnectionManager   *Driver;
 BlopTransfer         *Transfer;
 void Link(CConnectionManager *_Driver,BlopTransfer *_Transfer){Driver=_Driver;Transfer=_Transfer;}
 void TryServerConnectCommands(BlopMessage *Message,SessionLoginInformation &Session);
};

class CConnectGuard
{
public:
 DWORD               AccID;
 DWORD               ClientType;
 DWORD               ClientProto;

 CConnectionManager  *Driver;
 BlopTransfer         Transfer;
 CServerConnectReader ServerConnector;

 CConnectGuard(CConnectionManager* _Driver,DWORD _Socket,sockaddr_in &_Addr);
 void Run();

 void SetFail(DWORD FType);
 bool Accept(BlopMessage* Msg,DWORD *Key);

 DWORD  FindClientType (LPCSTR Str);
 DWORD  FindClientProto(LPCSTR Str);
 DWORD  FindAccID      (LPCSTR Str);
 bool   CheckPasswd    (DWORD AID,LPCSTR Passwd,DWORD *Key);
};

class CClientConnectReader
{
public:
 BlopBaseObject       *Driver;
 BlopTransfer         *Transfer;
 void Link(BlopBaseObject *_Driver,BlopTransfer *_Transfer){Driver=_Driver;Transfer=_Transfer;}
 void TryClientConnectCommands(BlopMessage *Message,LPCSTR Acc,LPCSTR Passw,LPCSTR HPass);
};

class CConnectKeeper
{
 public:
  BlopBaseObject       *Driver;
  BlopTransfer         Transfer;
  CClientConnectReader ClientConnector;
  CConnectKeeper(BlopTransfer &Tr,BlopBaseObject *_Driver);

  void SetFail(DWORD FType);
  DWORD Login(LPCSTR Name,LPCSTR Pass,SessionLoginInformation &Session);
  DWORD Spawn(DWORD ServerID);
};


//---------------------------------------------------------------------------
#endif
