//---------------------------------------------------------------------------

#ifndef BlopMessagesH
#define BlopMessagesH
//---------------------------------------------------------------------------

#define MAX_BMSG_SIZE 1024



struct BlopMessage
{
 WORD Size;
 BYTE Cmd;
 char Data[1];
 inline WORD  size()const;
 void         Put(BYTE cmd,const void *_Data,WORD DataSize);
 void         Assign(const BlopMessage &m);
 DWORD        AsDword()const;
 //DWORD        Data_as_DWORD();
};

typedef const BlopMessage* LPCBlopMessage;

 BlopMessage* CreateBlopMessage(BYTE cmd,const void *Data,WORD DataSize);
 BlopMessage* CreateBlopMessage(BYTE cmd,DWORD  Data);
 BlopMessage* CreateBlopMessage(BYTE cmd,LPCSTR Data);

 void DeleteBlopMessage(BlopMessage* Message);

class BlopMessageStock
{
 char *MessageBegin;
 char *CurrentMessage;
 WORD Offsets[0xFF];
public:
 DWORD NumMessages;
 DWORD Allocated;
 void Add(const BlopMessage* Message);
 void Clear();
 LPCBYTE ToBegin();
 LPCBYTE Next();
 void CheckAlloc(DWORD Size);
 BlopMessageStock();
~BlopMessageStock();
};



//**********************************************
//************** SYSTEM ************************
#define bCMD_IDLE      0
#define bCMD_START     1
#define bCMD_STOP      2
#define bCMD_RESTART   3
#define bCMD_UPDATE    4
#define bCMD_SYNC      5

//**********************************************
//************** MOVENT ************************
#define bCMD_MOVE      100
#define bCMD_ROTATE    101

#define bCMD_SYNC_POS  201

#endif
