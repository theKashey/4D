/*===========================================================================
    Заголовок: MemoryPoop.cpp
    Автор: Kashey
    Информация:Memory manager

===========================================================================*/

// MemoryPool.cpp: implementation of the CTemporalyPointer class.
//
//////////////////////////////////////////////////////////////////////
#undef _DEBUG
#define NDEBUG
#include "ProjectSpace.h"
#include "MemoryPool.h"

//MemoryPool ErrorPool(100,16);

PMemoryPool GlobalDataPool   =NULL;//&ErrorPool;
PMemoryPool DataTemporalyPool=NULL;//&ErrorPool;

PMemoryPool GlobalStringPool   =NULL;//&ErrorPool;
PMemoryPool StringTemporalyPool=NULL;//&ErrorPool;

PMemoryPool LittleGlobalPool   =NULL;//&ErrorPool;
PMemoryPool LittleTemporalyPool=NULL;//&ErrorPool;

//********************************************************************
WORD __MemDataSlotSize      =512;
WORD __MemStringSlotSize    =256;
WORD __MemLittleSlotSize    =128;

DWORD __MemGlobalDataAlloc   =6*_MB;
DWORD __MemTempDataAlloc     =4*_MB;
DWORD __MemGlobalStringAlloc =2*_MB;
DWORD __MemTempStringAlloc   =1*_MB;
DWORD __MemGlobalLittleAlloc =2*_MB-64;
DWORD __MemTempLittleAlloc   =1*_MB;

bool gMEM2TCollapsing;
extern void StringsReady();

void __InitMem()
{
 gMEM2TCollapsing=false;
 GlobalDataPool  =new MemoryPool(__MemGlobalDataAlloc  ,__MemDataSlotSize);
 LittleGlobalPool=new MemoryPool(__MemGlobalLittleAlloc,__MemLittleSlotSize);
 StringsReady();
 CBaseSmartVector::_SmartReadyStart=0xABDF0;
}



void __KillMem()
{
    gMEM2TCollapsing=true;
	delete GlobalDataPool;
}

MemoryPool* GetGlobalDataPool()
{
	return GlobalDataPool;
}
//MemoryPool* MemoryPool::GetGlobalDataPool(){ return ::GetGlobalDataPool();}

SectionHardLocker MemoryLocker;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
void operator = (void* &Dest,SlotPoint &Sourse)
{
    if(MasterSlot)
      Dest=MasterSlot->GetAt(Sourse.ID);
    else Dest=NULL;
}
void operator = (void* &Dest,SlotPoint *Sourse)
{
    if(MasterSlot)
      Dest=MasterSlot->GetAt(Sourse->ID);
    else Dest=NULL;
}

void operator = (SlotPoint &Dest,void &Sourse)
{

}

void operator = (SlotPoint *Dest,void *Sourse)
{
}
*/

SlotPoint::SlotPoint(){MasterSlot=NULL;}

// void       SlotPoint::SetAccocPtr(void *_Ptr){AccocPtr=(void**)_Ptr;CheckPlace();}
// void       SlotPoint::CheckPlace()           {if(AccocPtr)*AccocPtr=GetAddr();};
PMemoryPool SlotPoint::GetMasterSlot(){return MasterSlot;}

WORD  SlotPoint::GetSlot(){return MySlot;}
WORD  SlotPoint::GetSlotNum(){return Len;}

SlotPoint::SlotPoint(WORD MID,PMemoryPool  _Master,WORD _MySlot,WORD _Len)
{
   MasterSlot=_Master;MySlot=_MySlot;Len=_Len;
   MyID=MID;
//   AccocPtr  = NULL;
}
SlotPoint::~SlotPoint()
{
   if(MasterSlot)
    MasterSlot->FreeSlot(this,3);
   Unvalid();    
}

DWORD SlotPoint::Size()
{
 if(this && MasterSlot)
 return Len*MasterSlot->SlotSize;
 else
 return 0;
}


