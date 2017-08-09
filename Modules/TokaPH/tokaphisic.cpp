// TokaPhisic.cpp: implementation of the CTokaPhisic class.
//
//////////////////////////////////////////////////////////////////////
#define MAX_OVERLAPPED_PAIR 1000

#include "StdAfx.h"
#include "ProjectCounter.h"
#include "TokaPhisic.h"

#pragma comment (lib,"tokamak.lib")

float in_PPS=0;

extern float c_PPS;

MakeProfile(TokaTick,"Phisics");
MakeProfile(TokaCycle,"Advance");


nMatrix tokaNET3_TO_MATRIX4X4f(neT3 &T)
{
	nMatrix m;
	NET3_TO_MATRIX4X4f(m,T);
	return m;
}

#define neT3_TO_phT3( a) (phT3(a.rot.M[0].v,a.rot.M[1].v,a.rot.M[2].v,a.pos.v));


DWORD SYS_PHISIC=0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FILE *TokaLogF=NULL;

void TokaLog(char *str)
{
	if(!TokaLogF)TokaLogF=fopen("tokalog","w+");
	fprintf(TokaLogF,"%s",str);
	fflush(TokaLogF);
}

void CollisionCallback (neCollisionInfo & collisionInfo)
{
	BodyCollInfo *BodyA;
	BodyCollInfo *BodyB;
	if (collisionInfo.typeA == NE_RIGID_BODY)
	{
		neRigidBody * rbA = (neRigidBody *)collisionInfo.bodyA;
		BodyA=(PBodyCollInfo)rbA->GetUserData();
	}
	if (collisionInfo.typeB == NE_RIGID_BODY)
	{
		neRigidBody * rbB = (neRigidBody *)collisionInfo.bodyB;
		BodyB=(PBodyCollInfo)rbB->GetUserData();
	}
	if (collisionInfo.typeA == NE_ANIMATED_BODY)
	{
		neAnimatedBody * rbA = (neAnimatedBody *)collisionInfo.bodyA;
		BodyA=(PBodyCollInfo)rbA->GetUserData();
	}
	if (collisionInfo.typeB == NE_ANIMATED_BODY)
	{
		neAnimatedBody * rbB = (neAnimatedBody *)collisionInfo.bodyB;
		BodyB=(PBodyCollInfo)rbB->GetUserData();
	}
	if(!BodyA || !BodyB) return;
    ((CTokaSimulator*)BodyA->Sim)->MakeColl(BodyA,BodyB,collisionInfo);
}


CTokaSimulator:: CTokaSimulator():CBasePhisicSim()
{
	if(!SYS_PHISIC)SYS_PHISIC=__GetSysPhisic();
	sim=NULL;
	PhParams.NumAnimated=300;
	PhParams.NumRigidParticles=1500;
	PhParams.NumRigits  =300+PhParams.NumRigidParticles;
    DWORD n=PhParams.NumAnimated+PhParams.NumRigits+PhParams.NumRigidParticles;
	PhParams.NumGeometry=n;
	PhParams.OverlappedCount=(n * (n - 1)) / 2;
	PhParams.ConstraintsCount = 100;
	PhParams.SensorsCount     = 10;
}

CTokaSimulator::~CTokaSimulator()
{
	Stop();
}

bool CTokaSimulator::Start()
{
	Create();
	return true;
}

void CTokaSimulator::Stop()
{
	try
	{
    if(sim)
	 neSimulator::DestroySimulator(sim);
	}
	catch(...)
	{
     Print2Console(SYS_PHISIC,"Unable to delete");
	}
	sim=NULL;
}


void CTokaSimulator::SetParams(CTokaSectorParams &Params)
{
	PhParams=Params;
}

void CTokaStarter(DWORD Param)
{
  ((CTokaSimulator*)Param)->TickCycle();
}

