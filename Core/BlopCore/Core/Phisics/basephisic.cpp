/*===========================================================================
    Заголовок: BasePhisics.cpp
    Автор: Kashey
    Информация: Базовые обьекты для физики и симулятора

===========================================================================*/

// BasePhisic.cpp: implementation of the CBasePhisic class.
//
//////////////////////////////////////////////////////////////////////
#include "ProjectSpace.h"
#include "BasePhisic.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace Phisic;

CBasePhisic::CBasePhisic()
{
	SetActionType(ACTION_NONE);
	SetActionCallBack(NULL);
	MaxLifeTime=-1;
	LifeTime   =0;
	AutoDeath  =0;
	SetAutoDeath(0);
	_IsActionObject=false;
}


CBasePhisic::~CBasePhisic()
{

}

void        CBasePhisic::SetAutoDeath(bool Enable)
{
	if(AutoDeath<2) AutoDeath=Enable;
}
bool        CBasePhisic::GetAutoDeath()
{
	return AutoDeath==1;
}
bool        CBasePhisic::IsDead()
{
	return AutoDeath==2;
}

void       CBasePhisic::SetDead(){AutoDeath=2;}

DWORD SYS_PHISIC=0;
CBasePhisicSim::CBasePhisicSim():BaseObject(REFC_CORE,REFB_WORLD,"PHISICS")
{
	if(!SYS_PHISIC)
     RegisterNewConsoleClass(SYS_PHISIC,"PHISIC",-1);
}

DWORD CBasePhisicSim::Tick (float Time)
{
 IterateActionObjects(Time);
 return 1;
}

DWORD __GetSysPhisic(){return SYS_PHISIC;}


CBasePhisicSim* DefaultSim=NULL;

CBasePhisic * CBasePhisicSim::sCreateRigid()         {return DefaultSim->CreateRigid();}
CBasePhisic * CBasePhisicSim::sCreateRigidParticle() {return DefaultSim->CreateRigidParticle();}
CBasePhisic * CBasePhisicSim::sCreateAnimated()      {return DefaultSim->CreateAnimated();}
void          CBasePhisicSim::sDeleteRigid   (CBasePhisic *Obj){DefaultSim->DeleteRigid   (Obj);}
void          CBasePhisicSim::sDeleteAnimated(CBasePhisic *Obj){DefaultSim->DeleteAnimated(Obj);}


void SetDefaultPhisicSim(CBasePhisicSim* Sim)
{
	DefaultSim=Sim;
}

void COLLNULL (phCollisionInfo *Info)
{
}

void CBasePhisic::SetActionType(BYTE Type){ActionType=Type;}
BYTE CBasePhisic::GetActionType(){return ActionType;}
void CBasePhisic::SetActionCallBack(phActionCallBack CB){ ActionCallback=CB;}

void          CBasePhisicSim::AddActionObject(CBasePhisic *Obj)
{
	ActionObjects.push_back(Obj);
	Obj->SetObjectAsAction();
}
void          CBasePhisicSim::IterateActionObjects(float PPS)
{
	phObjectArray::iterator end=ActionObjects.end();
	for(phObjectArray::iterator it=ActionObjects.begin();it!=end;++it)
		if((*it)->LifeTime>0)
		switch((*it)->GetActionType())
	    {
		  case ACTION_NONE     :break;
		  case ACTION_CALLBACK :(*it)->CallActionCallBack(PPS);break;
		  case ACTION_MEMBER   :(*it)->MemberActionCallback(PPS);break;
	    }
}

bool          CBasePhisicSim::RemoveActionObject(CBasePhisic *Obj)
{
	if(!Obj->IsActionObject()) return false;
	phObjectArray::iterator end=ActionObjects.end();
	int i=0;
	for(phObjectArray::iterator it=ActionObjects.begin();it!=end;++it,++i)
		if((*it)==Obj){ ActionObjects.erase(i);return true;}
    return false;
}


void CBasePhisicSim::CanselRotation(CBasePhisic *ph,const bVector3f &Dir,const float PPS)
{
	bVector3f Rot;
	float A;
	ph->GetRotationM3(Rot,A);
	ph->SetRotationForceM3(Rot,-A);
}
