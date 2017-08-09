//---------------------------------------------------------------------------

#ifndef SectionLockerH
#define SectionLockerH
//---------------------------------------------------------------------------
class  SectionLocker
{
 CRITICAL_SECTION Locker;
 int            LockCount;
public:
 SectionLocker()
 { InitializeCriticalSection(&Locker);LockCount=0;}
~SectionLocker()
 { DeleteCriticalSection    (&Locker); }
 void Lock()  { EnterCriticalSection(&Locker);LockCount++;}
 void Unlock(){ LeaveCriticalSection(&Locker);LockCount--;}
};

class  SectionHardLocker
{
 CRITICAL_SECTION Locker;
 DWORD            LC;
public:
 SectionHardLocker()
 { LC=0;InitializeCriticalSection(&Locker);}
~SectionHardLocker()
 { DeleteCriticalSection    (&Locker); }
 void Lock()  { if(LC) WAITABLECYCLE(!LC); ++LC; EnterCriticalSection(&Locker);}
 void Unlock(){ LC--; LeaveCriticalSection(&Locker);}
};

class /*DLLTYPE*/ DynamicLocker
{
  SectionLocker     * SLocker;
  SectionHardLocker * HLocker;
public:
  DynamicLocker(SectionLocker     &SL){ SLocker=&SL;SL.Lock();}
  DynamicLocker(SectionHardLocker &SL){ HLocker=&SL;SL.Lock();SLocker=NULL;}
 ~DynamicLocker()                     { if(SLocker)SLocker->Unlock();else HLocker->Unlock();}
};

#define _LOCK(a) DynamicLocker NearLocker(a);

#endif
