/*===========================================================================
    Заголовок: blopBaseObject.cpp
    Автор: Kashey
    Информация: Base object

===========================================================================*/
//---------------------------------------------------------------------------


#include "ProjectSpace.h"
#include "ProjectDividors.h"
#include "blopbaseobject.h"
//---------------------------------------------------------------------------
DWORD ObjectIDCounter=1;
DWORD NumBBObjects=0;


class CBaseClassManagerBun
{
	DWORD SelfID;
	DWORD NumObjects;
	DWORD LastDeletedObject;
	typedef SmartVector<PBlopBaseObject> CBunVector;
    CBunVector ObjVector;
public:
	CBaseClassManagerBun(DWORD ID);
	void KILL(DWORD FromRef);
	bool CanHoldOne()const;
	DWORD AllocateOne(DWORD ID,PBlopBaseObject Obj);
	bool  RemoveOne  (DWORD ID,PBlopBaseObject Obj);
	PBlopBaseObject GetObject(DWORD ID);
};

class CBaseClassManager
{
    CBaseClassManagerBun *Buns[0xF];
	DWORD ThisRef;
public:
	 CBaseClassManager(DWORD REF);
	~CBaseClassManager();
	void KILL();
	bool CanHoldOne(BYTE BUN);
	CObjectID AllocateOne(BYTE Bun,DWORD ID,PBlopBaseObject Obj);
	bool 	  RemoveOne  (BYTE Bun,DWORD ID,PBlopBaseObject Obj);
    PBlopBaseObject GetObject(BYTE Bun,DWORD ID);
};


#define DEF_POSTMESSAGE_LEN 0xFF*4

//typedef SmartVector<BlopBaseObject*>    _ObjPack;
typedef CBSPDividorLookUpArray<char,DWORD> _BASENAMEBSP;

//_ObjPack  EngineObjectes;
_BASENAMEBSP EngineObjectsLookUpTable;


CBaseClassManager *REF_CLASSES[0xF];

bool CheckBun(DWORD ClassBun)
{
	static bool BUN_ACTIVE=0;
	if(!BUN_ACTIVE)
	{
		for(int x=0;x<0xF;++x)
			REF_CLASSES[x]=new CBaseClassManager(x);
		BUN_ACTIVE=1;
	}
	return REF_CLASSES[ClassBun>>4]->CanHoldOne(ClassBun&0x0F);
}

CObjectID Add2ClassBun(DWORD ClassBun,DWORD ID,BlopBaseObject *Obj)
{
  return REF_CLASSES[ClassBun>>4]->AllocateOne(ClassBun&0x0F,ID,Obj);
}

void RemoveFromClassBun(CObjectID &ID,BlopBaseObject *Obj)
{

   REF_CLASSES[ID.GetObjectClass()]->RemoveOne(ID.GetObjectBun(),ID.GetObjectID(),Obj);
}

BlopBaseObject* CORE::REF_FindObjectByID(const CObjectID &ID)
{
   if(ID!=REF_NULL)
		if(CheckBun(ID.GetObjectClassBun()))
           return REF_CLASSES[ID.GetObjectClass()]->GetObject(ID.GetObjectBun(),ID.GetObjectID());
   return NULL;
}

CObjectID CORE::REF_FindObjectByName(LPCSTR Name)
{
	return EngineObjectsLookUpTable.GetIDFor(Name,strlen(Name));
}

BlopBaseObject* CORE::REF_GetObjectByName(LPCSTR Name)
{
    return REF_FindObjectByID(REF_FindObjectByName(Name));
}


CObjectID CORE::REF_CreateID(DWORD ClassBun,DWORD ID,BlopBaseObject *Obj)
{
	ClassBun>>=24;
	if(CheckBun(ClassBun))
		return Add2ClassBun(ClassBun,ID,Obj);
	else return REF_NULL;
}
void CORE::REF_RemoveID(CObjectID &ID,BlopBaseObject *Obj)
{
	if(ID!=REF_NULL)
		if(CheckBun(ID.GetObjectClassBun()))
			RemoveFromClassBun(ID,Obj);
	ID=REF_NULL;
}

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------

void  BaseObject::SetName(LPCSTR Name,...)
{
	FORMAT(thisName,Name);
}

LPCSTR BlopBaseObject::GetName()const
{
	return "BASE";
}

LPCSTR BlopBaseObject::GetFormatName()const
{
	return GetName();
}

const CObjectID    &BlopBaseObject::GetID()const{return thisID;}
BlopBaseObject::operator    CObjectID()const{return GetID();}

BlopBaseObject::BlopBaseObject(DWORD Class,DWORD Bun,DWORD ID) :CTicableObject(false)               
{ 
    thisID=REF_CreateID(Class|(Bun<<24),ID,this);
	++NumBBObjects;
//	AddObjectToPack(this);
}

BlopBaseObject::~BlopBaseObject() 
{
	REF_RemoveID(thisID,this);
	--NumBBObjects;
}

BaseObject::BaseObject(DWORD Class,DWORD Bun,LPCSTR Name,DWORD ID) :BlopBaseObject(Class,Bun,ID)               
{ 
	SetName(Name);
	EngineObjectsLookUpTable.PushAs(GetID().GetIDDWORD(),thisName.v_str(),thisName.length());
}

BaseObject::~BaseObject()
{
}

LPCSTR BaseObject::GetName()const
{
	return thisName;
}

LPCSTR BaseObject::GetFormatName()const
{
	return GetName();
}

void  BlopBaseObject::SendMessage(const CObjectID &Dest,DWORD Msg,WORD Len,const LPVOID Data,bool KeepData)
{
	SendUpSignal(CObjectIDTwin(Dest,GetID()),Msg,Len,Data,KeepData);
}

