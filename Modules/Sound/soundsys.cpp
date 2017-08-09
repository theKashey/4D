#include "StdAfx.h"
#include "remSnd.h"
#include "soundsource.h"
#include "soundsys.h"
#include "alinit.h"
#include "SoundBuffer.h"

using namespace Sound;


class  tSectionLocker
{
 CRITICAL_SECTION Locker;
 int            LockCount;
public:
 tSectionLocker()
 { InitializeCriticalSection(&Locker);LockCount=0;}
~tSectionLocker()
 { DeleteCriticalSection    (&Locker); }
 void Lock()  { EnterCriticalSection(&Locker);LockCount++;if(LockCount>1){ASSERT(FALSE);}}
 void Unlock(){ LockCount--;LeaveCriticalSection(&Locker);}
};


class /*DLLTYPE*/ tDynamicLocker
{
  tSectionLocker     * SLocker;
public:
  tDynamicLocker(tSectionLocker     &SL){ SLocker=&SL;SL.Lock();}
 ~tDynamicLocker()                     { if(SLocker)SLocker->Unlock();}
};
tSectionLocker  SOUND_LOCKER;
#define SLOCK  tDynamicLocker NearLock(SOUND_LOCKER);

bVector3f WorldScale;

bool IsGlobalGainChanged    =false;
bool IsGlobalListenerChanged=true;
bool IsTypeGainChanged      =false;
float OverallGain           =1.0f;
float AllGain               =1.0f;

DWORD SoundChannelsUsed=0;



DWORD SYS_SOUND=0;

PSoundSys MySoundSys;
SmartVector<PSound> Sounds;
MakeProfile(SoundProf,"Sound");

HANDLE SoundThread;
HANDLE SoundEvent,SoundAskE;

void Touch()
{
	SetEvent(SoundEvent);
};

PSound t_ResultSound=NULL;
bVector3f PosVector;
float *tmpPos=NULL;
DWORD t_PlayParams=0;
float fData;

//requests
void WINAPI t_CreateSound(DWORD Data)
{
   t_ResultSound=MySoundSys->Load((LPCSTR)Data);
   SetEvent(SoundAskE);
}

void WINAPI t_PlaySound(DWORD Data)
{
	((PSound)Data)->Play(t_PlayParams);
	t_PlayParams=0;
}

void WINAPI t_GainSound(DWORD Data)
{
	((PSound)Data)->SetGain(fData);//>Play(t_PlayParams);
	t_PlayParams=0;
}


void WINAPI t_PlayStop(DWORD Data)
{
	((PSound)Data)->Stop();
	t_PlayParams=0;
}

void WINAPI t_PlaySetPos(DWORD Data)
{
	((PSound)Data)->SetPosition(PosVector);;
}

float tmpListenerPos1[9];
float tmpListenerPos2[9];
void WINAPI t_PlaySetLPos(DWORD Data)
{
	alListenerfv(AL_POSITION,    tmpListenerPos1);
}

void WINAPI t_PlaySetLOri(DWORD Data)
{
	alListenerfv(AL_ORIENTATION, tmpListenerPos2);
}

struct SounQstr
{
	PAPCFUNC ptr;
	DWORD param;
};

SounQstr SoundQ[0xFF];
int	 CutCountQ=0;

int QueueSound(PAPCFUNC ptr,HANDLE SoundTH,DWORD Param)
{
	SLOCK
	 SoundQ[CutCountQ].ptr  =ptr;
     SoundQ[CutCountQ].param=Param;
	 CutCountQ++;
	 Touch();
	 return 0;
};

void UnquerySound()
{
	for(int i=0;i<CutCountQ;++i)
		if(SoundQ[i].ptr)
			SoundQ[i].ptr(SoundQ[i].param);
	CutCountQ=0;
}

