/*===========================================================================
    Заголовок: BlopMessages.cpp
    Автор: Kashey
    Информация: Basic message sending and transmiting

===========================================================================*/
//---------------------------------------------------------------------------


#include "ProjectSpace.h"
#include "BlopMessages.h"


WORD  BlopMessage::size()const { return 1+Size;}
void  BlopMessage::Put(BYTE cmd,const void *_Data,WORD DataSize)
{
  Cmd=cmd;
  if(Data){Size=DataSize;memcpy(Data,_Data,DataSize);}
  else Size=0;
}
void  BlopMessage::Assign(const BlopMessage &m) { Put(m.Cmd,m.Data,m.Size);}
DWORD BlopMessage::AsDword()const { return *((DWORD*)Data);}
 //WORD        Data_as_DWORD();
//---------------------------------------------------------------------------
BlopMessage* CreateBlopMessage(BYTE cmd,const void *Data,WORD DataSize)
{
 BlopMessage* Msg=(BlopMessage*)malloc(4+DataSize);
 Msg->Put(cmd,Data,DataSize);
 return Msg;
}
void DeleteBlopMessage(BlopMessage* Message)
{
 free(Message);
}

BlopMessage* CreateBlopMessage(BYTE cmd,DWORD  Data)
{ return CreateBlopMessage(cmd,&Data,sizeof(Data));}
BlopMessage* CreateBlopMessage(BYTE cmd,LPCSTR Data)
{ return CreateBlopMessage(cmd,(void*)Data,(WORD)(strlen(Data)+1));}



void BlopMessageStock::Add(const BlopMessage* Message)
{
 if(!Message) return;
 WORD msize=Message->size();
 CheckAlloc(msize);
 memcpy(CurrentMessage,&Message->Cmd,msize);
 Offsets[NumMessages]=msize;
 Next();
}

LPCBYTE BlopMessageStock::ToBegin()
{
 return (LPCBYTE)(CurrentMessage=MessageBegin);
}

LPCBYTE BlopMessageStock::Next()
{
 if(!Offsets[NumMessages]) return NULL;
 CurrentMessage+=Offsets[NumMessages++];
 return (LPCBYTE)CurrentMessage;
}

void BlopMessageStock::Clear()
{
 ZeroMemory(MessageBegin,Allocated);
 ToBegin();
 NumMessages=0;
}

void BlopMessageStock::CheckAlloc(DWORD Size)
{
 if(!Size || !Allocated || !CurrentMessage || (Allocated-(CurrentMessage-MessageBegin)<Size))
 {
  MessageBegin=(char*)realloc(MessageBegin,Allocated-(CurrentMessage-MessageBegin)+Size);
  if(!CurrentMessage) Clear();
  Allocated=Allocated-(CurrentMessage-MessageBegin)+Size*2;
 }
}

BlopMessageStock::BlopMessageStock()
{
  MessageBegin=CurrentMessage=NULL;
  Allocated=NumMessages=0;
}

BlopMessageStock::~BlopMessageStock()
{
 free(MessageBegin);
}


