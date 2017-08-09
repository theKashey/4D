//---------------------------------------------------------------------------

#ifndef AccauntBaseH
#define AccauntBaseH


#pragma pack(push,1)

struct CPlayerFileStore
{
 char          PlayerName[0xFF];
 char          PlayerPass[0xFF];
 DWORD         PlayerID;
 blopTimeStamp FirstConnection;
 blopTimeStamp LastConnection;
 DWORD         NumConnections;
};

struct CPlayerFileStore_ex: public CPlayerFileStore
{
 BYTE Removable;
};

#pragma pack(pop)

class CAccauntBase: public SectionLocker
{
 SmartVector<CPlayerFileStore> PlayerAccCashe;
 DWORD FindAccInCashe       (LPCSTR AccName,CPlayerFileStore *ARef);
 DWORD FindAccInCasheByID   (DWORD  AID    ,CPlayerFileStore *ARef);
 DWORD AddAccToCashe        (CPlayerFileStore &ARef);
 DWORD FindAccInDataBase    (LPCSTR AccName,CPlayerFileStore *ARef);
 DWORD CreateAccInDataBase  (LPCSTR AccName,CPlayerFileStore *ARef);
 DWORD UpdatePlayerAccInBase(CPlayerFileStore &ARef);

 int                        AccDatabaseFile;
 bool                       CheckFileOpen();
 bool                       OpenFile();
 bool                       CloseFile();

 BYTE                      AddAccIfNotFound;

 GString                   DataBaseFileName;
 DWORD                     GetNextAccID();

public:
 CAccauntBase();
 DWORD FindAcc             (LPCSTR AccName);
 DWORD CheckPasswd         (DWORD AID,LPCSTR Passwd,DWORD *Key);
 DWORD CreateAcc           (LPCSTR AccName,LPCSTR Passwd);
 DWORD DeleteAcc           (LPCSTR AccName);
 DWORD SetAccPasswd        (DWORD ID,LPCSTR Passwd);
 DWORD GetAccPasswd        (DWORD ID,LPSTR Passwd,bool &New);
 DWORD GetAccIDByName      (LPCSTR AccName);
};

//---------------------------------------------------------------------------
#endif