PSound RequestToLoad(LPCSTR Str)
{
	t_ResultSound=NULL;
	ResetEvent(SoundAskE);
	int ret=QueueSound(t_CreateSound, SoundThread, (DWORD)Str); 
	ret=WaitForSingleObject(SoundAskE,1000);
	return t_ResultSound;
}

void RequestToPlay(PSound Snd,DWORD Params)
{
	t_PlayParams=Params;
	if(!Snd) return;
	QueueSound(t_PlaySound, SoundThread, (DWORD)Snd); 
}

void RequestToGain(PSound Snd,float Params)
{
	fData=Params;
	if(!Snd) return;
	QueueSound(t_GainSound, SoundThread, (DWORD)Snd); 
}

void RequestToStop(PSound Snd)
{
	if(!Snd) return;
	QueueSound(t_PlayStop, SoundThread, (DWORD)Snd); 
}

void RequestToSetPos(PSound Snd,const float *Pos)
{
	PosVector=Pos;
	if(!Snd) return;
	QueueSound(t_PlaySetPos, SoundThread, (DWORD)Snd); 
}

void RequestToSetListenerPos(float *Pos)
{
	tmpPos=Pos;
    QueueSound(t_PlaySetLPos, SoundThread, (DWORD)Pos); 
}

void RequestToSetListenerOri(float *Pos)
{
	tmpPos=Pos;
    QueueSound(t_PlaySetLOri, SoundThread, (DWORD)Pos); 
}


//CALLBACK

int _CreateSound(LPCSTR FileName)
{
 if(!FileName) return 0;
 if(FileName[0]=='$')
 {
     if(strcmp(FileName,"$channelsused")==0)
         return SoundChannelsUsed;
 }
 PSound Sound=RequestToLoad(FileName);
 if(!Sound) return 0;
 Sounds.push_back(Sound);
 return Sounds.size();
}
void _PlaySound(int SID,DWORD Params)
{
	if(SID==0)return;
	if(SID>Sounds.size()){ASSERT(FALSE);}
	RequestToPlay(Sounds[SID-1],Params);
}
void _GainSound(int SID,float Gain)
{
	if(SID==0)return;
	if(SID>Sounds.size()){ASSERT(FALSE);}
	RequestToGain(Sounds[SID-1],Gain);
}

void _StopSound(int SID)
{
   if(SID==0)return;
   if(SID>Sounds.size()){ASSERT(FALSE);}
   RequestToStop(Sounds[SID-1]);
}
void _KillSound(int SID)
{
   if(SID>Sounds.size()){ASSERT(FALSE);}
   _StopSound(SID);
   delete Sounds[SID-1];
}
DWORD _DupSound(int SID)
{
   PSound Sound=MySoundSys->Duplicate(Sounds[SID-1]);
   if(!Sound) return 0;
   Sounds.push_back(Sound);
   return Sounds.size();
}


bVector3f GetWorldScale()
{
	return WorldScale;
}

void _SetPosSound(int SID,bVector3f &Pos)
{
	if(SID==0)return;
    if(SID>Sounds.size()){ASSERT(FALSE);}
	bVector3f SC=GetWorldScale();
	Pos[0]*=SC[0];
	Pos[1]*=SC[1];
	Pos[2]*=SC[2];
	RequestToSetPos(Sounds[SID-1],Pos);
}


bVector3f OldLPos;
void _SetListenerPos(bVector3f &_Pos)
{	
	if((OldLPos-_Pos).Length()<0.01f) return;
	OldLPos=_Pos;
	bVector3f SC=GetWorldScale();
    _Pos[0]*=SC[0];
	_Pos[1]*=SC[1];
	_Pos[2]*=SC[2];
	ALfloat Pos[3] = { _Pos[0], _Pos[1], _Pos[2] };
	ListenerPos=_Pos;
	memcpy(tmpListenerPos1,Pos,sizeof(Pos));
	RequestToSetListenerPos(_Pos);

	IsGlobalListenerChanged=true;
}

bVector3f OldLUp;
bVector3f OldLAt;