void CTokaSimulator::Create(CTokaSectorParams *Params)
{
	if(Params)SetParams(*Params);
	neV3 gravity;
	gravity.Set(0.0f, -9.0f, 0.0f);
	neSimulatorSizeInfo sizeInfo;
	sizeInfo.rigidBodiesCount          = PhParams.NumRigits;
	sizeInfo.animatedBodiesCount       = PhParams.NumAnimated;
	sizeInfo.geometriesCount           = PhParams.NumGeometry;
	sizeInfo.overlappedPairsCount      = PhParams.OverlappedCount;
	sizeInfo.rigidParticleCount        = PhParams.NumRigidParticles;
	sizeInfo.constraintsCount          = PhParams.ConstraintsCount;
//	sizeInfo.sensorsCount              = PhParams.SensorsCount;
//	sizeInfo.controllersCount          = 10;
	sizeInfo.terrainNodesStartCount    = 0;//200;//PhParams.200;
	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetCollisionCallback(CollisionCallback);
	sim->SetLogOutputCallback(TokaLog);
	sim->SetLogOutputLevel();
	NumRigitParticles=NumRigits=NumAnimated=0;
	PrepareMark();
	Print2Console(SYS_PHISIC,"CREATING ROOM(Tokamak)\n -Static:%d\n -Rigits:%d;\n -Particles:%d\n -Geometries:%d\n -Constraints:%d\n -Overlapped:%d\n",
		          PhParams.NumAnimated,PhParams.NumRigits,PhParams.NumRigidParticles,PhParams.NumGeometry,PhParams.ConstraintsCount,PhParams.OverlappedCount);
}



void CTokaSimulator::SetCollisionAction(DWORD O1,DWORD O2,phCollisionReponseBitFlag Action,phCollisionCallBack CB)
{
	neCollisionTable::neReponseBitFlag set;
	set=neCollisionTable::RESPONSE_IMPULSE;
	switch(Action)
	{
	case RESPONSE_IGNORE          : set=neCollisionTable::RESPONSE_IGNORE;          break;
	case RESPONSE_IMPULSE         : set=neCollisionTable::RESPONSE_IMPULSE;         break;
	case RESPONSE_CALLBACK        : set=neCollisionTable::RESPONSE_CALLBACK;        break;
	case RESPONSE_IMPULSE_CALLBACK: set=neCollisionTable::RESPONSE_IMPULSE_CALLBACK;break;
	}
	sim->GetCollisionTable()->Set(O1, O2,set);
	sim->GetCollisionTable()->Set(O2, O1,set);
	phCollisionTableElem Elem;
	Elem.CallBack=CB;
	Elem.O1=O1;
	Elem.O2=O2;
	CollTable.push_back(Elem);
}

CBasePhisic *CTokaSimulator::CreateRigid()
{
	if(NumRigits>PhParams.NumRigits-10) return NULL;
	neRigidBody * rigidBody = sim->CreateRigidBody();
    CTokaRigid * Rigid=new CTokaRigid(this);
	Rigid->SetBody(rigidBody);
	Rigits.push_back(Rigid);
	NumRigits++;
	return Rigid;
}

CBasePhisic *CTokaSimulator::CreateRigidParticle()
{
	//return CreateRigid();
	if(NumRigitParticles>PhParams.NumRigidParticles-20) return NULL;
	neRigidBody * rigidBody = sim->CreateRigidParticle();
    CTokaRigid * Rigid=new CTokaRigid(this);
	Rigid->IsParticle(true);
	Rigid->SetBody(rigidBody);
	Rigits.push_back(Rigid);
	NumRigitParticles++;
	return Rigid;
}

void        CTokaSimulator::_DeleteRigid(CBasePhisic *_Obj)
{
   CTokaRigid *Obj=(CTokaRigid *)_Obj;
   neRigidBody * rigidBody=Obj->GetBody();
   if(rigidBody)
    sim->FreeRigidBody(rigidBody);
   else return;
   Obj->SetBody(NULL);
   if(Obj->IsParticle())
    NumRigitParticles--;
   else
    NumRigits--;
   
   Rigits.checkbound();
   Rigits.erasePopItem(Obj);
   
   if(Obj->GetAutoDeath())
   delete Obj;
   else Obj->SetDead();
}

