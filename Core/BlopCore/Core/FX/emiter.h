#pragma once

#include "ParticleDef.h"

#define PAREM_CENTER                 1
#define PAREM_DELTACENTER            15 
#define PAREM_SPEED                  2
#define PAREM_DELTASPEED             14 
#define PAREM_OFFSET                 3
#define PAREM_MASS                   4
#define PAREM_DELTAMASS              11
#define PAREM_2RAD                   5
#define PAREM_RAD                    6
#define PAREM_RAD1                   7
#define PAREM_RAD2                   8
#define PAREM_SIZE                   9
#define PAREM_DELTASIZE              10
#define PAREM_LIFE                   13
#define PAREM_DELTALIFE              12

#define PAREM_SPEEDFACTOR            14
#define PAREM_DELTASPEEDFACTOR       15

#define PAREM_SIZECHANGE             16
#define PAREM_DELTASIZECHANGE        23

#define PAREM_ROTATION               17
#define PAREM_DELTAROTATION          18

#define PAREM_COLORCHANGE            19
#define PAREM_COLORSTAGE             20

#define PAREM_ROTATIONSP             21
#define PAREM_DELTAROTATIONSP        22//23

class DLLTYPE CParticleThinkerBase
{
public:
	virtual bool Rotater(){return false;}
	virtual bool Mover  (){return false;}
	virtual int Emite(CParticle *Particles,WORD Num){return 0;}
	virtual void Move(CParticle *Particles,WORD Num,float FPS){}
	virtual bool SetParamf      (const DWORD ParamID,float Val){return false;}
    virtual bool SetParamVector (const DWORD ParamID,const float* Val){return false;}
    virtual bool SetParam2Vector(const DWORD ParamID,const float* Val1,const float* Val2){return false;}
	virtual void Kill(CParticle &Particles){};
	virtual bool TryKill(CParticle &Particles){return true;};
};


class DLLTYPE CParticleEmiter:public CParticleThinkerBase
{
protected: 
    float Size,Mass;
    float DeltaSize,DeltaMass;
	float Life,DeltaLife;
	float SpeedFactor,DeltaSpeedFactor;
	float RotationSpeed,DeltaRotationSpeed;
public:
	void SetSize(float _Size){Size=_Size;}
	void SetMass(float _Mass){Mass=_Mass;}
	float GetLife(){return Life;}
	float GetDeltaLife(){return DeltaLife;}
	float GetSpeedFactor();
	CParticleEmiter(void);
	~CParticleEmiter(void);

    virtual bool SetParamf      (const DWORD ParamID,float Val);
    virtual bool SetParamVector (const DWORD ParamID,const float* Val){return false;}
    virtual bool SetParam2Vector(const DWORD ParamID,const float* Val1,const float* Val2){return false;}
	virtual CParticleEmiter *Duplicate(){return new CParticleEmiter;}
};

class DLLTYPE CPointSpeedEmiter : public CParticleEmiter
{
protected: 
	bVector3f    Center,Speed,DeltaCenter,DeltaSpeed;
public:
    bool SetParamVector (const DWORD ParamID,const float* Val);
	int  Emite(CParticle *Particles,WORD Num);
	virtual CParticleEmiter *Duplicate(){return new CPointSpeedEmiter;}
};

class DLLTYPE CPointParticleEmiter : public CPointSpeedEmiter
{public:int Emite(CParticle *Particles,WORD Num);
virtual CParticleEmiter *Duplicate(){return new CPointParticleEmiter;}};

class DLLTYPE  CPlaneParticleEmiter : public CParticleEmiter
{public:int Emite(CParticle *Particles,WORD Num){return 0;};
virtual CParticleEmiter *Duplicate(){return new CPlaneParticleEmiter;}};

class DLLTYPE CSphereParticleEmiter : public CParticleEmiter
{
protected:
    bVector3f    Center,DeltaCenter;
	float        RadX,
		         RadY,
				 RadZ,
				 RadXm,
		         RadYm,
				 RadZm;
	bVector3f    Speed,DeltaSpeed;
public:
    bool SetParamVector (const DWORD ParamID,const float* Val);
    bool SetParam2Vector(const DWORD ParamID,const float* Val1,const float* Val2);
	int  Emite(CParticle *Particles,WORD Num);
    virtual CParticleEmiter *Duplicate(){return new CSphereParticleEmiter;}
};

class DLLTYPE CSphereSpeedParticleEmiter : public CSphereParticleEmiter
{public:int Emite(CParticle *Particles,WORD Num);
virtual CParticleEmiter *Duplicate(){return new CSphereSpeedParticleEmiter;}};

class CDefRotated:public CParticleEmiter
{
public:
	bool Rotater(){return true;}
	bool Mover  (){return false;}
	void Move    (CParticle *Particles,WORD Num,float FPS);
    virtual CParticleEmiter *Duplicate(){return new CDefRotated;}
};

class CCloudRotater:public CParticleEmiter
{
protected:
	float SizeChange;
	bVector3f ColorChange;
	bool ColorChanging;
public:
	CCloudRotater();
	bool Rotater(){return true;}
	bool Mover  (){return true;}

    bool SetParamf      (const DWORD ParamID,float Val);
    bool SetParamVector(const DWORD ParamID,const float* Val1);
	void Move    (CParticle *Particles,WORD Num,float FPS);
	virtual CParticleEmiter *Duplicate(){return new CCloudRotater;}
};

class CGravitator:public CParticleEmiter
{
protected:
    bVector3f ColorChange;
	bool ColorChanging;
public:
	bool Rotater(){return false;}
	bool Mover  (){return true;}
    bool SetParamVector(const DWORD ParamID,const float* Val1);
	void Move    (CParticle *Particles,WORD Num,float FPS);
	virtual CParticleEmiter *Duplicate(){return new CGravitator;}
};

class CCloudRotaterMin:public CParticleEmiter
{
public:
	bool Rotater(){return false;}
	bool Mover  (){return true;}
	void Move    (CParticle *Particles,WORD Num,float FPS);
	virtual CParticleEmiter *Duplicate(){return new CCloudRotaterMin;}
};


