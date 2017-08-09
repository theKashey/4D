#include "StdAfx.h"
#include "TokaPhisic.h"
#include "PhBox.h"
#include "ragdude.h"

#define RAD_DUDE_BOX 0
#define RAD_DUDE_SPHERE 1
#define RAD_DUDE_CYLINDER 2

enum
{
	BONE_BODY,
	BONE_HEAD,
	BONE_RIGHT_ARM,
	BONE_LEFT_ARM,
	BONE_RIGHT_FOREARM,
	BONE_LEFT_FOREARM,
	BONE_RIGHT_THIGH,
	BONE_LEFT_THIGH,
	BONE_RIGHT_LEG,
	BONE_LEFT_LEG,
};

#define BONES_PER_DUDE  10
#define JOINTS_PER_DUDE 9

struct BoneData
{
	s32 geometryType;
	f32 zRotation;
	neV3 pos;
	neV3 size;
	neV3 colour;
    BoneData(
	s32 _geometryType,
	f32 _zRotation,
	neV3 _pos,
	neV3 _size,
	neV3 _colour)
	{
	 geometryType=_geometryType;
	 zRotation=_zRotation;
	 pos=_pos;
	 size=_size;
	 colour=_colour;
	}
};

BoneData bones[] = 
{
	BoneData(RAD_DUDE_BOX,		0.0f, neV3(0.0f, 0.0f, 0.0f), neV3(0.55f, 0.7f, 0.3f), neV3(0.8f, 0.2f, 0.2f)), //body 		
	BoneData(RAD_DUDE_SPHERE,	0.0f, neV3(0.0f, 0.55f, 0.0f), neV3(0.4f, 0.35f, 0.2f), neV3(0.8f, 0.8f, 0.2f)), //head 
	
	BoneData(RAD_DUDE_CYLINDER, -NE_PI / 2.0f, neV3(-0.45f, 0.28f, 0.0f), neV3(0.25f, 0.4f, 0.2f), neV3(0.2f, 0.2f, 0.8f)), //right arm 
	BoneData(RAD_DUDE_CYLINDER, NE_PI / 2.0f, neV3(0.45f, 0.28f, 0.0f), neV3(0.25f, 0.4f, 0.2f), neV3(0.2f, 0.2f, 0.8f)), //left arm

	BoneData(RAD_DUDE_BOX, -NE_PI / 2.0f, neV3(-0.9f, 0.28f, 0.0f), neV3(0.24f, 0.6f, 0.24f), neV3(0.2f, 0.2f, 0.8f)), //right forearm
	BoneData(RAD_DUDE_BOX, NE_PI / 2.0f, neV3(0.9f, 0.28f, 0.0f), neV3(0.24f, 0.6f, 0.24f), neV3(0.2f, 0.2f, 0.8f)), //left forearm

	BoneData(RAD_DUDE_CYLINDER, 0.0f, neV3(-0.20f, -0.6f, 0.0f), neV3(0.27f, 0.7f, 0.2f), neV3(0.3f, 0.7f, 0.2f)), //right thigh 
	BoneData(RAD_DUDE_CYLINDER, 0.0f, neV3(0.20f, -0.6f, 0.0f), neV3(0.27f, 0.7f, 0.2f), neV3(0.3f, 0.7f, 0.2f)), //left thigh 
	
	BoneData(RAD_DUDE_BOX, 0.0f, neV3(-0.20f, -1.3f, 0.0f), neV3(0.3f, 0.8f, 0.3f), neV3(0.3f, 0.7f, 0.2f)), //right leg
	BoneData(RAD_DUDE_BOX, 0.0f, neV3(0.20f, -1.3f, 0.0f), neV3(0.3f, 0.8f, 0.3f), neV3(0.3f, 0.7f, 0.2f)), //left leg
};

struct JointData
{
	s32 bodyA;
	s32 bodyB;
	neV3 pos;
	s32 type; // 0 = ball joint, 1 = hinge joint
	f32 xAxisAngle;
	f32 lowerLimit;
	f32 upperLimit;
	neBool enableLimit;
	neBool enableTwistLimit;
	f32 twistLimit;
	JointData(
	s32 _bodyA,
	s32 _bodyB,
	neV3 _pos,
	s32 _type,
	f32 _xAxisAngle,
	f32 _lowerLimit,
	f32 _upperLimit,
	neBool _enableLimit,
	neBool _enableTwistLimit,
	f32 _twistLimit=0
		)
		{
	 bodyA=_bodyA;
	 bodyB=_bodyB;
	 pos=_pos;
	 type=_type;
	 xAxisAngle=_xAxisAngle;
	 lowerLimit=_lowerLimit;
	 upperLimit=_upperLimit;
	 enableLimit=_enableLimit;
	 enableTwistLimit=_enableTwistLimit;
	 twistLimit=_twistLimit;
		}
};

