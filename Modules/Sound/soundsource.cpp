#include "StdAfx.h"
#include "soundsource.h"
#include "soundbuffer.h"
#include "al.h"
#include "alc.h"
#include "alu.h"
#include "alut.h"
#include "alinit.h"

extern bool IsGlobalListenerChanged;
extern bVector3f ListenerPos;
extern bVector3f GetWorldScale();
extern float     GetGainFor(const int Type);
extern bool IsGlobalListenerChanged;
extern bool IsTypeGainChanged;

CSoundSource::CSoundSource(void)
{
	CurrentBuffer=0;
	IsMusic      =0;
	IsGainChanged=true;
	IsPositionChanged=true;
	CurrentGain  =1;
	fIsLooped    =1;// _!_
	CurPlayStatus=0;
	Driver       =NULL;
	DistanceModel=0;//DISTANCE_MODEL_USERFALLOFF;
	GAIN_DISTANCE=200;
	Queued       =-1;
	GainPack     =SND_GAIN_MUSIC;
	OldGain      =-1;
	GainFactor   =1;
    AfterLoop    =0;
}

bool CSoundSource::DuplicateFrom(CSoundSource *Src)
{
	*this=*Src;
    BufferProccesing.DropData();
	BufferProccesing.CopyFrom(*Src->GetBufferProccesing());
    CurrentGain  =1;
	Driver       =NULL;
	Queued       =-1;
	OldGain      =-1;
	IsGainChanged=true;
	IsPositionChanged=true;
    return true;
}

bool  CSoundSource::Assign(CSoundSourceShader *Shader)
{
    if(!Shader) return false;
    fIsLooped=Shader->Looped;
    DistanceModel=Shader->DistModel;
    GAIN_DISTANCE=Shader->DistMax;
    GainPack     =Shader->GainPack;
    GainFactor   =Shader->Gain;
    AfterLoop    =Shader->AfterLooped;

    if(Shader->NumBuffers)
    {
        for(DWORD i=0;i<Shader->NumBuffers;++i)
            AddBuffer(GetBufferFor(Shader->Buffers[i].Name,Shader->Buffers[i].Streamed?SND_STREAM:0));
        return true;
    }
    else return false;

}

CSoundSource::~CSoundSource(void)
{
	FreeMe();
}

bool CSoundSource::AddBuffer(DWORD BufferID)
{
	if(IsMusic  ) return false;
	if(!BufferID)return false;
	IncBufferUsage(BufferID);
	BufferProccesing.push_back(BufferID);
	const BufferInfo& info= GetInfoForBuffer(BufferID);
	if(info.IsStreamed())
	{
      IsMusic=true;
	}
	return true;
}

float CSoundSource::PriLevel()
{
	return GetFinalGain();
}

float CSoundSource::GetFinalGain()
{
	if(fFinalGain==0 || (IsGainChanged || IsPositionChanged || IsGlobalListenerChanged || IsTypeGainChanged))
     RecalcGain();
	return fFinalGain;
}

void CSoundSource::TouchGain(const DWORD GP)
{
	if(GP==GainPack)
	{
	  IsTypeGainChanged=true;
	  if(RecalcGain())
	  {
	  }
	}
}