void _SetListenerOri(bVector3f &_At,bVector3f &_Up)
{	
	if((OldLUp-_Up).Length()<0.01f &&  (OldLAt-_At).Length()<0.01f)return ;
    OldLUp=_Up;
    OldLAt=_At;
    //Print2Console(SYS_SOUND,"LORIL%f,%f,%f-%f,%f,%f",_At[0], _At[1], _At[2], _Up[0], _Up[1], _Up[2]);
	 ALfloat Pos[6] = { _At[0], _At[1], _At[2], _Up[0], _Up[1], _Up[2]};
	 memcpy(tmpListenerPos2,Pos,sizeof(Pos));
	 RequestToSetListenerOri(Pos);
}

void _SetScale(bVector3f &SC)
{
	WorldScale=SC;
	IsGlobalListenerChanged=true;
	Touch();
};

void _SetGainOA(const float Gain)
{
     IsGlobalGainChanged=true;
	 OverallGain=Gain;
	 Touch();
}

void _SetGain(const float Gain)
{
     IsGlobalGainChanged=true;
	 AllGain=Gain;
	 Touch();
}


float GainPackByTypes[0xFF];

float     GetGainFor(const int Type)
{
	if(Type<0 || Type>0xFF) return 0;
	else
	return GainPackByTypes[Type];
}

void _SetGain(int SID,float Gain)
{
  if(SID<0 || SID>0xFF) return;
  GainPackByTypes[SID]=Gain;
  if(SID==0)
  {
	 IsGlobalGainChanged=true;
	 OverallGain=Gain;
  }
  else
  if(SID==1)
  {
	 IsGlobalGainChanged=true;
	 AllGain=Gain;
  }
  Touch();

}

void SetGainForAll()
{
	for(int i=0;i<0xFF;++i) GainPackByTypes[i]=1.0f;
}

struct THSstruct
{
	DWORD TimeStart,TimeToDo,TimeEnd;
	CGainPack Gains;
	bool  Valid;
};


THSstruct THGs[0xFF];
int NumTHG=0;

void _ClearTHG()
{
	SLOCK
	NumTHG=0;
	Touch();
};

void _SetTHG(DWORD TimeToDo,CGainPack * Gains)
{
	SLOCK
	THGs[NumTHG].TimeStart=Time::Get();
	THGs[NumTHG].TimeToDo=TimeToDo;
	THGs[NumTHG].TimeEnd =THGs[NumTHG].TimeStart+TimeToDo;
    THGs[NumTHG].Gains   =*Gains;	
	THGs[NumTHG].Valid   =true;
    NumTHG++; 
	Touch();
}




//SOUND SYS

SoundSys::SoundSys(void):BaseObject(REFC_CORE,REFB_MODULE,"SOUND")
{
	 MySoundSys=this;
//	 SetName("Sound[OpenAL]");
}

SoundSys::~SoundSys(void)
{
}


void CRTHT(void *P)
{((SoundSys*)P)->ThreadFunc();}

void StartThread(SoundSys *trg)
{
  SoundThread=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)CRTHT,trg,0,0);
}