bool SlotPoint::Realloc(DWORD NewLen)
{
  if(MasterSlot)
    return MasterSlot->ReallocSlot(this,NewLen);
  return false;
}

/*void SlotPoint::Merge(SlotPoint *Merging)
{
  MemoryPool *_MasterSlot=MasterSlot;
  WORD       _MySlot     =MySlot;
  WORD       _Len        =Len;

  MasterSlot=Merging->MasterSlot;
  MySlot    =Merging->MySlot    ;
  Len       =Merging->Len       ;

  Merging->MasterSlot=_MasterSlot;
  Merging->MySlot    =_MySlot    ;
  Merging->Len       =_Len       ;

  CheckPlace();
  Merging->CheckPlace();
}    */

bool SlotPoint::Defragment()
{
 if(MasterSlot)
  return MasterSlot->Defragment();
 return false;
}

void SlotPoint::Unvalid()
{
   MySlot=0;
   Len=0;
//   if(AccocPtr)
//    *AccocPtr=NULL;
//   MasterSlot=NULL;
}

void *SlotPoint::GetAddr()
{
 return MasterSlot->GetAt(this);
}

SlotPoint::operator LPVOID()
{
 return GetAddr();
}

DWORD MemoryPool::_MAP(DWORD SlotID)
{
 return SlotID*DSlot;
}


MemoryPool::MemoryPool(DWORD ByteCount,WORD _SlotSize,bool _ChildBlock)
{
	ChildBlock=_ChildBlock;
    if(!ChildBlock)
    {
     MemList::Add(this);
    }
    {
     dSlotSize=0;
     int j=1;
     for(dSlotSize=0;dSlotSize<32;++dSlotSize)
         {
          if(_SlotSize<=j)
              {
                _SlotSize=j;
                break;
              }
          j=j<<1;
         }
    }

    //calc number of slots
    if(_SlotSize>sizeof(SingleSlot)) SlotSize=_SlotSize;
    else                             SlotSize=sizeof(SingleSlot);
    SlotsNum=ByteCount / SlotSize;
    DSlot=SlotSize/sizeof(SingleSlot);
    if( SlotsNum*SlotSize < ByteCount ) ++SlotsNum;

    ASSERT((" Memory pool report - to many chunks" || 1) && (SlotsNum<=0xFFFF));

    Slots   =(SingleSlot*)malloc(_MAP(SlotsNum)*4);//new SingleSlot[_MAP(SlotsNum)];
    SlotUsed=(byte*      )malloc(SlotsNum);//new       byte[SlotsNum];
    Points  =(PSlotPoint*)malloc(SlotsNum*sizeof(PSlotPoint));//new PSlotPoint[SlotsNum];
    DWORD p=_MAP(SlotsNum)*sizeof(SingleSlot);
    bzero(Slots   ,p);//_MAP(SlotsNum)*sizeof(SingleSlot));
    bzero(SlotUsed,SlotsNum*sizeof(byte      ));
    bzero(Points  ,SlotsNum*sizeof(PSlotPoint));

    NumSlotsFree   =SlotsNum;
    MaximumSlotFree=SlotsNum;
    MaximumSlotPos =0;
	FirstFreeBlock =0;
	LastBlockFree  =SlotsNum;

	LinkedPool=NULL;

    NumOperations=0;
	if(!ChildBlock)
	Print2Console(SYS_ENGINE,"+%sMEM %d bytes in %d slot size(%d slots)",ChildBlock?"c":"r",SlotsNum*SlotSize,SlotSize,SlotsNum);
}

MemoryPool::~MemoryPool()
{
	if(LinkedPool)delete LinkedPool;
	LinkedPool=NULL;
    for(DWORD x=0;x<SlotsNum;++x)
        if(Points[x]){ Points[x]->Unvalid(); delete Points[x];Points[x]=NULL;}
    free(Slots);
    free(SlotUsed);
    free(Points);
	if(!gMEM2TCollapsing)
	Print2Console(SYS_ENGINE,"-%sMEM %d bytes in %d slot size(%d slots)",SlotsNum*SlotSize,SlotSize,SlotsNum);
}

