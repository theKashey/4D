// BasePhisic.h: interface for the CBasePhisic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPHISIC_H__229C632D_859B_426A_A9AE_E195A05335EB__INCLUDED_)
#define AFX_BASEPHISIC_H__229C632D_859B_426A_A9AE_E195A05335EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Phisic
{

struct CBaseMaterial
{
	float friction   ; //trenie
	float restitution; //N
	CBaseMaterial(){friction=0.5;restitution=0.5;}
	CBaseMaterial(float A,float B){friction=A;restitution=B;}
};

class CBaseGemetry
{
public:
	virtual void SetMaterial(DWORD mid)=0;
	virtual DWORD GetMaterial()=0;
	virtual void SetBox(bVector3f Sizes){};
	virtual void SetSphere(float Rad){};
	virtual void SetCylinder(float Rad,float Height){};
	virtual void SetTransform(neT3 &Transform)=0;
};

struct BodyCollInfo;
class  CBasePhisicSim;
class  CBaseSensor;
class  CBaseController;

struct phT3
{
	float rot[3*4];
	float pos[4];
	phT3()
	{
	  ZeroMemory(rot,sizeof(rot));
	  ZeroMemory(pos,sizeof(pos));
	}
	phT3(float *_rot,float *_pos)
	{
		int i=0;
		for(i=0;i<4;++i)rot[i]=_rot[i+4];
		for(i=0;i<4;++i)rot[i+4]=_rot[i+4];
        for(i=0;i<4;++i)rot[i+8]=_rot[i+4];
		for(i=0;i<3;++i)pos[i]=_pos[i];
	}
	phT3(float *_rot1,float *_rot2,float *_rot3,float *_pos)
	{
		int i=0;
		for(i=0;i<12;i++)rot[i]=_rot1[i];
		for(i=0;i<4 ;i++)pos[i]=_pos[i];
	}
	bVector3f operator *     (const bVector3f& V  )
	{
		bVector3f tmp;
    	tmp[0] = rot[0] * V[0] + rot[4] * V[1] + rot[8]  * V[2];
	    tmp[1] = rot[1] * V[0] + rot[5] * V[1] + rot[9]  * V[2];
	    tmp[2] = rot[2] * V[0] + rot[6] * V[1] + rot[10] * V[2];
    	return tmp;
	}
};

#define ACTION_NONE     0
#define ACTION_CALLBACK 1
#define ACTION_MEMBER   2

class CBasePhisic ;
typedef void(*phActionCallBack)(CBasePhisic * Object,float PPS);

class DLLTYPE CBasePhisic  
{
protected:
	BodyCollInfo   *Info;
	LPCVOID         EUX;
	BYTE            ActionType;
    phActionCallBack ActionCallback;
	BYTE            AutoDeath;
	bool            _IsActionObject;
	DWORD           L1Collision;
	DWORD           L2Collision;
public:
	bool            IsActionObject(){return _IsActionObject;}
	void            SetObjectAsAction(){_IsActionObject=true;}
	float           LifeTime;
	float           MaxLifeTime;
	void SetLifeTime(float LT){MaxLifeTime=LT;}
  CBasePhisic();
  ~CBasePhisic();
    void        SetAutoDeath(bool Enable);
	bool        GetAutoDeath();
	bool        IsDead();
	void        SetDead();

  virtual void  SetMass            (float Mass){};
  virtual void  SetItertiaMass     (float Mass){};
  virtual float GetMass            ()                        {return 0;};
  virtual void  SetInertiaTensor   (bVector3f Intertion){};
  virtual void  SetInertiaBox      (bVector3f BoxParams){};
  virtual void  SetInertiaSphere   (float Rad){};
  virtual void  SetInertiaCylinder (float diameter, float height){};

  virtual void  Apply(){};
  virtual CBaseGemetry *AddGeometry()            {return NULL;}
  virtual void  UpdateGeometry(){};

  virtual nMatrix GetTransform()  {return nMatrix();}
  virtual neT3  GetRealTransform(){return neT3();}
  virtual void  GetPos(bVector3f &Pos){};
      bVector3f GetPos(){bVector3f tmp;GetPos(tmp);return tmp;}
  virtual void  SetPos(bVector3f &Pos){};
          void  SetPosTo(float *V){bVector3f Pos;GetPos(Pos);Pos.SetTo(V);}
  virtual void  GetRotationM3(bVector3f &Rot,float &Angle){};
 // virtual void  GetRotationQ (bVector3f &Rot,float &Angle){};

  virtual void  SetRotationForceM3(const bVector3f &Rot,const float &Angle){};

  virtual void  SetCollisionClass(DWORD Class){};
  virtual DWORD GetCollisionClass(){return 0;}

  virtual void  SetSpeed(bVector3f &Speed){};
  virtual void  GetSpeed(bVector3f &Speed){};
  virtual void	GetVelocityAtPoint(const bVector3f & Point,bVector3f &Speed){};
  bVector3f	GetVelocityAtPoint(const bVector3f & Point){ bVector3f Speed;GetVelocityAtPoint(Point,Speed);return Speed;}
  bVector3f     ReturnSpeed(){bVector3f Speed;GetSpeed(Speed);return Speed;}
  virtual void  AddSpeed(bVector3f &Speed){bVector3f S;GetSpeed(S);S+=Speed;SetSpeed(S);}
  virtual void  AddImpulse(bVector3f &Impulse){AddSpeed(Impulse);}
  virtual void  AddImpulseWithTwist(bVector3f &Impulse,bVector3f &Pos){AddImpulse(Impulse);}

  virtual void EnableGravity(bool Enable){};
  virtual void Enable(bool enable){};
  void SetExUserData(LPCVOID DATA){ EUX=DATA;}
  LPCVOID GetExUserData(){return EUX;}

  virtual bool CollideConnected(bool Enable){return false;};
  virtual void SetSleepingParameter(float param){};
  virtual void SetRotation(LPCVOID Q){};
  virtual void SetRotation(bVector3f &Rot,float &Angle){};
  virtual void SetActionType(BYTE Type);
          BYTE GetActionType();
		  void SetActionCallBack(phActionCallBack CB);
  virtual void MemberActionCallback(float PPS){};
          void CallActionCallBack(float PPS){if(ActionCallback) ActionCallback(this,PPS);};

  virtual CBaseSensor * AddSensor(){return NULL;};
  virtual CBaseController * AddController(CBaseController * BaseControler){return NULL;};
  virtual void BeginIterateSensor(){};
  virtual CBaseSensor * GetNextSensor(){return NULL;};
  
};

typedef CBasePhisic * PBasePhisic;

typedef enum
{
   RESPONSE_IGNORE = 0,
   RESPONSE_IMPULSE = 1,
   RESPONSE_CALLBACK = 2,
   RESPONSE_IMPULSE_CALLBACK = 3,
}phCollisionReponseBitFlag;

struct phCollisionInfo
{
    CBasePhisic   * bodyA;
    CBasePhisic   * bodyB;
    CBaseGemetry  * geometryA;
    CBaseGemetry  * geometryB;
    DWORD           materialIdA;
    DWORD           materialIdB;
    bVector3f       bodyContactPointA;
    bVector3f       bodyContactPointB;
    bVector3f       worldContactPointA;
    bVector3f       worldContactPointB;
    bVector3f       relativeVelocity;
    bVector3f       collisionNormal;
	CBasePhisicSim *Sim;
	float           PPS;
};

#define PH_COLL_NULL         0
#define PH_COLL_FLOOR        1
#define PH_COLL_PLAYER       2
#define PH_COLL_PARTICLE     3
#define PH_COLL_SHOT         4
#define PH_COLL_OBJECT       5
#define PH_COLL_LEVELOBJECT  6
#define PH_COLL_ACTIVEOBJECT 7
#define PH_COLL_PLUSOBJECT   8
#define PH_COLL_ROCKET       9
#define PH_COLL_STAIRS       10
#define PH_COLL_RAGDOLL      11
#define PH_COLL_BULLET       12

typedef void(*phCollisionCallBack)(phCollisionInfo *Info);


struct phCollisionTableElem
{
   DWORD O1,O2;
   phCollisionCallBack CallBack;
};

struct BodyCollInfo;

class DLLTYPE CBaseJoint
{
public:
	typedef enum
    {
		BALLSOCKET =0,
		BALLSOCKET2=1,
		HINGE      =2,
		SLIDE      =3
    }phJointTypes;
	virtual void SetType(phJointTypes Type)=0;
	virtual void SetJointFrameWorld(float *T4)=0;
	virtual void SetJointFrameA    (float *T4)=0;
	virtual void SetJointFrameB    (float *T4)=0;
	virtual void SetUpperLimit     (float limit)=0;
	virtual void SetLowerLimit     (float limit)=0;
	virtual bool EnableLimit(bool Enable){return false;}
	virtual void SetUpperLimit2    (float limit)=0;
	virtual void SetLowerLimit2    (float limit)=0;
	virtual bool EnableLimit2(bool Enable){return false;}
	virtual bool Enable(bool Enable){return false;}
	virtual void SetIteration(int Iterations)=0;
};

class DLLTYPE CBaseSensor
{
public:
	virtual void SetLineSensor(bVector3f &Pos,bVector3f &Dir){};
	virtual bVector3f GetDetectContactPoint()=0;
	virtual float     GetDetectDepth()=0;
	virtual bVector3f GetDetectNormal()=0;
	virtual bVector3f GetLinePos()=0;
	virtual bVector3f GetLineUnitVector()=0;


};

class DLLTYPE CBaseController
{
public:
	virtual void SetControllerForce (bVector3f &Forse){};
	virtual void SetControllerTorque(bVector3f &Forse){};
};

typedef CBaseController* PBaseController;
CBaseController DLLTYPE GetControllerMemmberCallback();

typedef SmartVector<CBasePhisic*> phObjectArray;

}
void DLLTYPE COLLNULL (Phisic::phCollisionInfo *Info);