bool CSoundSource::RecalcGain()
{
	//not now
	if(!IsGainChanged && !IsPositionChanged && !IsGlobalListenerChanged && !IsTypeGainChanged) return fFinalGain>0;
	fFinalGain=0;
    IsPositionChanged=0;
    IsGainChanged    =0;
	float NormalGain=CurrentGain*GainFactor*GetGainFor(GainPack);
	float tmpFinalGain=1;
	if(NormalGain==0) 
	{
		IsGainChanged    =0;
		IsPositionChanged=0;
		fFinalGain       =0;
		GainStop();
		return false;
	}
	tmpFinalGain=NormalGain;
	if(DistanceModel)
	{
	  bVector3f vDist=(ListenerPos-Position);
	  bVector3f WorldScale=GetWorldScale();
	  /*vDist[0]*=WorldScale[0];
	  vDist[1]*=WorldScale[1];
	  vDist[2]*=WorldScale[2];*/
	  float Dist=vDist.Length();
	  float MD=GetGainDistance();
	  if(Dist>MD) 
	  {GainStop();// to far
	   tmpFinalGain=0;
	  }
	  else
      switch(DistanceModel)
	  {
	  case DISTANCE_MODEL_USERFALLOFF:
        Dist=MIN(Dist,MD);
 		Dist/=MD;
	    tmpFinalGain*=(1-Dist);
		break;
	  case DISTANCE_MODEL_USERFALLOFFQ:
        Dist=MIN(Dist,MD);
 		Dist/=MD;
	    tmpFinalGain*=(1-Dist*Dist);
		break;
	  case DISTANCE_MODEL_USERFALLOFFBQ:
        Dist=MIN(Dist,MD);
 		Dist/=MD;
	    tmpFinalGain*=(1-Dist)*(1-Dist);
		break;
	  case DISTANCE_MODEL_USERFALLOFFBSQR:
        Dist=MIN(Dist,MD);
 		Dist/=MD;
	    tmpFinalGain*=(1-sqrtf(Dist));
		break;
	  }
	}
	fFinalGain=tmpFinalGain;
	return tmpFinalGain>0;
	
}

bool CSoundSource::Tick()
{
	if(Queued!=-1)
	if(!RecalcGain()) return false;
	if(OldGain!=fFinalGain)if(Driver)Driver->SetGain(fFinalGain);
    if(Driver)Driver->SetMaxGain(GainFactor);
	OldGain=fFinalGain;
	if (!BufferProccesing.size()) return false; 	
	int				Processed = 0,TP;
	int             BQueued   = 0;
	ALuint			BufID;
	FetchALError();
	alGetSourcei(SourceID, AL_BUFFERS_PROCESSED, &Processed);
	alGetSourcei(SourceID, AL_BUFFERS_QUEUED   , &BQueued);
	TP=Processed;
	CheckALError();
	
	CurrentBuffer=Processed;
	if(!IsMusic)
	{
	  if(Queued<Processed)
      {
	  for(;Processed<BufferProccesing.size();Processed++)
	  {
		 BufID=GetALBufferID(BufferProccesing[Processed]);
		 Queued++;
		 if(BufID)
 		  alSourceQueueBuffers(SourceID,1,&BufID);
	  }
        if(AfterLoop){alSourceQueueBuffers(SourceID,1,&BufID);alSourceQueueBuffers(SourceID,1,&BufID);};
      }
      if(AfterLoop && (TP>=BufferProccesing.size()-1 || !BQueued))
	  {
		  if(!BQueued)
		  {
			  BufID=GetALBufferID(BufferProccesing[BufferProccesing.size()-1]);
     	 	  if(BufID)
 		        alSourceQueueBuffers(SourceID,1,&BufID);
		  }
          SetLooped(true);
	  }
	}
	else
	{
	 bool FP=false;
	 if(Queued==-1)	// first
	 {
		 FP=true;
		 CheckALError();
		 bool checks=IsPlaying();
		 BufID=GetALBufferID(BufferProccesing[0]); Queued++;
		 if(BufID) alSourceQueueBuffers(SourceID,1,&BufID);
		 CheckALError();
		 BufID=GetALBuffer2ID(BufferProccesing[0]); Queued++;
		 if(BufID) alSourceQueueBuffers(SourceID,1,&BufID);
		 CheckALError();
		 checks=IsPlaying();
	 }
	 else
     while (Processed--)//Processed--)
	 {
		bool checks=IsPlaying();
		int ret=alUseMusicBuffer(SourceID,BufferProccesing[0]);
		checks=IsPlaying();
		if(ret==-1)break;
		if(Queued<0)Queued=0;
		Queued++;
		if(Queued>65000)Queued=1;
		if(!ret) Stop();
		if(ret==1 && !IsLooped()) Stop();
	 }
	 /*if(!FP)
	 {
	   bool checks=IsPlaying();
	   if(!checks)
	   {Play();
		Print2Console(SYS_SOUND,"sound not playing!!");
	   }
	 }   */
	}
	return CheckALError()==AL_TRUE;
}