bool MemoryPool::IsFree()const{return SlotsNum==NumSlotsFree;}

SlotPoint *MemoryPool::Alloc(DWORD NumBytes)
{
    static _Profilers::CSectionProfile defAllocfuncPro(NULL);
    static _Profilers::CSectionProfile *AllocfuncPro=&defAllocfuncPro;
    if(NumBytes==(DWORD)-2) 
    {   //delete defAllocfuncPro;
        defAllocfuncPro.~CSectionProfile();
        AllocfuncPro=new _Profilers::CSectionProfile("MemoryPool::Alloc");
        return NULL;
    }
    {
    Profile(*AllocfuncPro);
   DWORD Ssize=(DWORD)(NumBytes >> dSlotSize);
   if(Ssize*SlotSize < NumBytes ) ++Ssize;
   if(Ssize<1) return NULL;
   --Ssize;
  //find slot
   int Indx;
   DWORD y;
   if(Ssize<SlotsNum)
   if((int)Ssize<NumSlotsFree)
   for(DWORD x=FirstFreeBlock;x<LastBlockFree;)
   if(!SlotUsed[x])
   {
    Indx=Ssize;
    BYTE *SU=&SlotUsed[x+1];
    for(y=x+1;y<SlotsNum;++y,--Indx,++SU)if(*SU || !Indx) break;

    if(Indx>0) x=y+1;
     else
    //we found place//mark slots
     return AddSlot(x,Ssize+1,NumBytes);
   }else x+=SlotUsed[x];
	}
   //so we out of mem
   if(!LinkedPool)
   {
	   DWORD m=MAX(NumBytes+SlotSize,SlotsNum*SlotSize);
	   DWORD SL=SlotSize;
	   while((m / SL)>0xFFFF) SL=SL<<1;
	   LinkedPool=new MemoryPool(m,SL,true);
	   Print2Console(SYS_ENGINE,"+%sMEM %d bytes in %d slot size(%d slots)",true?"c":"r",m,SL,m/SL);
   }
   if(LinkedPool)
	   return LinkedPool->Alloc(NumBytes);
   return NULL;
}

void MemoryPool::FreeSlot(SlotPoint *Slot,BYTE Delete)
{
 FreeSlot(Slot->MyID,Delete);
 /*SlotPoint **S=Points;
 for(WORD i=0;i<SlotsNum;++i,++S)
     if(*S==Slot)
         {
         ASSERT(Points[i]==Slot);
         FreeSlot(i,Delete);
         return;
         }

 S=Points;
 for(WORD i=0;i<SlotsNum;++i,++S)
 {
      if(!(*S))
          {
            *S=Slot;
            FreeSlot(i,Delete);return;
          }
 }
 ASSERT(FALSE);
 */
}

void MemoryPool::FreeSlot(DWORD SlotID,BYTE Delete)
{
//    DynamicLocker DL(MemoryLocker);
    if(SlotID>=0 && SlotID<SlotsNum)
    {
      PSlotPoint Slot=Points[SlotID];
      const int FreeLen=Slot->MySlot+Slot->Len;
      NumSlotsFree+=Slot->Len;
      MemList::RegDea(Slot->Size());
      for(int x=Slot->MySlot;x<FreeLen;++x)
          SlotUsed[x]=0;
      if(Delete==1) delete Points[SlotID];
      if(Delete!=0) Points[SlotID]=NULL;
      ++NumOperations;
    }
}