JointData joints[] = 
{
	JointData(BONE_HEAD, BONE_BODY,				neV3(0.0f, 0.35f, 0.0f), 1, 0.0f, -NE_PI / 4.0f, NE_PI / 4.0f, true, false, 0.0f), //head <-> body
	
	JointData(BONE_RIGHT_ARM, BONE_BODY,			neV3(-0.22f, 0.28f, 0.0f), 0, NE_PI, 0.0f, NE_PI / 2.5f, true, true, 0.1f), //
	JointData(BONE_LEFT_ARM, BONE_BODY,			neV3(0.22f, 0.28f, 0.0f), 0, 0.0f, 0.0f, NE_PI / 2.5f, true, true, 0.1f),

	JointData(BONE_RIGHT_FOREARM, BONE_RIGHT_ARM,neV3(-0.65f, 0.28f, 0.0f), 1, NE_PI, 0.0f, NE_PI / 2.0f, true, false),
	JointData(BONE_LEFT_FOREARM, BONE_LEFT_ARM,	neV3(0.65f, 0.28f, 0.0f), 1, 0.0f, 0.0f, NE_PI / 2.0f, true, false),

	JointData(BONE_RIGHT_THIGH, BONE_BODY,		neV3(-0.20f, -0.32f, 0.0f), 0, NE_PI * 6.0f / 8.0f, 0.0f, NE_PI / 4.0f, true, true, 0.1f),
	JointData(BONE_LEFT_THIGH, BONE_BODY,		neV3(0.20f, -0.32f, 0.0f), 0, NE_PI * 2.0f / 8.0f, 0.0f, NE_PI / 4.0f, true, true, 0.1f),

	JointData(BONE_RIGHT_LEG, BONE_RIGHT_THIGH,	neV3(-0.20f, -0.95f, 0.0f), 1, -NE_PI * 0.5f, -NE_PI / 2.0f, 0.0f, true, false),
	JointData(BONE_LEFT_LEG, BONE_LEFT_THIGH,	neV3(0.20f, -0.95f, 0.0f), 1, -NE_PI * 0.5f, -NE_PI / 2.0f, 0.0f, true, false),
};

CRagDude::CRagDude(void)
{
}

CRagDude::~CRagDude(void)
{
}


void CRagDude::ChangeCollClass(DWORD C)
{
	for(int i=0;i<20;i++)
		if(!rigidBodies[i])return;
		else rigidBodies[i]->SetCollisionClass(C);

}

void CRagDude::MakeActive()
{
	for(int i=0;i<20;i++)
		if(!rigidBodies[i])return;
		else rigidBodies[i]->EnableGravity(true);

}