bool  BlopBaseObject::IsStarted(){return StartReference.Started;}
bool   BlopBaseObject::StartObject(){if(IsStarted())return true;StartReference.Started=Start();return IsStarted();}

//const CObjectID &BlopBaseObject::GetID()const {return thisID;}

bool  BlopBaseObject::Start(){return true;};
void  BlopBaseObject::Stop (){};
DWORD BlopBaseObject::OnMessage(const CBlopMessage &Message){return 0;}
bool  BlopBaseObject::Prepare(){return true;}



SectionLocker MessagePostLocker;

CBlopMessage PostCycle [DEF_POSTMESSAGE_LEN];
CBlopMessage OutPostBuf[DEF_POSTMESSAGE_LEN];
WORD         Post_CurrentPosition=0;
WORD         Pos_Len             =0;

inline WORD LimitHole(int Value,WORD Limit)  // _!_ place this to math
{
	if(Value>Limit) {Value-=Limit;return LimitHole(Value,Limit);}
	else             return Value;
}

void DoPost(CBlopMessage &Msg)
{
  PostCycle[Pos_Len++]=Msg;
}

void CORE::SendUpSignal(CObjectIDTwin &Address,DWORD CMD,WORD Len,LPVOID InPoint,bool KeepData)
{
	DynamicLocker Locker(MessagePostLocker);
    CBlopMessage Msg;
	Msg.SetAddress(Address);
	Msg.SetMsg    (CMD);
	Msg.SetLen    (Len);
	if(InPoint)
    {
	 if(KeepData)Msg.SetData(InPoint);
	 else
	 {
      BYTE*  MsgData=new BYTE[Len];  // _!_ BAAD thing!
	  memcpy(MsgData,InPoint,Len);
      Msg.SetData(MsgData);
	 }
	}
	else
	Msg.SetData(NULL);
    DoPost(Msg);
}

CBlopMessage* CORE::GetUpSingnal(WORD &Len)
{
	DynamicLocker Locker(MessagePostLocker);
	Len=Pos_Len;
	if(!Pos_Len) return NULL;
	for(WORD x=0;x<Pos_Len;++x)
		OutPostBuf[x]=PostCycle[x];
    Pos_Len=0;
	return OutPostBuf;
}

CTicableObject::CTicableObject(bool AddToTicableBun)
{
}

CTicableObject::~CTicableObject()
{
}

DWORD CTicableObject::DoTick(float Time)
{
	return 0;
}


CBaseClassManagerBun::CBaseClassManagerBun(DWORD ID)
{
	SelfID=ID<<24;
	NumObjects=0;
	LastDeletedObject=0;
}

bool CBaseClassManagerBun::CanHoldOne()const{return NumObjects<0xFFFFFF;}

DWORD CBaseClassManagerBun::AllocateOne(DWORD ID,BlopBaseObject *Obj)
{
	CBunVector::iterator it=ObjVector.begin()+LastDeletedObject;
	for(;it<ObjVector.end();++it)
	{
		if((DWORD)(*it)==NULL)
		{
			*it=Obj;
            ++LastDeletedObject;
			return SelfID|((DWORD)(it-ObjVector.begin())+1);
		}
	}
	++LastDeletedObject;
	ObjVector.push_back(Obj);
	return SelfID|ObjVector.size();
}

bool 	  CBaseClassManagerBun::RemoveOne  (DWORD ID,BlopBaseObject *Obj)
{
	if(ObjVector[ID-1]!=Obj)
	{
		ASSERT(FALSE && "OBJECT NOT IN POSTION");
		return false;
	}
	ObjVector[ID-1]=0;
	return true;
}

PBlopBaseObject CBaseClassManagerBun::GetObject(DWORD ID)
{
	if(ID<ObjVector.size()) return ObjVector[ID-1];
	else return NULL;
}

void CBaseClassManagerBun::KILL(DWORD FromRef)
{
	for(CBunVector::iterator it=ObjVector.begin();it<ObjVector.end();++it)
		if(*it)
		{
			SendUpSignal(CObjectIDTwin((*it)->GetID(),FromRef|SelfID),BMSG::CMD_TERMINATE);
		}
}


CBaseClassManager::CBaseClassManager(DWORD R)
{
	ThisRef=R<<28;
	for(int x=0;x<0xF;++x)Buns[x]=NULL;
}

CBaseClassManager::~CBaseClassManager()
{
    for(int x=0;x<0xF;++x)
		if(Buns[x])delete Buns[x];
}

void CBaseClassManager::KILL()
{
    for(int x=0;x<0xF;++x)
		if(Buns[x])Buns[x]->KILL(ThisRef);
	SendUpSignal(CObjectIDTwin(ThisRef,ThisRef),BMSG::CMD_TERMINATE);
}

bool CBaseClassManager::CanHoldOne(BYTE BUN)
{
	if(!Buns[BUN])Buns[BUN]=new CBaseClassManagerBun(BUN);
	return Buns[BUN]->CanHoldOne();
}

CObjectID CBaseClassManager::AllocateOne(BYTE Bun,DWORD ID,BlopBaseObject *Obj)
{
	return CObjectID(ThisRef | Buns[Bun]->AllocateOne(ID,Obj));
}

bool 	  CBaseClassManager::RemoveOne  (BYTE Bun,DWORD ID,BlopBaseObject *Obj)
{
	return Buns[Bun]->RemoveOne(ID,Obj);
}

PBlopBaseObject CBaseClassManager::GetObject(BYTE Bun,DWORD ID)
{
	return Buns[Bun]->GetObject(ID);
}