#ifdef _DEBUG
void MemoryPool::VALIDATEOVERRUN(DWORD FOR)
{
  if(!Points[FOR])return;
  DWORD MEMFROM=(DWORD)Points[FOR]->GetAddr();
  DWORD MEMTO  =(DWORD)Points[FOR]->Size()+MEMFROM;
    for(DWORD i=0;i<SlotsNum;++i)
    if(i!=FOR)
     if(Points[i])
     {
      if((DWORD)Points[i]->GetAddr()<MEMFROM)
          if((DWORD)Points[i]->GetAddr()+(DWORD)Points[i]->Size()>MEMFROM)
              {
              ASSERT("MEMORY BLOCK LOWER OVERRUN");
              return;
              }
      if((DWORD)Points[i]->GetAddr()>MEMTO)
          if((DWORD)Points[i]->GetAddr()<MEMTO)
              {
              ASSERT("MEMORY BLOCK NEXT OVERRUN");
              return;
              }
     }
}
#else
void MemoryPool::VALIDATEOVERRUN(DWORD FOR)
{
}
#endif


SlotPoint *MemoryPool::AddSlot(const WORD x,const WORD len,DWORD NumBytes)
{
//    DynamicLocker DL(MemoryLocker);
    MemList::RegAlloc(NumBytes);
    for(DWORD i=0;i<SlotsNum;++i)
     if(!Points[i])
     {
         Points[i]=new SlotPoint(i,this,x,len);
//         Points[i]->ID=i;
//         Points[i]->UsedLen=NumBytes;
         int l=len;
         NumSlotsFree-=l;
         int z=x;
         int sz=0;
         while(l>=0xFF)
         {
              const int il=z+0xFF;
              BYTE *SU=&SlotUsed[z];
              for(;z<il;++z,++SU)*SU=0xFF;
              sz+=0xFF;
              l-=0xFF;
         }
         const int il=x+l+sz;
         //++l;
         BYTE *SU=&SlotUsed[z];
         for(;z<il;++z,--l,++SU)*SU=l;
         //  VALIDATEOVERRUN(i);

         ++NumOperations;
         return Points[i];
     }
    return NULL;
}

bool MemoryPool::Optimize()
{
   bool ret=false;
   if(LinkedPool)ret|=LinkedPool->Optimize();
   if(NumOperations<(SlotsNum>>3)+1){}
   {
   NumOperations=0;
   FirstFreeBlock =0;
   LastBlockFree  =SlotsNum;
   int i=SlotsNum;
   DWORD OFF=0;
   for(;i>=0;--i)
   {
    if(!SlotUsed[i])
    {
      LastBlockFree=i;
      break;
    }   
   }
   for(;i>=0;--i)
   {
    if(!SlotUsed[i]){ FirstFreeBlock=i;OFF=0;}
    else
    {
      ++OFF;
      SlotUsed[i]=MIN((DWORD)0xFF,OFF);
    }
   }

   SlotPoint **S=Points;
   for(WORD i=0;i<SlotsNum;++i,++S)
   {
     if(*S)(*S)->MasterSlot=this;
   }
   ret=true;
   }
   if(LinkedPool)
   {
	   if(LinkedPool->IsFree())
		   if(((float)SlotsNum/(float)NumSlotsFree)<0.7)
		   {
			   delete LinkedPool;
	           LinkedPool=NULL;
		   }
   }
   return ret;
};

void *MemoryPool::GetAt(SlotPoint* Slot)
{
 return GetAt(Slot->MyID);          
/* SlotPoint **S=Points;
 for(WORD i=0;i<SlotsNum;++i,++S)
     if(*S==Slot)
         {
          ASSERT(Points[i]==Slot);
          return GetAt(i);
         }
 S=Points;
 for(WORD i=0;i<SlotsNum;++i,++S)
     {
      if(!(*S))
          {
            *S=Slot;
            return GetAt(i);
          }
     }
 ASSERT(FALSE);
 return NULL;
*/
}

void *MemoryPool::GetAt(WORD SlotN)
{
//  DynamicLocker DL(MemoryLocker);
  if(SlotN<SlotsNum)
      if(Points[SlotN])
       return &Slots[_MAP(Points[SlotN]->MySlot)];
  return NULL;
}