bool CSoundSource::NeedPlayButUncapable()
{
	bool free= !Driver && CurPlayStatus>0;
	if(free)
		return RecalcGain();
	else return false;
}

void CSoundSource::SetSource(DWORD Source,CSoundPlayChanel *_Driver)
{
   if(Driver && !_Driver)// parent stops us
   {
	   if(IsPlaying())GainStop();
	   //else if(CurPlayStatus==1)
	   if(CurPlayStatus)
	   CurPlayStatus=2;
       //Position.Set(-10000,-10000,-10000);
   }
   SourceID=Source;
   Driver  =_Driver;
   if(Driver && SourceID) 
    Tick();
}

void CSoundSource::FreeMe()
{
	UnBind();
	//free processing buffers
    if (!BufferProccesing.size())
		for(int i=0;i<BufferProccesing.size();++i)
			DecBufferUsage((BufferProccesing[i]));

}
void CSoundSource::UnBind()
{
	if(Driver)
		Driver->FreeMe();
}

void CSoundSource::SetGain(float in_Gain)
{
	CurrentGain=in_Gain;
	IsGainChanged=true;
	if(Driver)
	{
	   float Gain=GetFinalGain();
	   if(Gain && Driver)
	   Driver->SetGain  (Gain);
	}
}

void CSoundSource::Play()
{
	if(Driver)
	{
		if(CurPlayStatus==2)
		{
			//Print2Console(SYS_SOUND,"%X reactivated",this);
			Queued--;
		//	Tick();
		}
		float Gain=GetFinalGain();
		if(Gain>0)
		{
		 Tick();
		 Driver->SetGain  (Gain);
		 SetLooped(IsLooped());
		 Move     (Position);
		 Driver->Play();
		 bool checks=IsPlaying();
		//if(CurPlayStatus==0)
         CurPlayStatus=1;  
		}
	}
}
void CSoundSource::Stop()
{
	if(Driver) Driver->Stop();
	//if(CurPlayStatus==1)
	CurPlayStatus=0;
}

void CSoundSource::GainStop()
{
	CurPlayStatus=2;
    fFinalGain=0;
	if(Driver) Driver->Stop();
}

void CSoundSource::Move(const bVector3f &Pos)
{
	//if((Position-Pos).Length()<0.0001f) return;
	Position=Pos;
	IsPositionChanged=true;
	//there is no possible moving for music
	if(IsMusic)return;
	if(Driver)
	  Driver->SetPosition(Position);
}

void CSoundSource::SetLooped(const bool Looped)
{
	fIsLooped=Looped;
	if(Driver && !IsMusic)
		Driver->SetLooped(IsLooped());
}

bool CSoundSource::IsPlaying()
{
	if(Driver)return Driver->IsPlaying();
	else return false;
}

CSoundPlayChanel::CSoundPlayChanel()
{
   m_SourceID=0;
   Source    =0;
}

bool CSoundPlayChanel::CreateSource()
{
	m_SourceID=0;
    Source    =0;
    ALboolean WasError=CheckALError();
	ALuint ms;
	alGenSources(1, &ms);
	m_SourceID=ms;
	if (!CheckALError() && WasError)
		return false;
	alSourcef (m_SourceID, AL_PITCH   ,1.0f    );
	alSourcef (m_SourceID, AL_GAIN    ,1.0f    );
    alSourcef (m_SourceID, AL_MIN_GAIN,0.0f    );
	alSourcei (m_SourceID, AL_LOOPING ,	AL_FALSE);
	StopChecked=2;
	//Stop();
	return false;
}

