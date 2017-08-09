#pragma once

#define PARTICLE_DISABLED              1

struct CParticle
{
	DWORD     ID;
	bVector3f Position;
	bVector4f Speed   ; 
    float     Rotation;
	float     Size;
	float     Mass;
	float     Friction;
	BYTE      Color[4];
    BYTE      Flags;
	int       LifeTime;
	DWORD     Texture;
	void SetSpeed(float x,float y,float z){Speed.Set(x,y,z,0);}
	void SetColor(const float *C){for(int i=0;i<4;++i)Color[i]=C[i]*0xFF;}
	bool IsDisabled(){ return (Flags & PARTICLE_DISABLED);}
	void DoDisabled(){         Flags|= PARTICLE_DISABLED ;}
	void DoEnabled (){         Flags&=!PARTICLE_DISABLED ;}
};

struct CParticleOut
{
	float Position[3];
	float Tex     [3];
	float Color   [4];
};

struct CParticleOutVS
{
	float Position[4];
	float Tex     [3];
	float Color   [4];
};