CBasePhisic* CRagDude::Make(bVector3f &_position)
{
	int cur;
	int scale = 1.0f;
    ZeroMemory(rigidBodies,sizeof(rigidBodies));
	neV3 position;
	for(int x=0;x<3;x++)
     position[x]=_position[x];
	int index=0;
	CPHBOX *Box;
	for (int i = 0; i < BONES_PER_DUDE; i++)
	{
		cur = index + i;

		rigidBodies[cur] = DefSim->CreateRigid();
		Box=new CPHBOX((CBasePhisic*)1);
		Box->ph=rigidBodies[cur];
		Box->ph->SetExUserData(this);
		Box->ph->SetCollisionClass(PH_COLL_RAGDOLL);
		rigidBodies[i]->EnableGravity(false);

		rigidBodies[cur]->CollideConnected(true);

		f32  mass;
	    mass = 8.0f;
		if (i == 0)
			mass = 20.0f;
		else if (i == 8 || i ==9)
			mass = 8.0f;
		mass/=1000.0f;
		rigidBodies[cur]->SetMass(mass);
		CBaseGemetry * geom = rigidBodies[cur]->AddGeometry();
		bVector3f _Box(bones[i].size[0] * scale, bones[i].size[1] * scale, bones[i].size[2] * scale);
        float CR= bones[i].size[0] * scale;
		float CH= bones[i].size[1] * scale;
		float Srad=bones[i].size[0] * scale;
		
// Box->SetScale(0.5,0.5,0.5);
		switch (bones[i].geometryType)
		{
		case RAD_DUDE_BOX:
			geom->SetBox(_Box);
			Box->SetScale(_Box[0]*0.5,_Box[1]*0.5,_Box[2]*0.5);
			rigidBodies[cur]->SetInertiaBox(_Box);		
			break;
		case RAD_DUDE_SPHERE:			
			geom->SetSphere(Srad);
			float BS;
			BS=Srad*0.5;
			Box->SetScale(BS,BS,BS);
			rigidBodies[cur]->SetInertiaSphere(Srad);
			break;
		case RAD_DUDE_CYLINDER:        
			geom->SetCylinder(CR,CH);
			Box->SetScale(CR*0.5,CH*0.5,CR*0.5);
			rigidBodies[cur]->SetInertiaCylinder(CR,CH);
			break;
		}

		rigidBodies[cur]->UpdateGeometry();
		neV3 pos;
		pos = bones[i].pos * scale+ position;
		bVector3f _pos(pos[0],pos[1],pos[2]);
		rigidBodies[cur]->SetPos(_pos);
		neQ quat;
		neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);
		quat.Set(bones[i].zRotation, zAxis);
		rigidBodies[cur]->SetRotation(&quat);
		rigidBodies[cur]->SetSleepingParameter(0.5f); //make it easier to sleep
		//rigidBodies[cur]->EnableGravity(false);
	}
	CBaseJoint * joint;
		
	neT3 jointFrame;

	int JPD=JOINTS_PER_DUDE;//
	for (i = 0; i < JPD; i++)
	{
		//if(i==5 || i==6)continue;
		joint = DefSim->CreateJoint(rigidBodies[joints[i].bodyA + index], rigidBodies[joints[i].bodyB + index]);
		jointFrame.SetIdentity();
		jointFrame.pos = joints[i].pos * scale + position;
	
		if (joints[i].type == 0)
		{
			joint->SetType(CBaseJoint::BALLSOCKET);
  		    neQ q;
			neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);
			q.Set(joints[i].xAxisAngle, zAxis);
			jointFrame.rot = q.BuildMatrix3();
		}
		else
		{
			/*if( i== 5 || i==6)
				joint->SetType(CBaseJoint::BALLSOCKET);
			else
			*/
			joint->SetType(CBaseJoint::HINGE);
			if (i == 3)
			{
				jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
				jointFrame.rot[1].Set(0.0f, 1.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 0.0f, 1.0f);
			}
			else if (i == 4)
			{
				jointFrame.rot[0].Set(-1.0f, 0.0f, 0.0f);
				jointFrame.rot[1].Set(0.0f, -1.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 0.0f, 1.0f);
			}
			else
			{
				jointFrame.rot[0].Set(0.0f, 0.0f, -1.0f);
				jointFrame.rot[1].Set(-1.0f, 0.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
			}
		}

		joint->SetJointFrameWorld((float*)&jointFrame);
		
		if (i == 5 || i == 6) // right
		{
			//neT3 body2w = ((CTokaRigid*)rigidBodies[joints[i].bodyB+index])->GetBody()->GetTransform();
			neT3 body2w = rigidBodies[joints[i].bodyB+index]->GetRealTransform();//>GetTransform();
			neT3 w2Body = body2w.FastInverse();
/*			w2Body.rot[0].n.W=1;
			w2Body.rot[1].n.W=1;
			w2Body.rot[2].n.W=1;
			w2Body.pos.n.W=1;
*/
			neM3 m;
			neQ q1, q2;
			neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);
			q1.Set(joints[i].xAxisAngle, zAxis);
			neV3 xAxis; xAxis.Set(1.0f, 0.0f, 0.0f);
			q2.Set(-NE_PI * 0.30f, xAxis);
			neQ q; q = q2 * q1;
			m = q1.BuildMatrix3();
			neT3 frame2body;
			frame2body.rot = w2Body.rot * m;
			frame2body.pos = w2Body * jointFrame.pos;
			//neT3 frame2w = body2w * frame2body;
			joint->SetJointFrameB((float*)&frame2body);

		}
		joint->SetLowerLimit(joints[i].lowerLimit);
		joint->SetUpperLimit(joints[i].upperLimit);
		if (joints[i].enableLimit)		joint->EnableLimit(true);
		if (joints[i].enableTwistLimit)
		{
			joint->SetLowerLimit2(joints[i].twistLimit);
			joint->EnableLimit2(true);
		}
		joint->SetIteration(4);
		joint->Enable(true);
	}
	ChangeCollClass(PH_COLL_RAGDOLL);
	return rigidBodies[0];
}