bool  SoundSys::Start()
{
	if(!SYS_SOUND)
     RegisterNewConsoleClass(SYS_SOUND,"SOUND",-1);
	Print2Console(SYS_SOUND,"OpenAL sound system ready");
	Sound::Set::SetCreateCallBack     (_CreateSound);
	Sound::Set::SetPlayCallBack       (_PlaySound);
	Sound::Set::SetGainCallBack       (_GainSound);
	Sound::Set::SetStopCallBack       (_StopSound);
	Sound::Set::SetKillCallBack       (_KillSound);
	Sound::Set::SetPosCallBack        (_SetPosSound);
	Sound::Set::SetDupCallBack        (_DupSound);

	Sound::Set::SetListenPosCallBack  (_SetListenerPos);
    Sound::Set::SetListenOriCallBack  (_SetListenerOri);
	Sound::Set::SetOverallGainCallBack(_SetGainOA);
	Sound::Set::SetWorldScaleCallBack (_SetScale);
    Sound::Set::SetClearTHCallBack    (_ClearTHG);
    Sound::Set::SetTHGainCallBack     (_SetTHG);
	Sound::Set::SetGainForCallBack    (_SetGain);


	SetGainForAll();
	WorldScale.Set(0.01f,0.01f,0.01f);
	Stoped=1;
	SoundEvent=CreateEvent(0,FALSE,TRUE,NULL);
	SoundAskE =CreateEvent(0,FALSE,TRUE,NULL);
	ResetEvent(SoundAskE);
	StartThread(this);
	int ret=WaitForSingleObject( SoundAskE,10000);
	ResetEvent(SoundAskE);
	return true;
	//return suss;
}

void  SoundSys::SetPlaySlotsNum(int num)
{
	PlaySlots.SetSize(num);
    for(int i=0;i<num;i++)
	{
		PlaySlots[i]=new CSoundPlayChanel();
		PlaySlots[i]->CreateSource();
	}
}

void  SoundSys::Stop ()
{
	ResetEvent(SoundAskE);
	Stoped=1;
	Touch();
	WaitForSingleObject( SoundAskE,3000);
}

DWORD  SoundSys::Tick (float Time)
{
	//do not anything
    return 0;
}

void  SoundSys::ThreadFunc()
{
	//SleepEx(100,false);
	Stoped=0;
	if(SoundThread!=GetCurrentThread())
		Print2Console(SYS_SOUND,"Thread id not equal: %X : %X",SoundThread,GetCurrentThread());
	bool suss= InitializeOpenAL();
	if(suss)
	{
		SetPlaySlotsNum(32);
	}
	else
	{
		Stoped=1;
	}
	alListenerf(AL_GAIN    ,    1.0f);
    bool FS=true;
	while(!Stoped)
	{
		if(FS)
		{
			SetEvent(SoundAskE);
			FS=0;
		}
		//SleepEx(10,true);

        FetchALError();
		int ret=WaitForSingleObject(SoundEvent,200);//
		ResetEvent(SoundEvent);
		{
				
				try
				{
					if(IsGlobalGainChanged)
			         alListenerf(AL_GAIN    ,    OverallGain*AllGain);
		            IsGlobalGainChanged=false;
					{
                        SLOCK;
					try
					{
					UnquerySound();
					}
                    catch(...){Print2Console(SYS_SOUND,"sound query error");}
					}
					
				    if(NumTHG)
					{
						int NumValid=0;
						DWORD ThisTime=Time::Get();
						for(int i=0;i<NumTHG;++i)
							if(THGs[i].Valid)
							{
								if(THGs[i].TimeEnd<ThisTime)THGs[i].Valid=false;
								else
								{
								  NumValid++;
								  CGainPack* GP=&THGs[i].Gains;
                                  //SND_GAIN_OVERALL
								  if(THGs[i].TimeToDo==0)THGs[i].TimeToDo=1;
								  if(GP->IsValid(SND_GAIN_OVERALL))
								  {
									  _SetGainOA(GP->GetGain(SND_GAIN_OVERALL)*(
                                       ((float)(ThisTime-THGs[i].TimeStart))/((float)THGs[i].TimeToDo)));
								  }
								  if(GP->IsValid(SND_GAIN_ALL))
								  {
									  _SetGain(GP->GetGain(SND_GAIN_ALL)*(
                                       ((float)(ThisTime-THGs[i].TimeStart))/((float)THGs[i].TimeToDo)));
								  }
								  //else
								  {
								    int GID=-1;
									while((GID=GP->GetGainID(GID+1))!=-1)
									{
									 _SetGain(GID,GP->GetGain(GID)*(
                                       ((float)(ThisTime-THGs[i].TimeStart))/((float)THGs[i].TimeToDo)));
									 RecalcSoundGainFor(GID);
									}
								  }
								}
							}
						if(NumValid==0)NumTHG=0;
					}

					if(	!Onces.size() && !Dymanic.size()) continue;

      //update ones
				{
	               SoundA::iterator end3=Onces.end();
                   for(SoundA::iterator it2=Onces.begin();it2!=end3;++it2)
		              if(*it2)
					  {
						  if(!(*it2)->IsPlaying())
						  {
							  (*it2)->Stop();
							  delete *it2;
							  *it2=NULL;
						  }
					  }
				}

	  //update chanels
				_SPLAYPACK::iterator it;
                _SPLAYPACK::iterator end=PlaySlots.end();
				DWORD NumAwait=32;
				int nChanel=0;
                int ChanelsUsed=0;
	            for(it=PlaySlots.begin();it!=end;++it,++nChanel)
				{
					CSoundPlayChanel *ThisPlay=*it;
                  if(!(*it)->IsFree())
				  {
			        (*it)->Tick();
                    ChanelsUsed++;
				  }
		          else
				  if(NumAwait)
				  {
					  NumAwait=0;
					  //bool Haved=false;
					  {
	                    SoundA::iterator end=Dymanic.end();
                        for(SoundA::iterator it=Dymanic.begin();it!=end;++it)
			            if(*it)if((*it)->NeedPlayButUncapable())
						{
							(*it)->PlayOn(ThisPlay);
							NumAwait++;
							break;
						}
					  }
					  if(!NumAwait)
					  {
	                    SoundA::iterator end3=Onces.end();
                        for(SoundA::iterator it=Onces.begin();it!=end3;++it)
		                 if(*it)if((*it)->NeedPlayButUncapable())
						 {
							 (*it)->PlayOn(ThisPlay);
							 NumAwait++;
					     }
					  }

				  } 
				}
				IsGlobalListenerChanged=false;
                SoundChannelsUsed=ChanelsUsed;

				}
				catch(...)
				{
					Print2Console(SYS_SOUND,"sound proceed error");
				}
		}
	}
	DestroyOpenAL();
	SetEvent(SoundAskE);
	CloseHandle(SoundEvent);
	Stoped=2;
}

