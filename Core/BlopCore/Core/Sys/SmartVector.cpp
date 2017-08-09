#include "ProjectSpace.h"
#include "SmartVector.h"

DWORD CBaseSmartVector::_SmartReadyStart=0x096AC;

CBaseSmartVector::CBaseSmartVector()
{
	SetGrowSize(20);
}

extern bool gMEM2TCollapsing;

MemoryPool * SMEM=NULL;

bool CBaseSmartVector::SmartReadyStart()
{
	return _SmartReadyStart==0xABDF0;
}

LPVOID CBaseSmartVector::PoolMem(LPVOID From,const DWORD Size)
{
	  if(gMEM2TCollapsing)return NULL;
	  if(!SMEM) SMEM=new MemoryPool(4160*256*12,256);
	  if(From)
	  {
		  LPCSTR Buf=(LPCSTR)From-4;
          SlotPoint *Slot=(SlotPoint*)(*((int*)Buf));
          if(!Slot || Slot->GetMasterSlot()!=SMEM)
		   {
		    
		    SlotPoint * NSlot=SMEM->FindSlotByAddr(Buf);
			ASSERT(NSlot==Slot);
			Slot=NSlot;
		   }

		  if(Size<=0)
		  {
			  delete Slot;
			  return NULL;
		  }
		  else
		  {
			  if(Slot->Size()<Size)
			  {
				  //
				  delete Slot;
				  LPVOID Ret= PoolMem(NULL,Size);
				  if(Ret!=From)
				   memcpy(Ret,From,_size*elem_size());
				  
				  return Ret;
			  }
			  else
		      return From;
		  }
	  }
	  else
	  {
		 SlotPoint *SP=SMEM->Alloc(Size+4);
         if(!SP)
		 {
			 ASSERT("out of mem" && FALSE);
			 return NULL;
		 }
         LPSTR Buf=(char*)SP->GetAddr();
		 int *SL=(int*)Buf;
         *SL=(int)SP;
         Buf+=4;
		 return Buf;
	  }
}

