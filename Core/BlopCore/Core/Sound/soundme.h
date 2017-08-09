#pragma once
#ifndef _SOUND_ME_H
#define _SOUND_ME_H

#define SND_PLAY_ONCE             1

namespace Sound
{

#define SND_GAIN_OVERALL         0
#define SND_GAIN_ALL             1
#define SND_GAIN_MUSIC           2
#define SND_GAIN_FX              3
#define SND_GAIN_SPEECH          4
#define SBD_GAIN_AMBIENT         5

struct CGainPack
{
	DWORD TimeStart;
	float GainFor[0xF];
	bool  GainChange[0xF];
    CGainPack()
	{
		ZeroMemory(GainChange,sizeof(GainChange));
	}
	void SetGainFor(const DWORD Type,const float Rate)
	{
		if(Type>0xF-1) return;
		GainFor   [Type]=Rate;
		GainChange[Type]=true;
	}
	bool IsValid(const DWORD Type)
	{
		if(Type>0xF-1) return false;
		return GainChange[Type];
	}
	int GetGainID(DWORD Start=0)
	{
		for(int i=Start;i<0xF-1;++i)
			if(GainChange[i]) return i;
		return -1;
	}
	float GetGain(const DWORD Type)
	{
		if(Type>0xF-1) return false;
		return GainFor[Type];
	}
};


  namespace Set
  {
   typedef int  (*soundCreate     )(LPCSTR FileName);
   typedef void (*soundPlay       )(int SID,DWORD Params);
   typedef void (*soundGain       )(int SID,float Gain);
   typedef void (*soundStop       )(int SID);
   typedef void (*soundKill       )(int SID);
   typedef void (*soundPosSet     )(int SID,bVector3f &Pos);
   typedef DWORD(*soundDuplicate  )(int SID);
   typedef void (*soundListenPos  )(bVector3f &Pos);
   typedef void (*soundListenOri  )(bVector3f &_At,bVector3f &_Up);
   typedef void (*soundOverallGain)(const float Gain);
   typedef void (*soundVoid)       ();
   typedef void (*soundTHGain)     (DWORD TimeToDo,CGainPack * Gains);

  
   void DLLTYPE SetCreateCallBack     (soundCreate      F);
   void DLLTYPE SetPlayCallBack	      (soundPlay        F);
   void DLLTYPE SetGainCallBack       (soundGain        F);
   void DLLTYPE SetStopCallBack	      (soundStop        F);
   void DLLTYPE SetKillCallBack	      (soundKill        F);
   void DLLTYPE SetPosCallBack	      (soundPosSet      F);
   void DLLTYPE SetDupCallBack        (soundDuplicate   F);
   void DLLTYPE SetListenPosCallBack  (soundListenPos   F);
   void DLLTYPE SetListenOriCallBack  (soundListenOri   F);
   void DLLTYPE SetOverallGainCallBack(soundOverallGain F);
   void DLLTYPE SetWorldScaleCallBack (soundListenPos   F);
   void DLLTYPE SetClearTHCallBack    (soundVoid        F);
   void DLLTYPE SetTHGainCallBack     (soundTHGain      F);
   void DLLTYPE SetGainForCallBack    (soundGain        F);
  }

  int   DLLTYPE CreateSound     (LPCSTR FileName);
  void  DLLTYPE PlaySound       (int SID);
  void  DLLTYPE PlayOnce        (int SID);
  void  DLLTYPE StopSound       (int SID);
  void  DLLTYPE KillSound       (int SID);
  void  DLLTYPE SetSoundPos     (int SID,bVector3f Pos);
  int   DLLTYPE DuplicateSound  (int SID);
  int   DLLTYPE GainControl     (int SID,float Gain);

  void  DLLTYPE SetListenerPosition   (bVector3f &Pos);
  void  DLLTYPE SetListenerOrientation(bVector3f &_At,bVector3f &_Up);
  void  DLLTYPE SetOverallGain        ( float Gain);

  void  DLLTYPE SetGainFor            ( DWORD Type, float Gain);
  void  DLLTYPE SetWorldScale         (bVector3f S);

  void  DLLTYPE ClearThreadedGainControl();
  void  DLLTYPE AddThreadedGainControl(DWORD TimeToDo,CGainPack * Gains);
};

#endif