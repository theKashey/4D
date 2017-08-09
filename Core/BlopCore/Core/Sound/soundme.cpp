/*===========================================================================
    Заголовок: SoundMe.cpp
    Автор: Kashey
    Информация: Sound 3th interface

===========================================================================*/
#include "ProjectSpace.h"

//using namespace Sound;
//using namespace Set;
Sound::Set::soundCreate       _soundCreate      =NULL;
Sound::Set::soundPlay         _soundPlay        =NULL;
Sound::Set::soundGain         _soundGain        =NULL;
Sound::Set::soundStop         _soundStop        =NULL;
Sound::Set::soundKill         _soundKill        =NULL;
Sound::Set::soundPosSet       _soundPosSet      =NULL;
Sound::Set::soundDuplicate    _soundDuplicate   =NULL;
Sound::Set::soundListenPos    _soundListenPos   =NULL;
Sound::Set::soundListenOri    _soundListenOri   =NULL;
Sound::Set::soundOverallGain  _soundOverallGain =NULL;
Sound::Set::soundListenPos    _soundWorldSC     =NULL;
Sound::Set::soundVoid         _soundClearTHG    =NULL;
Sound::Set::soundTHGain       _soundSetTHG      =NULL;
Sound::Set::soundGain         _soundGainFor     =NULL;

void Sound::Set::SetCreateCallBack      (soundCreate      F){_soundCreate     =F;}
void Sound::Set::SetPlayCallBack        (soundPlay        F){_soundPlay       =F;}
void Sound::Set::SetGainCallBack        (soundGain        F){_soundGain       =F;}
void Sound::Set::SetStopCallBack        (soundStop        F){_soundStop       =F;}
void Sound::Set::SetKillCallBack        (soundKill        F){_soundKill       =F;}
void Sound::Set::SetPosCallBack	        (soundPosSet      F){_soundPosSet     =F;}
void Sound::Set::SetDupCallBack         (soundDuplicate   F){_soundDuplicate  =F;}
void Sound::Set::SetListenPosCallBack   (soundListenPos   F){_soundListenPos  =F;}
void Sound::Set::SetListenOriCallBack   (soundListenOri   F){_soundListenOri  =F;}
 
void Sound::Set::SetOverallGainCallBack (soundOverallGain F){_soundOverallGain=F;}
void Sound::Set::SetWorldScaleCallBack  (soundListenPos   F){_soundWorldSC    =F;}
void Sound::Set::SetClearTHCallBack     (soundVoid        F){_soundClearTHG   =F;}
void Sound::Set::SetTHGainCallBack      (soundTHGain      F){_soundSetTHG     =F;}
void Sound::Set::SetGainForCallBack     (soundGain        F){_soundGainFor    =F;}
//void Sound::Set::GainControl         

int   Sound::CreateSound(LPCSTR FileName            )
{
	if(_soundCreate   )	return _soundCreate(FileName);
	else                return 0;
}
void  Sound::PlaySound     (int SID)		         {if(_soundPlay     ) _soundPlay(SID,0);}
void  Sound::PlayOnce      (int SID)                 {if(_soundPlay     ) _soundPlay(SID,SND_PLAY_ONCE);}
void  Sound::StopSound     (int SID)		         {if(_soundStop     ) _soundStop(SID);}
void  Sound::KillSound     (int SID)	      	     {if(_soundKill     ) _soundKill(SID);}
int   Sound::DuplicateSound(int SID)                 {if(_soundDuplicate) return _soundDuplicate(SID);return 0;}
void  Sound::SetSoundPos   (int SID,bVector3f Pos)   {if(_soundPosSet   ) _soundPosSet(SID,Pos);}
int   Sound::GainControl   (int SID,float Gain)      {if(_soundGain)_soundGain(SID,Gain);return 0;};


void  Sound::SetListenerPosition   (bVector3f &Pos)                  {if(_soundListenPos)_soundListenPos(Pos);}
void  Sound::SetListenerOrientation(bVector3f &_At,bVector3f &_Up)	 {if(_soundListenOri)_soundListenOri(_At,_Up);}
void  Sound::SetOverallGain        (const float Gain)                {if(_soundOverallGain)_soundOverallGain(Gain);}
void  Sound::SetGainFor            (const DWORD Type,const float Gain){if(_soundGainFor)_soundGainFor(Type,Gain);}
void  Sound::SetWorldScale         (bVector3f S)                     {if(_soundWorldSC)_soundWorldSC(S);};
void  Sound::ClearThreadedGainControl()                              {if(_soundClearTHG)_soundClearTHG();}
void  Sound::AddThreadedGainControl(DWORD TimeToDo,CGainPack * Gains){if(_soundSetTHG)_soundSetTHG(TimeToDo,Gains);}
