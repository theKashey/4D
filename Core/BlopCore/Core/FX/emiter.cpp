/*===========================================================================
    Заголовок: DLLManager.cpp
    Автор: Kashey
    Информация: Particle emiters 

===========================================================================*/
#include "ProjectSpace.h"
#include "emiter.h"

CParticleEmiter::CParticleEmiter(void)
{
	Size     =0.1f;
	Mass     =0.01f;
    DeltaMass=0;
    DeltaSize=0;
	Life=1000;
	DeltaLife=0;
	SpeedFactor=1;
	DeltaSpeedFactor=0;
	RotationSpeed=0;
    DeltaRotationSpeed=0;
}

CParticleEmiter::~CParticleEmiter(void)
{
}

bool CParticleEmiter::SetParamf      (const DWORD ParamID,float Val)
{
    switch(ParamID)
    {
    case PAREM_MASS      :Mass     =Val;return true;
    case PAREM_SIZE      :Size     =Val;return true;
    case PAREM_DELTAMASS :DeltaMass=Val;return true;
    case PAREM_DELTASIZE :DeltaSize=Val;return true;
	case PAREM_LIFE      :Life=Val;return true;
	case PAREM_DELTALIFE :DeltaLife=Val;return true;
	case PAREM_SPEEDFACTOR      :SpeedFactor=Val;return true;
	case PAREM_DELTASPEEDFACTOR :DeltaSpeedFactor=Val;return true;

	case PAREM_ROTATIONSP:RotationSpeed=Val;return true;
    case PAREM_DELTAROTATIONSP:DeltaRotationSpeed=Val;return true;

    };
    return false;
}

float CParticleEmiter::GetSpeedFactor()
{
	return SpeedFactor+DeltaSpeedFactor*random();
}

bool CPointSpeedEmiter::SetParamVector (const DWORD ParamID,const float* Val)
{
    if(CParticleEmiter::SetParamVector(ParamID,Val)) return true;
    switch(ParamID)
    {
    case PAREM_CENTER     :Center     =Val;return true;
    case PAREM_SPEED      :Speed      =Val;return true;
    case PAREM_DELTACENTER:DeltaCenter=Val;return true;
    case PAREM_DELTASPEED :DeltaSpeed =Val;return true;
    };
    return false;
}

bool CSphereParticleEmiter::SetParamVector (const DWORD ParamID,const float* Val)
{
    if(CParticleEmiter::SetParamVector(ParamID,Val)) return true;
    switch(ParamID)
    {
    case PAREM_CENTER     :Center     =Val;return true;
    case PAREM_SPEED      :Speed      =Val;return true;
    case PAREM_DELTACENTER:DeltaCenter=Val;return true;
    case PAREM_DELTASPEED :DeltaSpeed =Val;return true;
    case PAREM_RAD1       :RadX =Val[0];RadY =Val[1];RadZ =Val[2];return true;
    case PAREM_RAD2       :RadXm=Val[0];RadYm=Val[1];RadZm=Val[2];return true;
    };
    return false;
}
bool CSphereParticleEmiter::SetParam2Vector(const DWORD ParamID,const float* V1,const float* V2)
{
    if(CParticleEmiter::SetParam2Vector(ParamID,V1,V2)) return true;
    switch(ParamID)
    {
    case PAREM_RAD       :RadX =V1[0];RadY =V1[1];RadZ =V1[2];
                          RadXm=V2[0];RadYm=V2[1];RadZm=V2[2];return true;
    };
    return false;
}


int CSphereParticleEmiter::Emite(CParticle *Particles,WORD Num)
{
	float dx,dy,dz;
	dx=RadX-RadXm;
	dy=RadY-RadYm;
	dz=RadZ-RadZm;
	for(int i=0;i<Num;++i,++Particles)
	{
        bVector3f V(randomM(),randomM(),randomM());
        V.normalize(RadX-random()*dx,RadY-random()*dy,RadZ-random()*dz);
		(Center+V).SetTo((*Particles).Position);//
		bVector3f _Speed=((-V)+Speed)*GetSpeedFactor();
		//Speed.Normalize();
		(*Particles).Size=Size;
		(*Particles).Mass=Mass;
		(*Particles).Speed=_Speed;
        (*Particles).Speed[3]=RotationSpeed+DeltaRotationSpeed*(random()-0.5f);
		//(*Speed   )=V;
	}
	return Num;
}