DWORD MemoryPool::BytesFree()
{
//    DynamicLocker DL(MemoryLocker);
    DWORD res=0;
    for(DWORD x=0;x<SlotsNum;++x)
        if(!SlotUsed[x]) ++res; else x+=SlotUsed[x];
    return res*sizeof(SingleSlot);
}

DWORD MemoryPool::BytesAlloc()
{
    return  SlotsNum*SlotSize-BytesFree();
}

DWORD MemoryPool::GetMinimumSize()
{
	if(!this)
		return 0;
	return SlotSize;
}

bool MemoryPool::Defragment()
{
  return true;
}

bool MemoryPool::ReallocSlot(SlotPoint* OldSlot,DWORD NewSize)
{
// DynamicLocker DL(MemoryLocker);
 if((int)NewSize<=SlotSize*OldSlot->Len){ /*OldSlot->UsedLen=NewSize;*/ return true;}
 else
 {
  SlotPoint* NewSlot= Alloc(NewSize);
  memcpy(NewSlot->GetAddr(),OldSlot->GetAddr(),SlotSize*OldSlot->Len);
  FreeSlot(OldSlot,0);
  *OldSlot=*NewSlot;
  NewSlot->Unvalid();
  delete NewSlot;
  //FreeSlot(NewSlot,1);
//  OldSlot->Merge(NewSlot);
  return true;
 }
}

SlotPoint *MemoryPool::FindSlotByAddr(LPCVOID Addr)
{
	DWORD Start=(DWORD)Slots;
	DWORD End=((DWORD)Slots)+SlotsNum*SlotSize;
	DWORD Me=(DWORD)Addr;
	if(Me>=Start && Me<=End)
	{
	  //DWORD Off=(DWORD)((SingleSlot*)Addr-Slots);
      for(DWORD i=0;i<SlotsNum;++i)
       if(Points[i])
           {
            Points[i]->MasterSlot=this;
		    if(Points[i]->GetAddr()==Addr)
                return Points[i];
           }
	  return NULL;
	}
	else
	 if(LinkedPool)
		 return LinkedPool->FindSlotByAddr(Addr);
	 else
		 return NULL;
}



template< class A>
CTemporalyPointer<A>::CTemporalyPointer()
{
    Slot=UsedSlot=0;
}


template < class A >
CTemporalyPointer<A>::CTemporalyPointer(MemoryPool* Master,DWORD size,bool Absolute)
{
    if(!Master) Master=DataTemporalyPool;
    if(Master)
        ptr=Master->Alloc(size*sizeof(Item));
    if(!Master || !ptr)
        ptr=new SlotPoint;
}

template < class A >
CTemporalyPointer<A>::~CTemporalyPointer()
{
	delete ptr;
}

template < class A >
SlotPoint* CTemporalyPointer<A>::operator->()
{
 return &ptr;
}

void *_G_NEW(WORD Size)
{
 SlotPoint *PT=GlobalDataPool->Alloc(Size);
 if(!PT) return NULL;
 return LPVOID(PT);
}

void *_L_NEW(WORD Size)
{
 return _G_NEW(Size);
}


struct MemListElem
{
 PMemoryPool Pool;
 MemListElem *Next;
 MemListElem **Last(){ if(Next) return Next->Last();else return &Next;}
 MemListElem(PMemoryPool P){Pool=P;Next=NULL;}

};

struct CMemListInfo
{
 DWORD Allo,AlloB,AlloF,AlloBF;
 DWORD Dea,DeaB,DeaF,DeaBF;
 DWORD Rea,ReaB,ReaF,ReaBF;
 DWORD Cpy,CpyB,CpyF,ClyBF;
 CMemListInfo(){Clear();}
 void Clear()
 {
   Allo=AlloB=AlloF=AlloBF =0;
   Dea =DeaB =DeaF =DeaBF  =0;
   Rea =ReaB =ReaF =ReaBF  =0;
   Cpy =CpyB =CpyF =ClyBF  =0;
 }
};

