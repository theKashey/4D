// TokaPhisic.h: interface for the CTokaPhisic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOKAPHISIC_H__B03F208F_A914_40EA_A407_6BBF8F599686__INCLUDED_)
#define AFX_TOKAPHISIC_H__B03F208F_A914_40EA_A407_6BBF8F599686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ProjectSpace.h"
#include "include\tokamak.h"
#define TOKA1_2
#include "Core\Phisics\BasePhisic.h"
using namespace Phisic;

struct CTokaSectorParams
{
 int	NumRigits,
	    NumAnimated,
     	NumGeometry,
	    NumRigidParticles,
		OverlappedCount,
		ConstraintsCount,
        SensorsCount;
};  

class CTokaSimulator;

class DLLTYPE CTokaGeometry:public CBaseGemetry
{
	 neGeometry * geom;
public:
	CTokaGeometry(neGeometry* geom);
	virtual void SetBox(bVector3f Sizes);
	virtual void SetSphere(float Rad);
	virtual void SetCylinder(float Rad,float Height);

	virtual void SetMaterial(DWORD mid);
	virtual DWORD GetMaterial();
	void SetTransform(neT3 &Transform);
};

class DLLTYPE CTokaRigid:public CBasePhisic
{
  neRigidBody    *Body;
  CTokaSimulator *MySim;
  float           Mass,IMass;
  neV3            Inertia;
  bool            _IsParticle;
public:
  void  SetBody(neRigidBody *Body);
  neRigidBody *GetBody();
  CTokaRigid(CTokaSimulator *Sim);
 ~CTokaRigid();


  void  SetMass(float Mass);
  void  SetItertiaMass(float Mass);
  float GetMass();
  void  SetInertiaTensor(bVector3f Intertion);
  void  SetInertiaBox   (bVector3f BoxParams);
  void  SetInertiaSphere(float Rad);
  void  SetInertiaCylinder (float diameter, float height);

  void  Apply();
  CBaseGemetry *AddGeometry();
  void  UpdateGeometry();

  void  GetPos(bVector3f &Pos);
  void  SetPos(bVector3f &Pos);
  void  GetRotationM3(bVector3f &Rot,float &Angle);
  void  SetRotationForceM3(const bVector3f &Rot,const float &Angle);
  nMatrix GetTransform();
  neT3    GetRealTransform();

  void  SetSpeed  (bVector3f &Speed);
  void  GetSpeed  (bVector3f &Speed);
  void	GetVelocityAtPoint(const bVector3f & Point,bVector3f &Speed);
  
  void  AddImpulse(bVector3f &Impulse);
  void  AddImpulseWithTwist(bVector3f &Impulse,bVector3f &Pos);
  

  virtual void  SetCollisionClass(DWORD Class);
  virtual DWORD GetCollisionClass();
  virtual void EnableGravity(bool Enable);

  bool IsParticle(){return _IsParticle;}
  void IsParticle(bool F){_IsParticle=F;}

  virtual bool CollideConnected(bool Enable);
  virtual void SetSleepingParameter(float param);
  virtual void SetRotation(LPCVOID Q);
  virtual void SetRotation(bVector3f &Rot,float &Angle);

  virtual CBaseSensor * AddSensor();
  virtual CBaseController * AddController(CBaseController * BaseControler);
  virtual void BeginIterateSensor();
  virtual CBaseSensor * GetNextSensor();
};

class DLLTYPE CTokaAnimated:public CBasePhisic
{
  neAnimatedBody    *Body;
  CTokaSimulator *MySim;
public:
  void  SetBody(neAnimatedBody *Body);
  neAnimatedBody *GetBody();
  CTokaAnimated(CTokaSimulator *Sim);
 ~CTokaAnimated();

  CBaseGemetry *AddGeometry();
  void  UpdateGeometry();

  void  GetPos(bVector3f &Pos);
  void  SetPos(bVector3f &Pos);
  void  GetRotationM3(bVector3f &Rot,float Angle);
  nMatrix GetTransform();

  virtual void  SetCollisionClass(DWORD Class);
  virtual DWORD GetCollisionClass();


};

class DLLTYPE CTokaSensor : public CBaseSensor
{
	neSensor *Sensor;
public:
    CTokaSensor(neSensor *s);
	virtual void SetLineSensor(bVector3f &Pos,bVector3f &Dir);
	virtual bVector3f GetDetectContactPoint();
	virtual float     GetDetectDepth();
	virtual bVector3f GetDetectNormal();
	virtual bVector3f GetLinePos();
	virtual bVector3f GetLineUnitVector();


};

