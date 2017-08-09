/*===========================================================================
    Заголовок: AccauntBase
    Автор: Kashey
    Информация: База данных юзеров

===========================================================================*/
//---------------------------------------------------------------------------

		

#include "ProjectSpace.h"	
#include "AccauntBase.h"
#include "..\Sys\HashLib.h"

#include <io.h>
#include <fcntl.h>
#include <SYS\STAT.H>


#define strcpy(a,b) strcpy_s(a,0xFF*20,b)


CAccauntBase::CAccauntBase()
{
 AddAccIfNotFound=1;
 AccDatabaseFile=0;
 DataBaseFileName="DataBases\\Accaunts\\AccBase.idx";
}			  

DWORD CAccauntBase::GetAccIDByName(LPCSTR AccName)
{
 DWORD ret=FindAccInCashe(AccName,NULL);
 if(ret) return ret;
 CPlayerFileStore PL;
 ret=FindAccInDataBase(AccName,&PL);
 if(!ret)
 {
  if(AddAccIfNotFound)
   ret=CreateAccInDataBase(AccName,&PL);
  else
   return 0; 
 }
 AddAccToCashe(PL);
 return ret;
}

DWORD CAccauntBase::GetAccPasswd(DWORD ID,LPSTR Passwd,bool &New)
{
 CPlayerFileStore PL;
 DWORD ret=FindAccInCasheByID(ID,&PL);
 if(!ret) return 0;
 else
 {
  strcpy(Passwd,PL.PlayerPass);
  if(Passwd[0]==0)
  {
   strcpy(Passwd+1,PL.PlayerPass+1);
   New=true;
  }else
   New=false;
  return 1; 
 }
}

DWORD CAccauntBase::SetAccPasswd(DWORD ID,LPCSTR Passwd)
{
 CPlayerFileStore PL;
 DWORD ret=FindAccInCasheByID(ID,&PL);
 if(!ret) return 0;
 else
 {
  strncpy_s(PL.PlayerPass,sizeof(PL.PlayerPass),Passwd,strlen(Passwd)+1);
  UpdatePlayerAccInBase(PL);
 }
 return 0;
}

DWORD CAccauntBase::UpdatePlayerAccInBase(CPlayerFileStore &ARef)
{
 if(!CheckFileOpen()) return 0;
 if(lseek(AccDatabaseFile,8,SEEK_SET)==-1) return 0;
 CPlayerFileStore PL;
 while(1)
 {
  int rret=read(AccDatabaseFile,&PL,sizeof(PL));
  if(rret<1) return 0;
  if(PL.PlayerID==ARef.PlayerID)
  {
   lseek(AccDatabaseFile,-(int)(sizeof(PL)),SEEK_CUR);
   write(AccDatabaseFile,&ARef,sizeof(PL));
  }
 }
}

DWORD CAccauntBase::FindAccInCashe       (LPCSTR AccName,CPlayerFileStore *ARef)
{
 for(int x=0;x<PlayerAccCashe.size();++x)
  if(Fstrcmp(PlayerAccCashe[x].PlayerName,AccName)==0)
  {
   if(ARef) *ARef=PlayerAccCashe[x];
   return x;
  }
 return 0;
}

DWORD CAccauntBase::FindAccInDataBase    (LPCSTR AccName,CPlayerFileStore *ARef)
{
 if(!CheckFileOpen()) return 0;
 if(lseek(AccDatabaseFile,8,SEEK_SET)==-1) return 0;
 CPlayerFileStore PL;
 while(1)
 {
  int rret=read(AccDatabaseFile,&PL,sizeof(PL));
  int rret2=sizeof(PL);
  if(rret<rret2) return 0;
  if(Fstrcmp(PL.PlayerName,AccName)==0)
  {
   if(ARef) *ARef=PL;
   return PL.PlayerID;
  }
 }
 return 0;
}

DWORD CAccauntBase::CreateAccInDataBase  (LPCSTR AccName,CPlayerFileStore *ARef)
{
 if(!CheckFileOpen()) return 0;
 CPlayerFileStore PL;
 ZeroMemory(&PL,sizeof(PL));
 strcpy(PL.PlayerName,AccName);
 PL.PlayerPass[0]=0;
 strcpy(PL.PlayerPass+1,"No Passwd");
 PL.PlayerID=GetNextAccID();
 PL.FirstConnection.SetNOW();
 if(lseek(AccDatabaseFile,0,SEEK_END)==-1) return 0;
 write(AccDatabaseFile,&PL,sizeof(PL));
 if(ARef) *ARef=PL;
 return PL.PlayerID;
}

DWORD CAccauntBase::FindAccInCasheByID   (DWORD  AID    ,CPlayerFileStore *ARef)
{
 for(int x=0;x<PlayerAccCashe.size();++x)
 if(PlayerAccCashe[x].PlayerID==AID)
 {
   if(ARef) *ARef=PlayerAccCashe[x];
   return x+1;
 }
 return 0;
}

DWORD CAccauntBase::AddAccToCashe        (CPlayerFileStore &ARef)
{
 PlayerAccCashe.push_back(ARef);
 return true;
}


DWORD CAccauntBase::FindAcc(LPCSTR AccName)
{
 return GetAccIDByName(AccName);
}

DWORD CAccauntBase::GetNextAccID()
{
 if(!CheckFileOpen()) return 0;
 if(lseek(AccDatabaseFile,0,SEEK_SET)==-1) return 0;
 DWORD ID;
 int rret=read(AccDatabaseFile,&ID,sizeof(ID));
 if(rret<sizeof(ID))
 {
  ID=0;
  lseek(AccDatabaseFile,0,SEEK_SET);
  write(AccDatabaseFile,&ID,sizeof(ID));
  write(AccDatabaseFile,&ID,sizeof(ID));
 }
 ++ID;
 lseek(AccDatabaseFile,0,SEEK_SET);
 write(AccDatabaseFile,&ID,sizeof(ID));
 return ID;
}

DWORD CAccauntBase::CheckPasswd(DWORD AID,LPCSTR Passwd,DWORD *Key)
{
 char PasswdIn [0xFF];// 32 byte
 char PasswdOut[0xFF];// 32 byte
 char PasswdIn2[0xFF];// 32 byte
 bool New;
 if(!GetAccPasswd(AID,PasswdIn,New)) return 0;
 if(New)
  return 3;
 else
 {
  __Hash1Prepare(PasswdIn,PasswdIn2,Key);
  __Hash1(PasswdIn2,PasswdOut);
  if(strcmp(PasswdOut,Passwd)!=0)return 2;
 }
 return 4;
}

bool   CAccauntBase::CheckFileOpen()
{
 if(AccDatabaseFile) return true;
 return OpenFile();
}

bool   CAccauntBase::OpenFile()
{
/* if(AccDatabaseFile) CloseFile();
 AccDatabaseFile=open_s(DataBaseFileName.c_str(),O_BINARY | O_RDWR,_S_IREAD | _S_IWRITE);
 if(AccDatabaseFile<1)
 {
  AccDatabaseFile=open(DataBaseFileName.c_str(),O_CREAT | O_BINARY | O_RDWR,_S_IREAD | _S_IWRITE);
  DWORD _ZR=0;
  write(AccDatabaseFile,&_ZR,sizeof(_ZR));
  write(AccDatabaseFile,&_ZR,sizeof(_ZR));
 }
 return AccDatabaseFile>0;
 */
	return 1;
}

bool   CAccauntBase::CloseFile()
{
 int ret=close(AccDatabaseFile);
 if(!ret)AccDatabaseFile=0;
 return !ret;
}