int CSphereSpeedParticleEmiter::Emite(CParticle *Particles,WORD Num)
{
	float dx,dy,dz;
	dx=RadX-RadXm;
	dy=RadY-RadYm;
	dz=RadZ-RadZm;
	for(int i=0;i<Num;++i,++Particles)
	{
        bVector3f V(randomM(),randomM(),randomM());
        V.normalize(RadX-random()*dx,RadY-random()*dy,RadZ-random()*dz);
		V*=GetSpeedFactor();
		VectorSet3((*Particles).Position,Center);//
		VectorSet3((*Particles).Speed,V);
		(*Particles).Size=Size;
		(*Particles).Mass=Mass;
		//(*Speed   )=V;
	}
	return Num;
}

int CPointSpeedEmiter::Emite(CParticle *Particles,WORD Num)
{
	for(int i=0;i<Num;++i,++Particles)
	{
		VectorSet3((*Particles).Position,Center);//
		VectorSet3((*Particles).Speed,Speed);
	    (*Particles).Speed[3]=RotationSpeed+DeltaRotationSpeed*(random()-0.5f);
		(*Particles).Size=Size;
		(*Particles).Mass=Mass;
	}
	return Num;
}



int CPointParticleEmiter::Emite(CParticle *Particles,WORD Num)
{
	bVector3f V(0,0,0);
	for(int i=0;i<Num;i++,Particles++)
	{
 		VectorSet3((*Particles).Position,Center);//
		VectorSet3((*Particles).Speed,V);
		(*Particles).Size=Size;
		(*Particles).Mass=Mass;
	}
	return Num;
}


void CDefRotated::Move    (CParticle *Particle,WORD Num,float FPS)
{
		for(int i=0;i<Num;++i,++Particle) 
		 Particle->Rotation+=Particle->Speed[3]*FPS;
}

CCloudRotater::CCloudRotater():CParticleEmiter()
{
      SizeChange=0;
      ColorChanging=false;
}
bool CCloudRotater::SetParamf      (const DWORD ParamID,float Val)
{
		if(CParticleEmiter::SetParamf(ParamID,Val)) return true;
        switch(ParamID)
		{
          case PAREM_SIZECHANGE:SizeChange=Val;return true;
		}
		return false;
}

bool CCloudRotater::SetParamVector(const DWORD ParamID,const float* Val1)
{
		if(CParticleEmiter::SetParamVector(ParamID,Val1)) return true;
        switch(ParamID)
		{
          case PAREM_COLORCHANGE:ColorChange=Val1;ColorChange*=0xFF;ColorChanging=true;return true;
		}
		return false;
}

void CCloudRotater::Move    (CParticle *Particle,WORD Num,float FPS)
{
		for(int i=0;i<Num;++i,++Particle) 
		{
		Particle->Size+=SizeChange*FPS;
		Particle->Rotation+=Particle->Speed[3]*FPS;
		if(ColorChanging)
		{
			bVector3f D=ColorChange*FPS;
		    for(int i=0;i<3;++i)
			{
				int  Res=Particle->Color[i]+D[i];
				if(Res>0)Particle->Color[i]=Res;
				else     Particle->Color[i]=0;
			}
		}
		Particle->Position+=(float*)(Particle->Speed*FPS);
		}
};

bool CGravitator::SetParamVector(const DWORD ParamID,const float* Val1)
{
		if(CParticleEmiter::SetParamVector(ParamID,Val1)) return true;
        switch(ParamID)
		{
          case PAREM_COLORCHANGE:ColorChange=Val1;ColorChange*=0xFF;ColorChanging=true;return true;
		}
		return false;
}

void CGravitator::Move    (CParticle *Particle,WORD Num,float FPS)
{
        for(int i=0;i<Num;++i,++Particle) 
		{
		if(ColorChanging)
		{
			bVector3f D=ColorChange*FPS;
		    for(int i=0;i<3;++i)
			{
				int Res=Particle->Color[i]+D[i];
				if(Res>0)Particle->Color[i]=Res;
				else     Particle->Color[i]=0;
			}
			if(Particle->Color[0]==Particle->Color[1] && 
			   Particle->Color[1]==Particle->Color[2] && 
			   Particle->Color[2]==0)
				Particle->DoDisabled();
		}
		Particle->Position+=(float*)(Particle->Speed*FPS);
		Particle->Speed[2]-=100*FPS;
		}
}

void CCloudRotaterMin::Move    (CParticle *Particle,WORD Num,float FPS)
{
       for(int i=0;i<Num;++i,++Particle) 
		Particle->Position+=(float*)(Particle->Speed*FPS);
};

