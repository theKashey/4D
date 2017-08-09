//===========================================================
//  Filename: remSnd.h
//      Sound class.
//      Copyright 2003
//      Created by Roman E. Marchenko, vortex@library.ntu-kpi.kiev.ua
//      $Id: remSnd.h,v 1.2 2003/09/26 21:04:57 vortex Exp $
//===========================================================

#ifndef _REM_SND_H_
#define _REM_SND_H_

#include "al.h"
//#include <fstream>
#include "vorbis/vorbisfile.h"  
#include "../Parser/Sounds/SoundShader.h"

//#define NUM_OF_DYNBUF	2		// num buffers in queue

#define SOUND_WORLD_SCALE 0.1f
extern GString	GetSndInfo();
void   SetListenerOri(bVector3f &_At,bVector3f &_Up);
void   SetListenerPos(bVector3f &_Pos);
extern bVector3f ListenerPos;

#endif 
