//===========================================================
//  Kashey. OpenAL sound sybsystem
//===========================================================

#include "stdafx.h"
#include "remSnd.h"
#include <windows.h>
#include "al.h"
#include "alc.h"
#include "alu.h"
#include "alut.h"
#include "alinit.h"

#include "vorbis/codec.h"

#pragma comment(lib, "lib/alut.lib")
#pragma comment(lib, "lib/OpenAL32.lib")
#pragma comment(lib, "lib/ogg.lib")
#pragma comment(lib, "lib/vorbisfile.lib")



/*
void remSnd::Move(float X, float Y, float Z)
{
	ALfloat Pos[3] = { X, Y, Z };
	alSourcefv(mSourceID, AL_POSITION, Pos);
	CheckALError();
	float MD=MySound.MAX_DISTANCE*SOUND_WORLD_SCALE;
	float Dist=(ListenerPos-Pos).Length();
	//return;
	if(Dist<MD)
	{
		if(MySound.DISTANCE_MUTE_MODEL!=DISTANCE_FALLOF_GAIN)
			if(UserStoped)Play();
	switch(MySound.DISTANCE_MODEL)
	{
	 case DISTANCE_MODEL_USERFALLOFF:	
		Dist=MIN(Dist,MD);
 		Dist/=MD;
	    float GAIN=MySound.GAIN*(1-Dist);
		GainCtrl(GAIN);
		break;
	}
	}
	else
	{
		GainCtrl(0);
	switch(MySound.DISTANCE_MUTE_MODEL)
	{
		case DISTANCE_FALLOF_GAIN :GainCtrl(0);break;
		case DISTANCE_FALLOF_MUTE :GainCtrl(0);break;
		case DISTANCE_FALLOF_PAUSE:Pause();break;
		case DISTANCE_FALLOF_VPLAY://alPauseSource(mSourceID);break;
		case DISTANCE_FALLOF_STOP :
		default:                   Stop();break;
	};
	}
}

*/