CBasePhisic *CTokaSimulator::CreateAnimated()
{
	neAnimatedBody * animatedBody = sim->CreateAnimatedBody();
    CTokaAnimated  * Animated=new CTokaAnimated(this);
	Animated->SetBody(animatedBody);
	Animateds.push_back(Animated);
	return Animated;
}
void        CTokaSimulator::_DeleteAnimated(CBasePhisic *_Obj)
{
   CTokaAnimated *Obj=(CTokaAnimated *)_Obj;
   neAnimatedBody * animatedBody=Obj->GetBody();    
   sim->FreeAnimatedBody(animatedBody);
   Animateds.checkbound();
   Animateds.erasePopItem(Obj);
   Obj->SetBody(NULL);
   delete Obj;
}

CBaseJoint *CTokaSimulator::CreateJoint(CBasePhisic *_A,CBasePhisic*_B)
{
    CTokaRigid *A=(CTokaRigid *)_A;
	CTokaRigid *B=(CTokaRigid *)_B;
	neJoint * joint = sim->CreateJoint(A->GetBody(),B->GetBody());
    CTokaJoint *Joint=new CTokaJoint(this);
	Joint->SetJoint(joint);
	return Joint;
}

void        CTokaSimulator::DeleteRigid(CBasePhisic *_Obj)
{
	RigedDeleted.push_back(_Obj);
	_Obj->Enable(false);
	RemoveActionObject(_Obj);
}
void        CTokaSimulator::DeleteAnimated(CBasePhisic *_Obj)
{
	AnimatedDeleted.push_back(_Obj);
	_Obj->Enable(false);
    RemoveActionObject(_Obj);
}

 void           CTokaSimulator::PrepareMark()
 {
	 RigedDeleted.clear();
	 AnimatedDeleted.clear();
 }
 void           CTokaSimulator::DeleteMarked()
 {
	 for(int i=0;i<RigedDeleted.size();i++)
		 _DeleteRigid(RigedDeleted[i]);
     for(int j=0;j<AnimatedDeleted.size();j++)
		 _DeleteAnimated(AnimatedDeleted[j]);
 }

void    CTokaSimulator::TickCycle()
{
}

DWORD CTokaSimulator::VTick(float Time)
{
	Profile(TokaTick);
	DetectCycle(DC_PHC);
	float simc;
	try
	{
		Profile(TokaCycle);
		simc=GetCounter(DC_PHC);
		if(simc>1.0/60.0f) simc=1.0f/60.0f;
		//simc=1.0f/200.f;
		in_PPS=simc;
		*Get_c_PPS()=simc;
		sim->Advance(simc,1);
		CBasePhisicSim::Tick(simc);
		CheckLifeTime(simc);
	}
	catch(...)
	{
		Print2Console(SYS_PHISIC,"exception detected at %f PHC",1/simc);
	}
	DeleteMarked();
	PrepareMark();
	return 0;
}

DWORD CTokaSimulator::Tick(float FPS)
{	
 return VTick(FPS);
}

void CTokaSimulator::CheckLifeTime(float DeltaTime)
{
	RigitsArray::iterator end=Rigits.end();
	for(RigitsArray::iterator it=Rigits.begin();it!=end;it++)
	{
		(*it)->LifeTime+=DeltaTime;
		if((*it)->MaxLifeTime>0) 
			if((*it)->LifeTime>(*it)->MaxLifeTime) DeleteRigid(*it);
	}
}

void CTokaSimulator::AddBlow(bVector3f Position,float Rad,float Force)
{
	bVector3f Pos,Tw;
	for(int i=0;i<Rigits.size()-2;i++)
	if(Rigits[i])
	{
		Rigits[i]->GetPos(Pos);
		Pos=(Pos-Position);
		float len=Pos.Length();
		if(len<Rad)
		{
			Pos.Normalize();
			Tw=Pos;
			len=1-len/Rad;
			len=len*len*len;
			//n*=len;
			Pos*=len*Force;
			Tw*=len*0.0001;
			Rigits[i]->AddSpeed(Pos);//WithTwist(Pos,Tw);
		}
	}
}

