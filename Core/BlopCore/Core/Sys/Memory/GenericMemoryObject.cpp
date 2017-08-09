#include "ProjectSpace.h"
#include "genericmemoryobject.h"

int TotalAllocations      =0;
int TotalDeallocations    =0;
int TotalAllocationSpace  =0;
int TotalDeallocationSpace=0;

int FrameAllocations      =0;
int FrameDeallocations    =0;
int FrameAllocationsSpace =0;
int FrameDeallocationSpace=0;

void ClearMemoryFrameStatictic()
{
	FrameAllocations      =
	FrameDeallocations    =
    FrameAllocationsSpace =
	FrameDeallocationSpace=0;
}

void GetMemoryTotalStatictic(int &_TotalAllocations      ,
                             int &_TotalDeallocations    ,
                             int &_TotalAllocationSpace  ,
                             int &_TotalDeallocationSpace)
{
  _TotalAllocations      =TotalAllocations;
  _TotalDeallocations    =TotalDeallocations;
  _TotalAllocationSpace  =TotalAllocationSpace;
  _TotalDeallocationSpace=TotalDeallocationSpace;
}

void GetFrameTotalStatictic (int &_FrameAllocations      ,
                             int &_FrameDeallocations    ,
                             int &_FrameAllocationsSpace ,
                             int &_FrameDeallocationSpace)
{
  _FrameAllocations       =FrameAllocations;
  _FrameDeallocations     =FrameDeallocations;
  _FrameAllocationsSpace  =FrameAllocationsSpace;
  _FrameDeallocationSpace=FrameDeallocationSpace;
}

PVOID bGetMem(size_t Size)
{
	TotalAllocations++;
	FrameAllocations++;
	TotalAllocationSpace+=Size;
	FrameAllocationsSpace+=Size;
	return malloc(Size);
}
void  bFreeMem(PVOID Src) 
{
	TotalDeallocations++;
	FrameDeallocations++;
	TotalDeallocationSpace++;
	FrameDeallocationSpace++;
	return free  (Src);
}


/*PVOID MemoryStruct::operator new     (size_t Size)  {return bGetMem(Size);}
PVOID MemoryStruct::operator new[]   (size_t Size)  {return bGetMem(Size);}
void  MemoryStruct::operator delete  (PVOID Src)    {return bFreeMem(Src);}
void  MemoryStruct::operator delete[](PVOID Src)    {return bFreeMem(Src);}

PVOID MemoryClass::operator new     (size_t Size)   {return bGetMem(Size);}
PVOID MemoryClass::operator new[]   (size_t Size)   {return bGetMem(Size);}
															
void  MemoryClass::operator delete  (PVOID Src)     {return bFreeMem(Src);}
void  MemoryClass::operator delete[](PVOID Src)     {return bFreeMem(Src);}
*/


class CMemoryCleaner: public BlopBaseObject
{
public:
	void Tick(float Time)
	{
	  ClearMemoryFrameStatictic();
	}
};

MAKE_PLUGIN MEMORYPLUGIN(new CMemoryCleaner,"InlineMemory",0,true);