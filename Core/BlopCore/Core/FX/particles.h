#ifndef _BLOP_PARTICLES_H
#define _BLOP_PARTICLES_H
#pragma once

#include "ParticleDef.h"
#include "Emiter.h"
#include "ParticleDrawer.h"

//typedef void(*ParticleSpeedFunc)(CParticle &Particle);

typedef SmartVector<CParticle> ParticleList;


#define PD_CLEAR     0
#define PD_COLLAPCE  1
#define PD_BLENDZERO 2

#define PD_EMITE_UNDEF      0
#define PD_EMITE_USER       1
#define PD_EMITE_ONCE       2
#define PD_EMITE_FRAME      3
#define PD_EMITE_TIMER      4
#define PD_EMITE_CONST      5
#define PD_EMITE_TIMERFLOAT 6

#define PD_CENTER_NONE      0
#define PD_CENTER_TR		1
#define PD_CENTER_RT		2
#define PD_CENTER_SC		4
#define PD_CENTER_EMITER    5

struct CBaseParticleEmiteCharge
{
	DWORD ChargeTime;
	DWORD Num;
}; 

class DLLTYPE CBaseParticleSystem
{
public:
	typedef SmartVector<CBaseParticleSystem*> ParticlePack;
    ParticlePack Layers;
public:
	bool inBlock;
    void AddSystem   (CBaseParticleSystem* Sys);
	void RemoveSystem(CBaseParticleSystem* Sys);
	virtual void Tick (float TimeCompress);
	void Charge      ();
	virtual void CheckDead();
	virtual bool IsEnabled(){return true;}

	virtual void ChargeEmite(DWORD DeltaTime,int Num){};
	virtual void UnchargeEmite(){};
	virtual void Draw();
	virtual void SetLifeTime(WORD Time){};
	virtual CParticleEmiter* GetEmiter    (){return NULL;}
	virtual CParticleEmiter* GetRegister  (){return NULL;}
	virtual CParticleEmiter* GetController(){return NULL;}
	virtual CParticleEmiter* GetKiller    (){return NULL;}
	virtual CParticle * Emite(WORD Num) {return NULL;};
	virtual CParticle * Emite(){return NULL;}         ;
	virtual CParticle * GetByID(const DWORD ID){ASSERT(false  /*"abstract"*/);return NULL;};
	//virtual void SetOnceEmite(DWORD Num,bool Single,bool PerFrame){};

	virtual void SetDeathType(DWORD MaxDeathTime,DWORD DeathType){};
	virtual void Disable(){};
	virtual void Enable(){};

	virtual void SetTexture(DWORD CID,DWORD TID){}
	        void SetTexture(DWORD CID,LPCSTR TID);
	virtual void SetTAmins (DWORD X,DWORD Y,float speed){}

	virtual void SetSystemCenter     (const float *Pos)          {}
	virtual void SetSystemOrientation(const float* Rot)          {}
    virtual void SetSystemVelosity   (const float* Vel)          {}
	virtual void UseSystemCenter     (const int    Type)         {}
	virtual void SetCenterParticles  (ParticleList *Particles)   {}
	virtual void SetRotatable        (bool Rotatable)            {}
	virtual void SetBlendType        (DWORD BlendType)           {}
	virtual void SetColored          (bool Colored)              {}
	virtual void SetColors           (const float *C)            {};
    virtual bool IsFree              ()                          {return true;};
	virtual bool AssertShader        (DWORD ID)                  {return false;};
	virtual void Block               ()                          {inBlock=true;}
	virtual CBaseParticleSystem* Duplicate(DWORD Count)          {return NULL;}
	bool IsBlocked(){return inBlock;}
	virtual void Add2List(){};

	virtual bool IsVisibled(){return true;}
	
};

typedef int(*ExParticleControlerCallback)(const DWORD Tag,CParticle *Particle,const float TimeCompress);

typedef CBaseParticleSystem *PBaseParticleSystem;

#include  "Core/Sys/Cull/blopCull.h"

class DLLTYPE CBaseParticleSet:public CBaseParticleSystem
{
protected:
	CULL::CCullElement    *CullElement;
	DWORD                  DeltaTime;
	ParticleList           Particles;
	ParticleList           DeadParticles;
	ParticleList           CenterParticles;
	DWORD                  LastTimeCalled;
    int                    MaxParticles;
	int                    TimeOut;

