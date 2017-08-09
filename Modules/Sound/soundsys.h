#ifndef _SOUND_SYS_H
#define _SOUND_SYS_H

namespace Sound
{

class CSound
{
	BYTE State;
	CSoundSource Source;
public:
	BYTE GetState();

	CSound();
	~CSound();

	void Play(DWORD Params);
	void PlayOn(CSoundPlayChanel *Src);
	void Stop();
	void Pause();
	void SetGain(float Gain);

	void SetPosition(bVector3f Pos);
    void SetSpeed   (bVector3f Speed);


	bool Load(const GString &Name);

	bool IsPlaying();
	bool NeedPlayButUncapable();

	void Update();
    bool DuplicateFrom(CSound *Src);
};

typedef CSound* PSound;


typedef SmartVector<PSound> SoundA;

class SoundSys:public BaseObject
{
    SoundA  Dymanic;
	SoundA  Static;
	SoundA  Onces;
    typedef SmartVector<CSoundPlayChanel*> _SPLAYPACK;
    _SPLAYPACK   PlaySlots;
	void         SetPlaySlotsNum(int num);
	int          Stoped;

	void RecalcSoundGainFor(const DWORD GP);
public:
	bool  Start();
	void  Stop ();
	DWORD  Tick (float Time);
	SoundSys(void);
	~SoundSys(void);
	PSound Load(const GString Name);
	void   SetMyPosition   (bVector3f Pos);
	void   SetMyOrientation(bVector3f Or);

	CSound* Duplicate(CSound* Src);

	void   CheckDeath(PSound Sound);
	void   AddOnes   (PSound One);

	bool   GetPlaySlot(CSoundSource *Source);

	void ThreadFunc();
};

typedef SoundSys* PSoundSys;

};

#endif