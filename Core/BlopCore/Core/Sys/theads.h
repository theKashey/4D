//---------------------------------------------------------------------------

#ifndef theadsH
#define theadsH
//---------------------------------------------------------------------------

#define Vfork(a) CreateThread(NULL, 0,  (LPTHREAD_START_ROUTINE)a,NULL, 0, NULL)

#define WAITABLECYCLE(a) while(1) if(a) break;

#endif
