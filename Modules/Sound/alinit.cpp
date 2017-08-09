#include "StdAfx.h"
#include "al.h"
#include "alc.h"
#include "alu.h"
#include "alut.h"
#include "alinit.h"
#include "remSnd.h"
//#include <map>
//#include <sstream>
#include "soundbuffer.h"

bVector3f ListenerPos;

//using namespace std;

ALCdevice	*pDevice;
ALCcontext	*pContext;


bool FetchALError()
{
    return alGetError() == AL_NO_ERROR;
}

ALboolean CheckALCError()
{
	ALenum ErrCode;
	GString Err = "ALC error: ";
	if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
	{
		Err += (char *)alcGetString(pDevice, ErrCode);
		Error2Console(SYS_SOUND,Err.c_str());
		return AL_FALSE;
	}
	return AL_TRUE;
}

#ifndef __FUNCTION__
#define __FUNCTION__ "UAR"
#endif



#ifdef DEBUG
ALboolean _CheckALError(LPCSTR FuncName,long Line)
#else
ALboolean _CheckALError()
#endif
{
	ALenum ErrCode;
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
        GString Err = "OpenAL error: ";
#ifdef DEBUG
    	Err.Format("OpenAL error(%s:%d):",FuncName,Line);
#endif
		Err += (char *)alGetString(ErrCode);
		Error2Console(SYS_SOUND,Err.c_str());
		return AL_FALSE;
	}
	return AL_TRUE;
}

bool InitializeOpenAL()
{

	// Position of the Listener.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

	// Velocity of the Listener.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	//LPCSTR defaultDevice,deviceList;
	//GString DD="ALC_ENUMERATION_EXT";
//	bool IEE=alcIsExtensionPresent(NULL,(ALCubyte*)DD.v_str());
//	if(IEE)
	//{
     //defaultDevice = (LPCSTR)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
     //deviceList    = (LPCSTR)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	//}

	// Open default sound device
	pDevice = alcOpenDevice(NULL);
	// Check for errors
	if (!pDevice)
	{
		Error2Console(SYS_SOUND,"Default sound device not present");
		return false;
	}
	// Creating rendering context
	pContext = alcCreateContext(pDevice, NULL);
	if (!CheckALCError()) return false;

	alcMakeContextCurrent(pContext);

	// Set listener properties  
	alDistanceModel(AL_INVERSE_DISTANCE);

	alListenerf (AL_GAIN    ,    0.0f);
	alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
	Print2Console(SYS_SOUND,GetSndInfo().c_str());
	return true;
}

void DestroyOpenAL()
{
	// Clear all buffers and sources
	for (_SOUNDBUFFERARRAY::iterator i = BuffersPack.begin(); i != BuffersPack.end(); i++)
		  i->Kill(-10);
	// Shut down context
	alcMakeContextCurrent(NULL);
	// Destroy context
	alcDestroyContext(pContext);
	// Close sound device
	alcCloseDevice(pDevice);
}

GString GetSndInfo()
{
	GString SOut;
	GString Line;
	SOut = "ALC info: \n";
	Line.Format("DEFAULT_DEVICE_SPECIFIER: %s \n" ,(char *)alcGetString(pDevice, ALC_DEFAULT_DEVICE_SPECIFIER));SOut+=Line;
	Line.Format("DEVICE_SPECIFIER: %s\n", (char *)alcGetString(pDevice, ALC_DEVICE_SPECIFIER));SOut+=Line;
	Line.Format("EXTENSIONS: %s\n",(char *)alcGetString(pDevice, ALC_EXTENSIONS));SOut+=Line;
	Line.Format("AL info: \n");SOut+=Line;
	Line.Format("VERSION: %s\n", (char *)alGetString(AL_VERSION));SOut+=Line;
	Line.Format("RENDERER: %s\n", (char *)alGetString(AL_RENDERER));SOut+=Line;
	Line.Format("VENDOR: %s\n" , (char *)alGetString(AL_VENDOR));SOut+=Line;
	Line.Format("EXTENSIONS: %s\n" , (char *)alGetString(AL_EXTENSIONS));SOut+=Line;
	return SOut;
}