void CTokaSimulator::MakeColl(BodyCollInfo *B1,BodyCollInfo* B2,neCollisionInfo & collisionInfo)
{
	DWORD O1=B1->Body->GetCollisionClass();
	DWORD O2=B2->Body->GetCollisionClass();
	for(int i=0;i<CollTable.size();i++)
	{
		if((CollTable[i].O1==O1 && CollTable[i].O2==O2)||
			CollTable[i].O1==O2 && CollTable[i].O2==O1)
		{
			if(CollTable[i].CallBack)
			{
				 phCollisionInfo Info;
				 Info.bodyA=B1->Body;
				 Info.bodyB=B2->Body;
				 Info.PPS=in_PPS;
				 Info.Sim=this;
				 CollTable[i].CallBack(&Info);
			}
			return;
		}
	}
}

 DWORD CTokaSimulator::CreateMaterial(DWORD index,CBaseMaterial &Mat)
 {
	 bool ok=false;
	 ok=sim->SetMaterial(index,Mat.friction,Mat.restitution);
	 return (index!=0) & (ok);
 }
	 
 DWORD CTokaSimulator::GetMaterial   (DWORD index,CBaseMaterial &Mat)
 {
	 sim->GetMaterial(index,Mat.friction,Mat.restitution);
	 return index;
 }


 bool CTokaSimulator::Prepare()
 {
	 return AwaitSnapShot();
 }

 bool CTokaSimulator::AwaitSnapShot()
 {
	 return true;
 }

//************************************************************
//************************************************************
//************************************************************
//************************************************************

CTokaGeometry::CTokaGeometry(neGeometry* _geom)
{
	geom=_geom;
}

void CTokaGeometry::SetBox(bVector3f Sizes)
{
	if(geom)geom->SetBoxSize(Sizes[0],Sizes[1],Sizes[2]);
}
void CTokaGeometry::SetSphere(float Rad)
{
	if(geom)geom->SetSphereDiameter(Rad);
}
void CTokaGeometry::SetCylinder(float Rad,float Height)
{
	if(geom)geom->SetCylinder(Rad,Height);
}

void CTokaGeometry::SetMaterial(DWORD mid)
{
	if(geom)geom->SetMaterialIndex(mid);
}
DWORD CTokaGeometry::GetMaterial()
{
	if(geom)return geom->GetMaterialIndex();
	else    return 0;
}

void CTokaGeometry::SetTransform(neT3 &Transform)
{
	if(geom)geom->SetTransform(Transform);
}

//************************************************************
//************************************************************
//************************************************************
//************************************************************

CTokaRigid::CTokaRigid(CTokaSimulator *Sim)
{
  SetBody(NULL);
  _IsParticle=false;
  MySim=Sim;
  Info=new BodyCollInfo;
  Info->Sim=Sim;
  Info->Body=this;
}
CTokaRigid::~CTokaRigid()
{
  if(MySim && GetBody())
	  MySim->DeleteRigid(this);
  MySim=NULL;
  if(Info)
	  delete Info;
  Info=NULL;
}

void  CTokaRigid::SetBody(neRigidBody *_Body)
{
	Body=_Body;
	if(Body)
	Body->SetUserData((DWORD)Info);
}
neRigidBody *CTokaRigid::GetBody(){return Body;}


