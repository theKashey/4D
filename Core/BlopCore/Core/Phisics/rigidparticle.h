#include "ProjectSpace.h"
#include "BasePhisic.h"

using namespace Phisic;

class CRigidParticleControler
{
public:
	static bool Rotater(){return true;}
	static bool Mover  (){return true;}
	static void Move    (CParticle &Particle,float FPS)
	{
		((CBasePhisic*)Particle.ID)->SetPosTo(Particle.Position);
	};
	static bool Register(CParticle &Particle,WORD ID)
	{
		Particle.ID=(DWORD)CBasePhisicSim::sCreateRigidParticle();
		if(!Particle.ID)return false;
		CBasePhisic* ph=(CBasePhisic*)Particle.ID;
		ph->SetMass(Particle.Mass);
		float& boxSizeY=Particle.Size;
		ph->SetCollisionClass(PH_COLL_PARTICLE);
        ph->SetInertiaBox(bVector3f(boxSizeY,boxSizeY,boxSizeY));
		ph->AddGeometry()->SetSphere(boxSizeY);
        ph->UpdateGeometry();
        ph->Apply();
		bVector3f Pos(Particle.Position);
		((CBasePhisic*)Particle.ID)->SetPos(Pos);
        bVector3f Speed(Particle.Speed);
		((CBasePhisic*)Particle.ID)->SetSpeed(Speed);
		ph->SetExUserData(NULL);
		return Particle.ID!=0;
	};
	static bool TryKill (CParticle &Particle)
	{
		CBasePhisic* ph=(CBasePhisic*)Particle.ID;
        bVector3f& A=ph->ReturnSpeed();
		if(A.Length()<0.001f){Particle.DoDisabled();}
		return true;
	};
	static bool Kill    (CParticle &Particle)
	{
        CBasePhisic* ph=(CBasePhisic*)Particle.ID;
		CBasePhisicSim::sDeleteRigid(ph);
		return true;
	};
};