void SoundSys::RecalcSoundGainFor(const DWORD GP)
{
	_SPLAYPACK::iterator it;
    _SPLAYPACK::iterator end=PlaySlots.end();
    for(it=PlaySlots.begin();it!=end;++it)
    {
		CSoundPlayChanel *ThisPlay=*it;
        if(!(*it)->IsFree())
		{
			(*it)->TouchGain(GP);
		}
	}
}

CSound MyDummy;

PSound SoundSys::Load(const GString Name)
{
	PSound S=new CSound();
	if(!S->Load(Name))
	{
		Error2Console(SYS_SOUND,"Fail to load %s",Name.c_str());
		delete S;
		return &MyDummy;
	}
	//if(S->ContainStream())
	Dymanic.push_back(S);
	//else
	//Static.push_back(S);
	return S;
}

bVector3f OldlPos;

void   SoundSys::SetMyPosition   (bVector3f Pos)
{
    bVector3f TPos=Pos;
    Pos*=SOUND_WORLD_SCALE;
    if((OldlPos-TPos).Length()>1) Print2Console(SYS_SOUND,"Sound listener set to %f,%f,%f",Pos[0],Pos[1],Pos[2]);

	alListenerfv(AL_POSITION,(ALfloat*)&Pos);
}

void   SoundSys::SetMyOrientation(bVector3f Or)
{
	alListenerfv(AL_ORIENTATION,(ALfloat*)&Or);
}

PSound SoundSys::Duplicate(PSound Src)
{
	if(!Src)return false;

	PSound RS=new CSound();
	if(RS->DuplicateFrom(Src))
		return RS;
    delete RS;
	return NULL;
}

