//---------------------------------------------------------------------------

#ifndef blopThreadsH
#define blopThreadsH
//---------------------------------------------------------------------------
extern DWORD _blpLastThreadID;

#define _StartFunctionThread(a) CreateThread(NULL, 0,  (LPTHREAD_START_ROUTINE)a       ,(LPVOID)NULL, 0, &_blpLastThreadID);
#define _StartObjectedThread(a) CreateThread(NULL, 0,  (LPTHREAD_START_ROUTINE)a       ,(LPVOID)this, 0, &_blpLastThreadID);

#define _DeclareObjectedThreadFunc(a,b,c) void c(void *P){((a*)P)->b();}


#endif
