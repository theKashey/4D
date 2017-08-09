#ifndef __PARTICLE_DRAWER_FX
#define __PARTICLE_DRAWER_FX

#include "Core\Graphics\OpenGL\Env\grBleanding.h"

class CparticleCPUDrawer
{
	DWORD TID,TAX,TAY;
	float TASpeed;
	bool  Rotate;
	BlendingTypes BlendType;
	bool  fColored;
	bVector4f DefColor;
	bVector3f Center;
	bVector3f Rot;
	bool      UseCenter,UseRot;
public:
	virtual void SetTA(const DWORD _TID,const DWORD TX,const DWORD TY,const float TS);
	virtual void SetParams(bool Rot,DWORD BT,bool Colored,const float *_DefColor);
	virtual void SetTRROT(const float *TR,const float *_Rot);
	virtual bool Draw    (CParticle *Particles,const DWORD Num);
	virtual CparticleCPUDrawer* Duplicate(){return new CparticleCPUDrawer();}
};

class CparticleGPUDrawer:public CparticleCPUDrawer
{
public:
	virtual bool Draw    (CParticle *Particles,const DWORD Num){return true;}
	virtual CparticleCPUDrawer* Duplicate(){return new CparticleGPUDrawer();}
};


class CparticleEmiterDrawer:public CparticleCPUDrawer
{
public:
	virtual bool Draw    (CParticle *Particles,const DWORD Num){return true;}
	virtual CparticleCPUDrawer* Duplicate(){return new CparticleEmiterDrawer();}
};

#endif