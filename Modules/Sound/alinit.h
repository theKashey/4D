#ifndef _AL_INIT_H
#define _AL_INIT_H

bool			InitializeOpenAL();
void			DestroyOpenAL   ();
ALboolean       CheckALCError   ();

#ifndef __FUNCTION__
#define __FUNCTION__ "snd"
#endif

bool FetchALError();

#ifdef DEBUG
#define CheckALError() _CheckALError(__FUNCTION__,__LINE__)
#else
#define CheckALError() _CheckALError() 
#endif

#ifdef DEBUG
ALboolean _CheckALError(LPCSTR FuncName,long Line);
#else
ALboolean _CheckALError();
#endif

#endif