void  CTokaRigid::SetMass(float _Mass)
{IMass=Mass=_Mass;}
void  CTokaRigid::SetItertiaMass(float _Mass)
{IMass=_Mass;}
float CTokaRigid::GetMass()
{return Mass;}
void  CTokaRigid::SetInertiaTensor(bVector3f _Inertia)
{Inertia.Set(Inertia[0],Inertia[1],Inertia[3]);}
void  CTokaRigid::SetInertiaBox   (bVector3f BoxParams)
{Inertia=neBoxInertiaTensor(BoxParams[0],BoxParams[1],BoxParams[2],IMass);}
void  CTokaRigid::SetInertiaSphere(float Rad)
{Inertia=neSphereInertiaTensor(Rad,IMass);}
void  CTokaRigid::SetInertiaCylinder (float diameter, float height)
{Inertia=neCylinderInertiaTensor(diameter,height,IMass);}


void CTokaRigid::Apply()
{
	Body->SetInertiaTensor(Inertia);
	Body->UpdateInertiaTensor();
	Body->SetMass(Mass);
}

CBaseGemetry *CTokaRigid::AddGeometry()
{
	return new CTokaGeometry(Body->AddGeometry());
}
void  CTokaRigid::UpdateGeometry()
{
 Body->UpdateBoundingInfo();
}

void  CTokaRigid::GetPos(bVector3f &Pos)
{
	neV3 pos=Body->GetPos();
	Pos[0]=pos[0];
	Pos[1]=pos[1];
	Pos[2]=pos[2];
    //for(int i=0;i<3;i++)
	// Pos[i]=pos[i];

}
void  CTokaRigid::SetPos(bVector3f &Pos)
{
    neV3 pos;
	pos.Set(Pos[0],Pos[1],Pos[2]);
	//pos[4]=1;
	Body->SetPos(pos);
}
void  CTokaRigid::GetRotationM3(bVector3f &Rot,float &angle)
{
	neQ q=Body->GetRotationQ();
	neV3 pos;
	f32 Rad;
	q.GetAxisAngle(pos,Rad);
    for(int i=0;i<3;i++)
	 Rot[i]=pos[i];
	angle=PI2d180*Rad;
}

void  CTokaRigid::SetRotationForceM3(const bVector3f &Rot,const float &Angle)
{
	neQ Q;
	bVector3f V;
	//Q.Set(Angle,Rot);
	//Body->get
	Body->SetAngularMomentum(V);
	Body->SetTorque(V);
}

nMatrix CTokaRigid::GetTransform()
{
	nMatrix T;
	NET3_TO_MATRIX4X4f(T,Body->GetTransform());
	return T;
}

neT3 CTokaRigid::GetRealTransform()
{
	return (Body->GetTransform());
}

void  CTokaRigid::SetSpeed(bVector3f &Speed)
{
	neV3 pos;
	pos.Set(Speed[0],Speed[1],Speed[2]);
	Body->SetVelocity(pos);
}

void  CTokaRigid::GetSpeed(bVector3f &Speed)
{
	neV3 pos;
	pos=Body->GetVelocity();
	Speed[0]=pos[0];
	Speed[1]=pos[1];
	Speed[2]=pos[2];
}

void	CTokaRigid::GetVelocityAtPoint(const bVector3f & Point,bVector3f &Speed)
{
	neV3 pos;
	pos.Set(Point[0],Point[1],Point[2]);
	neV3 speed=Body->GetVelocityAtPoint(pos);
    Speed[0]=speed[0];
	Speed[1]=speed[1];
	Speed[2]=speed[2];
}

void  CTokaRigid::AddImpulse(bVector3f &Impulse)
{
    neV3 Imp;
	Imp.Set(Impulse[0],Impulse[1],Impulse[2]);
	Body->ApplyImpulse(Imp);
}

void  CTokaRigid::AddImpulseWithTwist(bVector3f &Impulse,bVector3f &Pos)
{
    neV3 Imp;
	Imp.Set(Impulse[0],Impulse[1],Impulse[2]);
	neV3 pos;
	pos.Set(Pos[0],Pos[1],Pos[2]);
	#ifdef TOKA1_2
	Body->ApplyImpulse(Imp,pos);
    #else
	Body->ApplyImpulseWithTwist(Imp,Pos);
    #endif
}