namespace Phisic
{

class DLLTYPE CBasePhisicSim:public BaseObject
{
protected:
	phObjectArray       ActionObjects;
public:
	CBasePhisicSim();
	~CBasePhisicSim(){};

    virtual DWORD CreateMaterial(DWORD index,CBaseMaterial &Mat)=0;
	virtual DWORD GetMaterial(DWORD index,CBaseMaterial &Mat)=0;

    static  CBasePhisic * sCreateRigid();
	static  CBasePhisic * sCreateRigidParticle();
	static  CBasePhisic * sCreateAnimated();

	static void          sDeleteRigid(CBasePhisic *Obj);
    static void          sDeleteAnimated(CBasePhisic *Obj);

	virtual CBasePhisic * CreateRigid(){return NULL;};
	virtual CBasePhisic * CreateRigidParticle(){return NULL;};
	virtual CBasePhisic * CreateAnimated(){return NULL;};
	virtual CBaseJoint  * CreateJoint(CBasePhisic*A,CBasePhisic*B){return NULL;}
    virtual void          DeleteRigid(CBasePhisic *Obj){};
    virtual void          DeleteAnimated(CBasePhisic *Obj){};
	virtual void          AddActionObject(CBasePhisic *Obj);
	virtual bool          RemoveActionObject(CBasePhisic *Obj);
	virtual void          IterateActionObjects(float PPS);

	virtual bool Start(){return false;};
	virtual void Stop (){};
	virtual DWORD Tick (float Time);
	virtual GString GetName(){return "NULL DRIVER";}
	virtual void AddBlow(bVector3f Position,float Rad,float Force){};

	virtual void SetCollisionAction(DWORD O1,DWORD O2,phCollisionReponseBitFlag Action,phCollisionCallBack CB=COLLNULL){};

	static void CanselRotation(CBasePhisic *ph,const bVector3f &Dir,const float PPS);
	
};


typedef CBasePhisicSim *PBasePhisicSim;

struct BodyCollInfo
{
  CBasePhisicSim *Sim;
  CBasePhisic    *Body;
};
typedef BodyCollInfo* PBodyCollInfo;


}

void DLLTYPE SetDefaultPhisicSim(Phisic::CBasePhisicSim* Sim);
DWORD DLLTYPE __GetSysPhisic();


#endif // !defined(AFX_BASEPHISIC_H__229C632D_859B_426A_A9AE_E195A05335EB__INCLUDED_)