void CSoundPlayChanel::DeleteSource()
{
	ALuint ms=m_SourceID;
    alDeleteSources( 1, &ms);
	m_SourceID=0;
}

void CSoundPlayChanel::Use(  CSoundSource *_Source)
{
	if(_Source!=Source)
    if(Source)
	{
	 Source->SetSource(0,0);
	 Source=NULL;
	 
	 Stop();
     if(Source)
         DeleteSource();
	}

	Source=_Source;
    if(Source)
	{   if(!m_SourceID)CreateSource();
	    Source=_Source;
		Source->SetSource(m_SourceID,this);
	}else
	DeleteSource();
}

void CSoundPlayChanel::SetMaxGain(float Gain)
{
    alSourcef (m_SourceID, AL_MAX_GAIN   ,Gain  );

}

void CSoundPlayChanel::SetPosition(const bVector3f &_Pos)
{
	/*bVector3f SC=GetWorldScale();
	SC[0]=SC[0]*_Pos[0];
	SC[1]=SC[1]*_Pos[1];
	SC[2]=SC[2]*_Pos[2];*/
#define SC _Pos
	ALfloat Pos[3] = { SC[0], SC[1], SC[2] };
    //Print2Console(SYS_SOUND,"Sound %x at %f,%f,%f",this,Pos[0],Pos[1],Pos[2]);
#undef SC
	alSourcefv(m_SourceID, AL_POSITION, Pos);
	CheckALError();
}

void CSoundPlayChanel::Tick()
{
	if(Source)
		if(!Source->Tick())
			FreeMe();
}

bool CSoundPlayChanel::IsPlaying()
{
   int i;
   if(m_SourceID==0)return false;
   alGetSourcei(m_SourceID,AL_SOURCE_STATE,&i);
   return i==AL_PLAYING;//AL_STOPPED;
}

void CSoundPlayChanel::SetGain(float Gain)
{
	if(!this) return;
	alSourcef (m_SourceID, AL_GAIN ,Gain);
}

void CSoundPlayChanel::SetLooped(const bool Looped)
{
	alSourcei (m_SourceID, AL_LOOPING ,	Looped?AL_TRUE:AL_FALSE);
	//Print2Console(SYS_SOUND,"%X loop %d",(DWORD)Looped);
}

void CSoundPlayChanel::Pause()
{
    alSourcePause(m_SourceID);
}
void CSoundPlayChanel::Stop ()
{
	//Print2Console(SYS_SOUND,"%X stoped",(DWORD)this);
	alSourceStop(m_SourceID);
	int             BQueued   = 0;
	/*alGetSourcei(m_SourceID, AL_BUFFERS_QUEUED   , &BQueued);
	if(BQueued>0)
	{
		ALuint			BufOut;
		for(int i=0;i<BQueued;++i)
			alSourceUnqueueBuffers(m_SourceID, 1, &BufOut);
	}
    */
	Use(NULL);
}

void CSoundPlayChanel::Play ()
{
	if(!this)return;
	int BQueued=0;
	alGetSourcei(m_SourceID, AL_BUFFERS_QUEUED   , &BQueued);
    //ASSERT(BQueued);
	if(!BQueued)return;
	//Print2Console(SYS_SOUND,"%X started",(DWORD)this);
    alSourcePlay(m_SourceID);
	StopChecked=0;
}

void CSoundPlayChanel::FreeMe()
{
    Stop();
    Use(NULL);
}

void CSoundPlayChanel::TouchGain(const DWORD GP)
{
	if(Source)
		Source->TouchGain(GP);
}

bool  CSoundPlayChanel::IsFree()
{
	if(!m_SourceID)return true;
	if(!Source)
	{
		if(StopChecked==0)
		{
			StopChecked=IsPlaying()?1:2;
			if(StopChecked==1)Stop();
		}
		return true;
	}
	return !IsPlaying();
}