static CMemListInfo MemListInfo;



 MemListElem *MLFirst;

void MemList::Add(PMemoryPool Pool)
{
 MemListElem* MLE;
 if(!MLFirst){ MLE=MLFirst=new MemListElem(Pool);return;}
 else *MLFirst->Last()=new MemListElem(Pool);
}
void MemList::Swap()
{
 if(MLFirst)
  {
   MemListElem* MLE=MLFirst;
   MLE->Pool->Optimize();
   while(MLE=MLE->Next)
       MLE->Pool->Optimize();
  } 
   MemListInfo.Clear();
}

bVector4d MemList::GetNumAlloc()
{
 return bVector4d(MemListInfo.AlloB,MemListInfo.Allo,MemListInfo.AlloBF,MemListInfo.AlloF);
}
bVector4d MemList::GetNumDealloc()
{
 return bVector4d(MemListInfo.DeaB,MemListInfo.Dea,MemListInfo.DeaBF,MemListInfo.DeaF);
}
bVector4d MemList::GetNumRealloc()
{
 return bVector4d(MemListInfo.ReaB,MemListInfo.Rea,MemListInfo.ReaBF,MemListInfo.ReaF);
}
DWORD MemList::GetMemDelta()
{
 return MemListInfo.AlloB-MemListInfo.DeaB;
}
bVector2d MemList::GetMemCpy()
{
 return bVector2d(MemListInfo.CpyB,MemListInfo.Cpy);
}

void MemList::RegAlloc(DWORD NB)
{
  ++MemListInfo.Allo;
  MemListInfo.Allo+=NB;
}
void MemList::RegRea  (DWORD NB)
{
  ++MemListInfo.Rea;
  MemListInfo.ReaB+=NB;
}
void MemList::RegDea  (DWORD NB)
{
  ++MemListInfo.Dea;
  MemListInfo.DeaB+=NB;
}


void* GmemALLOC(DWORD NumBytes)
{
 /**/
 SlotPoint *P=LittleGlobalPool->Alloc(NumBytes+8);
 DWORD *Data=(DWORD*)P->GetAddr();
 Data[0]=(DWORD)P;
 Data[1]=(DWORD)P;
 //Print2Console(-1,"GAlloc of %d -> %x|%d|%x",NumBytes,Data,P->GetSlot(),P->GetMasterSlot()); 
 return Data+2;
 /**/

}
void* GmemREALLOC(void* OldPtr,DWORD NewSize)
{
  if(!OldPtr)return GmemALLOC(NewSize);
  DWORD *Data=(DWORD*)OldPtr;
  Data=Data-2;
  SlotPoint *P=(SlotPoint*)*Data;
  ASSERT(P->GetMasterSlot()==LittleGlobalPool);
  P->Realloc(NewSize+8);
  Data=(DWORD*)P->GetAddr();
  Data[0]=(DWORD)P;
  Data[1]=(DWORD)P;
  //Print2Console(-1,"GRealloc of %d -> %x-%x|%d|%x",NewSize,(DWORD*)OldPtr-2,Data,P->GetSlot(),P->GetMasterSlot());
  //Print2Console(-1,"GRealloc of %d, %x->%x",NewSize,OldPtr,Data);
  return Data+2;
}
void  GmemFREE(void* Ptr)
{
  DWORD *Data=(DWORD*)Ptr;
  Data=Data-2;
  SlotPoint *P =(SlotPoint*)Data[0];
  SlotPoint *P2=(SlotPoint*)Data[1];
  if(P!=P2)
  {
   if(P->GetMasterSlot()!=LittleGlobalPool)
       P=P2;
  }
  if(P->GetMasterSlot()!=LittleGlobalPool)
  {
     P=LittleGlobalPool->FindSlotByAddr(Data);
     return;
  }
  //Print2Console(-1,"GFree of %x",Data);
  if(P)
  delete P;
}