class DLLTYPE CTokaController: public CBaseController
{
	neRigidBodyController *Controller;
public:
	CTokaController(neRigidBodyController *c);
	virtual void SetControllerForce (bVector3f &Forse);
	virtual void SetControllerTorque(bVector3f &Forse);
	neRigidBodyController *GetController(){return Controller;}
	neRigidBodyControllerCallback *GetControllerCallback(){return NULL;}
};

class DLLTYPE CTokaJoint: public CBaseJoint
{
	neJoint *Joint;
    CTokaSimulator *MySim;
public:
	CTokaJoint(CTokaSimulator *_MySim);
	void SetJoint(neJoint *_Joint);
	virtual void SetType(phJointTypes Type);
	virtual void SetJointFrameWorld(float *T4);
	virtual void SetJointFrameA    (float *T4);
	virtual void SetJointFrameB    (float *T4);
	virtual void SetUpperLimit     (float limit);
	virtual void SetLowerLimit     (float limit);
	virtual bool EnableLimit(bool Enable);
	virtual void SetUpperLimit2    (float limit);
	virtual void SetLowerLimit2    (float limit);
	virtual bool EnableLimit2(bool Enable);
	virtual bool Enable(bool Enable);
	virtual void SetIteration(int Iterations);
};


typedef  SmartVector<CTokaRigid*> RigitsArray;

class DLLTYPE CTokaSimulator:public CBasePhisicSim
{
 neAllocatorDefault all;
 CTokaSectorParams  PhParams;
 neSimulator *sim;
 RigitsArray                 Rigits;
 SmartVector<CTokaAnimated*> Animateds;
 SmartVector<phCollisionTableElem> CollTable;
 SmartVector<CBasePhisic   *>RigedDeleted;
 SmartVector<CBasePhisic   *>AnimatedDeleted;

 void           _DeleteAnimated(CBasePhisic *Obj);
 void           _DeleteRigid(CBasePhisic *Obj);

 int           NumRigitParticles,NumRigits,NumAnimated;
public:
 CTokaSimulator();
~CTokaSimulator();
 CBasePhisic   *CreateRigid();
 CBasePhisic   *CreateRigidParticle();
 void           DeleteRigid(CBasePhisic *Obj);

 CBasePhisic   *CreateAnimated();
 void           DeleteAnimated(CBasePhisic *Obj);

 CBaseJoint  * CreateJoint(CBasePhisic*A,CBasePhisic*B);
 
 DWORD CreateMaterial(DWORD index,CBaseMaterial &Mat);
 DWORD GetMaterial   (DWORD index,CBaseMaterial &Mat);

 void           PrepareMark();
 void           DeleteMarked();

 void SetParams(CTokaSectorParams &Params);
 void Create(CTokaSectorParams *Params=NULL);
 virtual bool Start();
 virtual void Stop ();
 void    TickCycle();
 DWORD   VTick(float Time);
 virtual DWORD Tick (float Time);
 GString GetName(){return "TOKAMAK DRIVER";}

 virtual void AddBlow(bVector3f Position,float Rad,float Force);

 virtual void SetCollisionAction(DWORD O1,DWORD O2,phCollisionReponseBitFlag Action,phCollisionCallBack CB=COLLNULL);

 void MakeColl(BodyCollInfo *B1,BodyCollInfo* B2,neCollisionInfo & collisionInfo);

 void CheckLifeTime(float DeltaTime);
 
 bool Prepare();

 bool AwaitSnapShot();
};

typedef CTokaSimulator* PTokaSimulator;

extern CTokaSimulator* DefSim;

nMatrix tokaNET3_TO_MATRIX4X4f(neT3 &T);

#define NET3_TO_MATRIX4X4f(T, tmat )                                    \
T.Set( tmat.rot[0][0], tmat.rot[1][0], tmat.rot[2][0], tmat.pos[0],     \
       tmat.rot[0][1], tmat.rot[1][1], tmat.rot[2][1], tmat.pos[1],     \
       tmat.rot[0][2], tmat.rot[1][2], tmat.rot[2][2], tmat.pos[2],     \
       0.0f,          0.0f,          0.0f,          1.0f           );

#endif // !defined(AFX_TOKAPHISIC_H__B03F208F_A914_40EA_A407_6BBF8F599686__INCLUDED_)
