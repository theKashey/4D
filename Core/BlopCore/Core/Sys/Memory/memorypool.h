// MemoryPool.h: interface for the CTemporalyPointer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYPOOL_H__A19DE71A_047F_4264_B924_BFB61BB6B56E__INCLUDED_)
#define AFX_MEMORYPOOL_H__A19DE71A_047F_4264_B924_BFB61BB6B56E__INCLUDED_

class MemoryPool;
typedef MemoryPool *PMemoryPool;

class DLLTYPE SlotPoint
{
    //void **AccocPtr;
    //WORD ID;
    WORD MySlot,Len;
	WORD MyID;
    PMemoryPool MasterSlot;
    //DWORD UsedLen;
public:
    SlotPoint();
    //void       SetAccocPtr(void *_Ptr);
    //void       CheckPlace()           ;
    SlotPoint(WORD MID,PMemoryPool _Master,WORD _MySlot,WORD _Len);

    bool Realloc(DWORD NewLen);
    bool Defragment();

    ~SlotPoint();
    void Unvalid();
    //void Merge(SlotPoint *Merging);

    void *GetAddr();
    DWORD Size();
    WORD  GetSlot();
    WORD  GetSlotNum();
    operator LPVOID();
    PMemoryPool GetMasterSlot();
    friend class MemoryPool;
};

typedef struct _SingleSlot{DWORD dpoint;} SingleSlot;
typedef SlotPoint *PSlotPoint;

class DLLTYPE MemoryPool
{
    SingleSlot *Slots;
    BYTE       *SlotUsed;
    SlotPoint  **Points;
    DWORD      SlotsNum;
    WORD       SlotSize,DSlot,dSlotSize;
    DWORD      _MAP(DWORD SlotID);
	MemoryPool *LinkedPool;
    int        NumSlotsFree;
    DWORD      MaximumSlotFree;
    DWORD      MaximumSlotPos;

	DWORD      FirstFreeBlock;
	DWORD      LastBlockFree;
    DWORD      NumOperations;
	bool       ChildBlock;
public:
    friend class SlotPoint;
    MemoryPool(DWORD ByteCount,WORD _SlotSize=0xFF,bool ChildBlock=false);
   ~MemoryPool();
    SlotPoint *Alloc(DWORD NumBytes);
    void FreeSlot(DWORD SlotID,BYTE Delete=1);
    void FreeSlot(SlotPoint *Slot,BYTE Delete=1);
    bool ReallocSlot(SlotPoint* OldSlot,DWORD NewSize);
    bool Defragment();
	bool Optimize();
    SlotPoint *AddSlot(const WORD x,const WORD len,DWORD NumBytes);
    void *GetAt(WORD SlotN);
    void *GetAt(SlotPoint* Slot);
	SlotPoint *FindSlotByAddr(LPCVOID Addr);
    DWORD BytesFree();
    DWORD BytesAlloc();
    DWORD GetMinimumSize();
    void VALIDATEOVERRUN(DWORD FOR);  

	bool IsFree()const;

    //static MemoryPool* GetGlobalDataPool();
};

template <class A>
class CTemporalyPointer
{
        SlotPoint ptr;
public:
	typedef CTemporalyPointer Item,*ItemRef;

    CTemporalyPointer();
	CTemporalyPointer(MemoryPool *Master,DWORD size,bool Absolute=false);
    CTemporalyPointer(DWORD size,bool Absolute=false);
	~CTemporalyPointer();
    SlotPoint* operator->();
};




//defaults POOLs

//temporaly pool
extern PMemoryPool GlobalDataPool;
extern PMemoryPool DataTemporalyPool;

extern PMemoryPool GlobalStringPool;
extern PMemoryPool StringTemporalyPool;

extern PMemoryPool LittleGlobalPool;
extern PMemoryPool LittleTemporalyPool;

void *_G_NEW(DWORD Size);
void *_L_NEW(DWORD Size);






//*****************************************************
#define _KB 1024
#define _MB 1048576

PMemoryPool DLLTYPE GetGlobalDataPool();

extern WORD __MemDataSlotSize     ;
extern WORD __MemStringSlotSize   ;
extern WORD __MemLittleSlotSize   ;

extern DWORD __MemGlobalDataAlloc  ;
extern DWORD __MemTempDataAlloc    ;
extern DWORD __MemGlobalStringAlloc;
extern DWORD __MemTempStringAlloc  ;
extern DWORD __MemGlobalLittleAlloc;
extern DWORD __MemTempLittleAlloc  ;

void __InitMem();
void __KillMem();


class DLLTYPE MemList
{
public:
    static void Add(PMemoryPool Pool);
    static void Swap();

    static bVector4d GetNumAlloc();
    static bVector4d GetNumDealloc();
    static bVector4d GetNumRealloc();
    static DWORD     GetMemDelta();
    static bVector2d GetMemCpy();

    static void RegAlloc(DWORD NB);
    static void RegRea  (DWORD NB);
    static void RegDea  (DWORD NB);
};

extern "C" DLLTYPE void* GmemALLOC(DWORD NumBytes);
extern "C" DLLTYPE void* GmemREALLOC(void* OldPtr,DWORD NewSize);
extern "C" DLLTYPE void  GmemFREE(void* Ptr);


#endif // !defined(AFX_MEMORYPOOL_H__A19DE71A_047F_4264_B924_BFB61BB6B56E__INCLUDED_)
