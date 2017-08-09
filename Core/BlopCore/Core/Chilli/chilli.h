//---------------------------------------------------------------------------

#ifndef ChilliH
#define ChilliH
//---------------------------------------------------------------------------

struct SessionLoginInformation
{
 DWORD PlayerID;
 DWORD SessionID,SessionCode;
 DWORD WorldID;
 DWORD RegionID;
 DWORD SectorID;
 NetAddr SignalAddr;
 NetAddr PlayerAddr;
 SessionLoginInformation()
 {SessionID=SessionCode=0;}
 SessionLoginInformation(DWORD S1,DWORD S2,NetAddr &SA)
 { };//SessionID=S1;SessionCode=S2;ServerAddr=SA;}

 bool IsValid() { return SessionID!=SessionCode && SessionCode!=0;}
};

class CChilli: public BlopBaseObject
{
public:
 CChilli(BlopUtilsPack &BU):BlopBaseObject(BU){};

 DWORD CreateSessionFor(DWORD ClientType,DWORD AccID,SessionLoginInformation &LI);
 DWORD CloseSession(DWORD  AccID);
};

#endif