void   SoundSys::CheckDeath(PSound Sound)
{
	bool F=false;
	SoundA::iterator end=Dymanic.end();
	for(SoundA::iterator it=Dymanic.begin();it!=end;++it)
		if((*it)==Sound){ F=true;*it=NULL;break;}
	if(!F)
	{
    	SoundA::iterator end2=Static.end();
	    for(SoundA::iterator it=Static.begin();it!=end2;++it)
		 if((*it)==Sound){ F=true;*it=NULL;break;}
	}
   	SoundA::iterator end3=Onces.end();
    for(SoundA::iterator it2=Onces.begin();it2!=end3;++it2)
	 if((*it2)==Sound){ F=true;*it2=NULL;break;}
}

void   SoundSys::AddOnes   (PSound One)
{
	Onces.push_back(One);
}

bool   SoundSys::GetPlaySlot(CSoundSource *Source)
{
    _SPLAYPACK::iterator it;
    _SPLAYPACK::iterator end=PlaySlots.end();
    float minimallevel=0.001f;
    int   minimalslot =-1;
    int i=0;
    for(it=PlaySlots.begin();it!=end;++it,++i)
    {
        if((*it)->IsFree())
        {
            (*it)->Use(Source);
            return true;
        }else if((*it)->PriLevel()<minimallevel){minimallevel=(*it)->PriLevel();minimalslot=i;}
    }
    if(Source->PriLevel()>minimallevel)
    {
      PlaySlots[minimalslot]->Use(Source);
      return true;
    }
    return false;
}

//------------------------------------------------------

// SOUND


CSound::CSound()
{
  State=0;
}
CSound::~CSound()
{
  Source.UnBind();
}

bool CSound::NeedPlayButUncapable()
{
  return Source.NeedPlayButUncapable();
}

void CSound::Play(DWORD Params)
{
	if(Source.Empty())return;
    if(Params)
	{
		if(Params & SND_PLAY_ONCE)
		 MySoundSys->AddOnes(this);
	}
	if(Source.HaveDriver() || MySoundSys->GetPlaySlot(&Source))
	 Source.Play();
	else
	{
		Source.DoAlterLoop();
	}
}

void CSound::PlayOn(CSoundPlayChanel *Src)
{
     Src->Use(&Source);
	 Source.Play();
}
bool CSound::DuplicateFrom(CSound *Src){ return Source.DuplicateFrom(&Src->Source);}
void CSound::Stop(){Source.Stop();}
void CSound::SetPosition(bVector3f Pos){Source.Move(Pos);}
BYTE CSound::GetState(){return State;}
bool CSound::Load(const GString &Name)
{
	Source.SetName(Name);
    CSoundSourceShader *Shader=NULL;
    bool found=GetSoundShaderManager()->GetShader(Name,Shader);
    bool ret=false;
    if(found && Shader)
     ret= Source.Assign(Shader);
    if(ret) return true;
    else
	return Source.AddBuffer(GetBufferFor(Name,0));
}



//bool CSound::IsStreamed(){return false;}//MySound->IsStreamed();}
void CSound::Update(){Source.Tick();}

//bool CSound::DuplicateTo(PSound Dest)
//{
//	if(!((remSnd*)Dest->_MySound)->CreateSourse(MySound->GetSoundShader()))		return false;
	//((remSnd*)Dest->_MySound)->AssingBuffer(MySound->GetBufferID());
	//return true;
//}

bool CSound::IsPlaying()   {return Source.IsPlaying();}
void CSound::SetGain  (float Gain)   {Source.SetGain(Gain);}

//PSoundSys MySoundSys;

PBlopBaseObject GetSoundSystem()
{
	return MySoundSys;
}

PBlopBaseObject GetSound()
{
	MySoundSys=new SoundSys();
	return MySoundSys;
}

PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN
    MAKE_PLUGIN (GetSound,"SOUND",0,true);
    END_PLUGIN
}


                                                



//9035774305  Марина








//SYSTEM



