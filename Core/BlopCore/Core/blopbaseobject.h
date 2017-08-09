//---------------------------------------------------------------------------

#ifndef blopBaseObjectH
#define blopBaseObjectH

#include "Sys/Memory/genericmemoryobject.h"
#include "Sys/REF/refID.h"
#include "Sys/REF/refCMD.h"

namespace CORE
{

struct BlopUtilsPack
{
 ConsoleUtil        *ConsoleOut;
};

struct CObjectID//: mS
{
	DWORD ID;
	CObjectID(DWORD _ID)           { ID=_ID;}
	CObjectID()                    { ID=REF_NULL;}
	BYTE  GetObjectClass()const{return (ID&0xF0000000)>>28;}
	WORD  GetObjectClassBun()const{return (ID&0xFF000000)>>24;}
	BYTE  GetObjectBun  ()const{return (ID&0x0F000000)>>24;}
	WORD  GetObjectID   ()const{return ID&0x00FFFFFF;}
	DWORD GetID32       ()const{return ID&0x0FFFFFFF;}
	DWORD GetIDDWORD    ()const{return ID;}
	bool Equal(const CObjectID &Sourse)const { return ID==Sourse.ID;}
	bool operator == (const CObjectID &Sourse)const {return Equal(Sourse);}
	bool operator == (const DWORD &Sourse)const {return ID==Sourse;}
	bool operator != (const DWORD &Sourse)const {return ID!=Sourse;}

};

struct  CObjectIDTwin//: mS
{
	CObjectID To;
	CObjectID From;
	CObjectIDTwin(){};
	CObjectIDTwin(const CObjectID& _To,const CObjectID& _From)
	{To  =_To;From=_From;}
};
typedef const CObjectID CCObjectID;
typedef CCObjectID& CCObjectIDRef;


CCObjectIDRef DLLTYPE GetRootID  ();
CCObjectIDRef DLLTYPE GetCameraID();
CObjectID     DLLTYPE AllocateID (WORD Class,WORD Bun);

struct CBlopMessage//: mS
{
private:
	CObjectIDTwin Address;
	DWORD         Msg;
    WORD          Len;
	BYTE         *Data;
public:
	CBlopMessage(){Len   =0;Msg   =0;Data  =NULL;}
    void Free()   {if(Data){delete []Data; Data=NULL;}}

	DWORD GetMsg()const{return Msg;}
	DWORD GetLen()const{return Len;}
	const BYTE* GetData()const{return Data;}
	const CObjectIDTwin &GetAddress()const{return Address;}

	void SetMsg(const DWORD _Msg){Msg=_Msg;}
	void SetLen(const DWORD _Len){Len=_Len;}
	void SetAddress(const CObjectIDTwin &TW){Address=TW;}
	void SetData(LPVOID D){Data=(BYTE*)D;}
	void DropData(){SetData(NULL);}
};
typedef CBlopMessage *PBlopMessage;

struct CStartReference{bool Started;CStartReference(){ Started=false;}};

class DLLTYPE CTicableObject
{
public:
	CTicableObject(bool bAddToTicableBun=true);
	~CTicableObject();
	void AddToTicableBun();
	virtual DWORD Tick(float Time){return 0;}
	static DWORD DoTick(float Time);
};

class DLLTYPE BlopBaseObject: public CTicableObject
{

  CObjectID           thisID;
  CStartReference     StartReference;
  //virtual void                SetName(LPCSTR Name,...){};
 protected:
 public:
  virtual LPCSTR      GetName()const;
  virtual LPCSTR      GetFormatName()const;
  bool                IsStarted();

  const CObjectID    &GetID()const;
  operator    CObjectID()const;

  BlopBaseObject(DWORD Class,DWORD Bun,DWORD ID=0) ;
  virtual ~BlopBaseObject();

  bool    StartObject();
  virtual bool Start();
  virtual void Stop ();
  virtual bool Prepare();

//protected:

  virtual DWORD OnMessage(const CBlopMessage &Message);
  void    SendMessage(const CObjectID &Dest,DWORD Msg,WORD Len,const LPVOID Data=0,bool KeepData=false);
};

class DLLTYPE BaseObject: public BlopBaseObject
{
  GString             thisName;
  void                SetName(LPCSTR Name,...);
 public:
  LPCSTR              GetName()const;
  LPCSTR              GetFormatName()const;

 // const CObjectID    &GetID()const;
 // operator    CObjectID()const{return GetID();}

  BaseObject(DWORD Class,DWORD Bun,LPCSTR Name=NULL,DWORD ID=0);
  virtual ~BaseObject();
};

typedef BlopBaseObject* PBlopBaseObject;
typedef BaseObject*     PBaseObject;





void         DLLTYPE SendUpSignal(CObjectIDTwin &Address,DWORD CMD,WORD Len=0,LPVOID InPoint=NULL,bool KeepData=false);
PBlopMessage DLLTYPE GetUpSingnal(WORD &Len);
PBlopMessage DLLTYPE ResvUpSingnal(const CObjectID &For,WORD &Len);

PBlopBaseObject DLLTYPE REF_FindObjectByID(const CObjectID &ID);
CObjectID       DLLTYPE REF_FindObjectByName(LPCSTR Name);
PBlopBaseObject DLLTYPE REF_GetObjectByName(LPCSTR Name);

CObjectID DLLTYPE REF_CreateID(DWORD ClassBun,DWORD ID,BlopBaseObject *Obj);
void DLLTYPE REF_RemoveID(CObjectID &ID,BlopBaseObject *Obj);

};
using namespace CORE;
//---------------------------------------------------------------------------
#endif