void  CTokaRigid::SetCollisionClass(DWORD Class)
{
	Body->SetCollisionID(Class);
}
DWORD CTokaRigid::GetCollisionClass()
{
	return Body->GetCollisionID();
}

void CTokaRigid::EnableGravity(bool Enable)
{
	Body->GravityEnable(Enable);
}

bool CTokaRigid::CollideConnected(bool Enable)
{
	Body->CollideConnected(Enable);
	return true;
}
void CTokaRigid::SetSleepingParameter(float param)
{
	Body->SetSleepingParameter(param);
}
void CTokaRigid::SetRotation(LPCVOID Q)
{
    neQ *q=(neQ*)Q;
	Body->SetRotation(*q);
}

void CTokaRigid::SetRotation(bVector3f &Rot,float &Angle)
{
	neQ q;
	q.Set(Angle,Rot);
	Body->SetRotation(q);
}

CBaseSensor * CTokaRigid::AddSensor()
{
	return new CTokaSensor(Body->AddSensor());
}
CBaseController * CTokaRigid::AddController(CBaseController * BaseControler)
{
	if(BaseControler)
	{
		CTokaController *Controller=(CTokaController *)BaseControler;
        return new CTokaController(Body->AddController(Controller->GetControllerCallback(),0));
	}
	else
	return new CTokaController(Body->AddController(NULL,0));
}
void CTokaRigid::BeginIterateSensor()
{
	Body->BeginIterateSensor();
}
CBaseSensor * CTokaRigid::GetNextSensor()
{
	neSensor* S=Body->GetNextSensor();
    if(!S) return NULL;
	return (CBaseSensor*)S->GetUserData();
}

//************************************************************
//************************************************************
//************************************************************
//************************************************************

CTokaAnimated::CTokaAnimated(CTokaSimulator *Sim)
{
  SetBody(NULL);
  MySim=Sim;
  Info=new BodyCollInfo;
  Info->Sim=Sim;
  Info->Body=this;
}
CTokaAnimated::~CTokaAnimated()
{
  if(MySim && GetBody())
	  MySim->DeleteAnimated(this);
  MySim=NULL;
  if(Info)
	  delete Info;
  Info=NULL;
}

void  CTokaAnimated::SetBody(neAnimatedBody *_Body)
{
	Body=_Body;
	if(Body)
	Body->SetUserData((DWORD)Info);
}
neAnimatedBody *CTokaAnimated::GetBody(){return Body;}


CBaseGemetry *CTokaAnimated::AddGeometry()
{
	return new CTokaGeometry(Body->AddGeometry());
}
void  CTokaAnimated::UpdateGeometry()
{
 Body->UpdateBoundingInfo();
}

void  CTokaAnimated::GetPos(bVector3f &Pos)
{
	neV3 pos=Body->GetPos();
	Pos[0]=pos[0];
	Pos[1]=pos[1];
	Pos[2]=pos[2];
    //for(int i=0;i<3;i++)
	// Pos[i]=pos[i];

}
void  CTokaAnimated::SetPos(bVector3f &Pos)
{
    neV3 pos;
	pos.Set(Pos[0],Pos[1],Pos[2]);
	Body->SetPos(pos);
}
void  CTokaAnimated::GetRotationM3(bVector3f &Rot,float angle)
{
	neQ q=Body->GetRotationQ();
	neV3 pos;
	f32 Rad;
	q.GetAxisAngle(pos,Rad);
    for(int i=0;i<3;i++)
	 Rot[i]=pos[i];
	angle=PI2d180*Rad;
}

nMatrix CTokaAnimated::GetTransform()
{
	nMatrix T;
	NET3_TO_MATRIX4X4f(T,Body->GetTransform());
	return T;
}


void  CTokaAnimated::SetCollisionClass(DWORD Class)
{
	Body->SetCollisionID(Class);
}
DWORD CTokaAnimated::GetCollisionClass()
{
	return Body->GetCollisionID();
}


//************************************************************
//************************************************************
//************************************************************
//************************************************************