	bVector3f              BoundingSize;
	bVector3f              BoundingCenter;
	bool                   Ticable;
	bool                   Enabled;
	bool                   Drawable;
	typedef SmartVector<CBaseParticleEmiteCharge> CEmiteCharge;
	CEmiteCharge           Charges;
	int                    TimeToDead;
	DWORD                  DeathType;
	DWORD                  LastTime;
	DWORD                  Texture;
	DWORD                  TAnimsX,TAminsY;
	float                  AnimationSpeed;
	bVector3f              SystemCenter;
	bVector3f              SystemRot;
	bVector3f              SystemVel;
	bool                   fRotatable;
	DWORD                  fBlendType;
	int                    SystemCenterUse;
	bool                   fColored;
	bVector4f              DefColor,DefColorParticle;
	float                  EmiteCharge;
	DWORD                  EmiteType;
	bool                   OnceEmited;
    float                  EmiteLasts;
	bool                   AdvanceEmite;
	ExParticleControlerCallback ExCallBack;
	DWORD                       ExCallBackTag;
	bVector3f              SystemSize;

	float                  occRad;
	bVector3f              occPosition;
public:
	bool IsVisibled();
	virtual void Add2List();
	void  AfterTick(const float TC);
    bool IsFree    ();
	CBaseParticleSet(DWORD _MaxParticles);
	~CBaseParticleSet();

    void SetTexture(DWORD CID,DWORD TID);
	void SetTAmins (DWORD X,DWORD Y,float speed);

	virtual void SetParticlesNum(DWORD NewCount);
	virtual void UpdateParticlePos(DWORD ID,float x,float y,float z,float size);
	virtual void SetTicable(bool Mode);
	virtual void SetLifeTime(WORD Time);
	virtual bool IsEnabled();
	virtual bool IsDrawable();
	virtual void Start(){};
	virtual void Stop (){};
	virtual void Tick (float TimeCompress);
	        void BeforeTick(float TimeCompress);
	virtual CParticle * Emite(WORD Num);
	virtual void CheckDead();
	CParticle * Emite();
	CParticle * GetByID(const DWORD ID);


	void ChargeEmite(DWORD DeltaTime,int Num);
	void UnchargeEmite();

	void SetDeathType(DWORD MaxDeathTime,DWORD DeathType);

	virtual CBaseParticleSystem* Duplicate(DWORD Count);
	virtual void Disable();
	virtual void Enable ();

	virtual void SetSystemCenter     (const float *Pos)       ;
	virtual void SetSystemOrientation(const float* Rot)       ;
    virtual void SetSystemVelosity   (const float* Vel)       ;
	virtual void UseSystemCenter     (const int    Type)      ;
	virtual void SetCenterParticles  (ParticleList *Particles);
	virtual void SetRotatable        (bool Rotatable)         ;
	virtual void SetBlendType        (DWORD BlendType)        ;

	virtual void SetColored          (bool Colored)           ;
	virtual void SetColors           (const float *C)         ;
	//virtual void SetOnceEmite        (DWORD Num,bool Single,bool PerFrame){OnceEmitenum=Num;OnceSingle=Single;OncePerFrame=PerFrame;};
	bool AssertShader        (DWORD ID);
	void KillParticle(CParticle &Particle,DWORD ID,bool KillAtOnce=false,bool DeadAllready=false);

private:
	//particle template
	CParticleEmiter*      MyEmiter;
	CparticleCPUDrawer*   MyDrawer;
	CParticleEmiter*      MyController;
	CParticleEmiter*      MyRegister;
	CParticleEmiter*      MyKiller;

public:
	CParticleEmiter* GetEmiter    (){return MyEmiter;}
	CParticleEmiter* GetRegister  (){return MyRegister;}
	CParticleEmiter* GetController(){return MyController;}
	CParticleEmiter* GetKiller    (){return MyKiller;}

	void SetEmiter    (CParticleEmiter    *Src){MyEmiter    =Src;}
	void SetRegister  (CParticleEmiter    *Src){MyRegister  =Src;}
	void SetController(CParticleEmiter    *Src){MyController=Src;}
	void SetKiller    (CParticleEmiter    *Src){MyKiller    =Src;}
	void SetDrawer    (CparticleCPUDrawer *Src){MyDrawer    =Src;}
	void Draw();
	//void KillParticle(CParticle &Particle,DWORD ID,bool KillAtOnce=false,bool DeadAllready=false);
};

extern PBaseParticleSystem ProjectParticles;







//PBaseParticleSystem DLLTYPE CreateParticleSystem(LPCSTR System);


namespace PARTICLES
{
void DLLTYPE DO();
void DLLTYPE Prepare(float Time=-1);
PBaseParticleSystem DLLTYPE LoadParticleSybset(LPCSTR Name,DWORD OverCount=0);

int DLLTYPE RegisterCallBack(LPCSTR Name,ExParticleControlerCallback CB);
int DLLTYPE RegisterCallBackParam(LPCSTR Name,float Param);

};

int DLLTYPE RegisterParticleSybset(CParticleEmiter* Set,LPCSTR SetName);


#endif