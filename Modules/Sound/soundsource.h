#ifndef _SOUND_SOURSE_H
#define _SOUND_SOURSE_H

class CSoundPlayChanel;

class CSoundSource
{
protected:
	DWORD              Flags;
	typedef SmartVector<DWORD> BufferPack;
	BufferPack         BufferProccesing;
	bVector3f		   Position;
	bVector3f          Speed;
	int                CurrentBuffer;
	DWORD              SourceID;
    CSoundPlayChanel  *Driver;
	DWORD              IsMusic;
	DWORD              GainPack;
	float              CurrentGain;
	float              GainFactor;
	bool               fIsLooped;
	float              fFinalGain;
	bool               IsGainChanged;
	bool               IsPositionChanged;
    bool               RecalcGain();
	int               CurPlayStatus;
	DWORD              DistanceModel;
	float              GAIN_DISTANCE;
	int                Queued;
	float              OldGain;
    bool               AfterLoop;
	GString            Name;
public:
	float GetGainDistance(){return GAIN_DISTANCE;}
	float SetGainDistance(float GD){GAIN_DISTANCE=GD;}
	float GetFinalGain();
	bool IsLooped()const {return fIsLooped;}
	CSoundSource(void);
	~CSoundSource(void);
	bool AddBuffer(DWORD BufferID);
	bool Tick();
	void SetSource(DWORD Source,CSoundPlayChanel *_Driver);

	void SetLooped(const bool Looped);
	void Play();
	void Stop();
	void GainStop();
	void Move(const bVector3f &Pos);
	void SetGain(float Gain);
	bool IsPlaying();
    float PriLevel();
	void FreeMe();
	void UnBind();
	bool NeedPlayButUncapable();
	bool DuplicateFrom(CSoundSource *Src);
	BufferPack *GetBufferProccesing(){return &BufferProccesing;}
    bool  HaveDriver(){return Driver!=NULL;}
    bool  Assign(CSoundSourceShader *Shader);
	void DoAlterLoop(){if(AfterLoop)Queued=15;CurPlayStatus=2;}
	void SetName(LPCSTR N){Name=N;}
	bool Empty(){return BufferProccesing.size()==0;}

	void TouchGain(const DWORD GP);
};

class CSoundPlayChanel
{
	DWORD m_SourceID;
protected:
	CSoundSource * Source;
	int            StopChecked;
public:
	CSoundPlayChanel();
	bool CreateSource();
	void DeleteSource();
	void Use(  CSoundSource *Source);
	void Tick();
	void TouchGain(const DWORD GP);
	void FreeMe();
	void UseMe ();
	bool IsPlaying();
	void SetGain(float Gain);
	void SetLooped(const bool Looped);
	void SetPosition(const bVector3f &Pos);

    void Play ();
    void SetMaxGain(float Gain);
	void Pause();
	void Stop ();
    float PriLevel(){return 0;}
    bool  IsFree();

};

#endif