CTokaJoint::CTokaJoint(CTokaSimulator *_MySim)
{
	Joint=NULL;
	MySim=_MySim;
}
void CTokaJoint::SetJoint(neJoint *_Joint)
{
	Joint=_Joint;
}
void CTokaJoint::SetType(phJointTypes Type)
{
#ifdef TOKA1_2
	neJoint::ConstraintType type;
#else
	neJoint::JointType type;
#endif
	switch(Type)
	{
	  case BALLSOCKET :type=neJoint::NE_JOINT_BALLSOCKET; break;
	  case BALLSOCKET2:type=neJoint::NE_JOINT_BALLSOCKET2;break;
	  case HINGE      :type=neJoint::NE_JOINT_HINGE;      break;
	  case SLIDE      :type=neJoint::NE_JOINT_SLIDE;      break;
	}
	Joint->SetType(type);
}
void CTokaJoint::SetJointFrameWorld(float *T4)
{
	neT3* frame=(neT3*)T4;
	Joint->SetJointFrameWorld(*frame);
}
void CTokaJoint::SetJointFrameA    (float *T4)
{
	neT3* frame=(neT3*)T4;
	Joint->SetJointFrameA(*frame);
}
void CTokaJoint::SetJointFrameB    (float *T4)
{
	neT3* frame=(neT3*)T4;
	Joint->SetJointFrameB(*frame);
}
void CTokaJoint::SetUpperLimit     (float limit)
{
	Joint->SetUpperLimit(limit);
}
void CTokaJoint::SetLowerLimit     (float limit)
{
	Joint->SetLowerLimit(limit);
}
bool CTokaJoint::EnableLimit(bool Enable)
{
	Joint->EnableLimit(Enable);
	return true;
}
void CTokaJoint::SetUpperLimit2     (float limit)
{
	Joint->SetUpperLimit2(limit);
}
void CTokaJoint::SetLowerLimit2     (float limit)
{
	Joint->SetLowerLimit2(limit);
}
bool CTokaJoint::EnableLimit2(bool Enable)
{
	Joint->EnableLimit2(Enable);
	return true;
}
bool CTokaJoint::Enable(bool _Enable)
{
	Joint->Enable(_Enable);
	return true;
}
void CTokaJoint::SetIteration(int Iterations)
{
	Joint->SetIteration(Iterations);
}

//************************************************************
//************************************************************
//************************************************************
//************************************************************
CTokaSensor::CTokaSensor(neSensor *s)
{
	Sensor=s;
	Sensor->SetUserData((DWORD)this);
}
void CTokaSensor::SetLineSensor(bVector3f &Pos,bVector3f &Dir)
{
	Sensor->SetLineSensor(Pos,Dir);
}
bVector3f CTokaSensor::GetDetectContactPoint()
{
	return Sensor->GetDetectContactPoint();
}
float     CTokaSensor::GetDetectDepth()
{
	return Sensor->GetDetectDepth();
}
bVector3f CTokaSensor::GetDetectNormal()
{
   return Sensor->GetDetectNormal();
}
bVector3f CTokaSensor::GetLinePos()
{
	return Sensor->GetLinePos();
}
bVector3f CTokaSensor::GetLineUnitVector()
{
	return Sensor->GetLineUnitVector();
}




//************************************************************
//************************************************************
//************************************************************
//************************************************************

CTokaController::CTokaController(neRigidBodyController *c)
{
	Controller=c;
}
void CTokaController::SetControllerForce (bVector3f &Force)
{
	Controller->SetControllerForce(Force);
}
void CTokaController::SetControllerTorque(bVector3f &Force)
{
	Controller->SetControllerTorque(Force);
}







CTokaSimulator* DefSim;


PBlopBaseObject  GetToka()
{
	DefSim=new CTokaSimulator();
    SetDefaultPhisicSim(DefSim);
	return DefSim;
}



PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN
    MAKE_PLUGIN (GetToka,"PHISIC",0,true);
    END_PLUGIN
}
