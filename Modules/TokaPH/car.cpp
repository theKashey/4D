#include "StdAfx.h"
#include "TokaPhisic.h"
#include "PhBox.h"
#include "car.h"
#include "PhBox.h"

bVector3f sensorData[4] = 
{
	bVector3f(2.0f, 0.0f, 1.5f),
	bVector3f(2.0f, 0.0f, -1.5f),
	bVector3f(-2.0f, 0.0f, 1.5f),
	bVector3f(-2.0f, 0.0f, -1.5f),
};


void Carcallback(CBasePhisic * Object,float PPS)
{
	CCar *Car=(CCar*)Object->GetExUserData();
	if(!Car) return;
	Car->Process();
	//Car->CarController();
}

class CControllerCB: public neRigidBodyControllerCallback
{
public:
	
	void RigidBodyControllerCallback(neRigidBodyController * controller);
};


void CControllerCB::RigidBodyControllerCallback(neRigidBodyController * controller)
{
	neRigidBody * rb = controller->GetRigidBody();
	CBasePhisic *B=((PBodyCollInfo)rb->GetUserData())->Body;
	CCar * car = (CCar *)B->GetExUserData();

	car->CarController();
}

CControllerCB cb;


CCar::CCar(void)
{
	MAX_SPEED=15;
	MAX_REVERSE_SPEED=-2;
    MAX_STEER=0.7f;
	MAX_SLIDE=0.8f;
	WHEEL_DIAMETER=0.9f;
	accel=0;
	steer=0;//PI/4.0f;
	slide=0;
	SteerDir[0].SetZero();
	SteerDir[1].SetZero();
}

CCar::~CCar(void)
{
}

DWORD CarSound=0;

void CCar::MakeCar(bVector3f Pos,bVector3f Orientation)
{
	float mass = 1.0f;
	carBody = DefSim->CreateRigid();
	//carBody->CollideConnected(true);
	CBaseGemetry * geom;
	bVector3f boxSize; 
	neT3 t; 
	t.SetIdentity();
	geom= carBody->AddGeometry();
	
    boxSize.Set(6.0f, 1.0f, 3.2f);
	geom->SetBox(boxSize);
	t.pos.Set(0.0f, 0.25f, 0.0f);
	geom->SetTransform(t);

    CPHBOX* Box=new CPHBOX((CBasePhisic*)1);

    if(!CarSound)
     CarSound=Sound::CreateSound("sound/common/bet");//mshot.wav");
    Box->mySound=Sound::DuplicateSound(CarSound);
	//Sound::PlaySound(Box->mySound);
	Box->ph=carBody;
	float s=0.5f;
	Box->SetScale(boxSize[0]*s,boxSize[1]*s,boxSize[2]*s);
	Box->SetTransform(t);

	geom = carBody->AddGeometry();
	boxSize.Set(2.5f, 1.0f, 2.8f);
	geom->SetBox(boxSize);
	t.pos.Set(-0.6f, 1.0f, 0.0f);
	geom->SetTransform(t);

	Box=new CPHBOX((CBasePhisic*)1);
	Box->ph=carBody;
	Box->SetScale(boxSize[0]*s,boxSize[1]*s,boxSize[2]*s);
	Box->SetTransform(t);

	//CBaseSensor * sn;
	CBaseSensor *sn;
	for (s32 si = 0; si < 4; si++)
	{
		sn =carBody->AddSensor();//carBody->AddSensor();
		bVector3f snPos, snDir;
		snDir.Set(0.0f, -1.0f, 0.0f);
		snPos=(sensorData[si]);
		sn->SetLineSensor(snPos, snDir);
	}
		// add a controller to the rigid body
	
//	controller = new CBaseControler(carBody->AddController(&cb);//&GetControllerMemmberCallback());
	controller = new CTokaController(((CTokaRigid*)carBody)->GetBody()->AddController(&cb,0));
	
	carBody->UpdateGeometry();
	carBody->SetMass(mass);
    boxSize.Set(0.1f,0.1f,0.1f);
	carBody->SetInertiaBox(boxSize);
	//carBody->SetMass(mass);
	carBody->SetExUserData(this);
	carBody->Apply();
	// set the car position
	carBody->SetPos(Pos);
	DefSim->AddActionObject(carBody);
	carBody->SetActionCallBack(Carcallback);
	carBody->SetActionType(ACTION_CALLBACK);
	
	
}

void CCar::MakeParts(bVector3f Pos,bVector3f Orientation){}

void CCar::Process()
{
	bVector3f vel;
	if (GetAsyncKeyState('L'))
    {
		vel.Set (0.0f,5.0f,0.0f);
		carBody->SetSpeed(vel);
	}
	if (GetAsyncKeyState('T'))
	{
		accel += 0.1f;
		if (accel > MAX_SPEED)
			accel = MAX_SPEED;
	} 
	else if (GetAsyncKeyState('G'))
	{
		accel -= 0.1f;
		if (accel < MAX_REVERSE_SPEED)
			accel = MAX_REVERSE_SPEED;
	}
	else 
		accel *= 0.99f;
	if (GetAsyncKeyState('F'))
	{
		steer += 0.02f;
		if (steer > MAX_STEER)
		{
		steer = MAX_STEER;
		slide += 0.05f;
		}
	}
	else if (GetAsyncKeyState('H'))
	{
		steer -= 0.02f;
		if (steer < -MAX_STEER)
		{
			steer = -MAX_STEER;
			slide += 0.05f;
		}
	}
	else
	{
		steer *= 0.9f;
		slide *= 0.9f;
	}
	if (slide > MAX_SLIDE)
		slide = MAX_SLIDE;

    neT3 body2World = carBody->GetRealTransform();
	for(int i=0;i<2;i++)
	if (i == 0 || i == 1)
		{
	 	 neT3 wheel2World, suspension2Body, tire2Suspension, cylinder2Tire;
	     suspension2Body.SetIdentity();
		 suspension2Body.pos.Set(0.0f, -suspensionLength[i] + WHEEL_DIAMETER / 2.0f, 0.0f);
	     suspension2Body.pos += sensorData[i];
	     cylinder2Tire.SetIdentity();
       	 neV3 rot; rot.Set(NE_PI * 0.5f, 0.0f, 0.0f);
     	 cylinder2Tire.rot.RotateXYZ(rot);
	     tire2Suspension.SetIdentity();
	 	 rot.Set(0.0f, steer, 0.0f);
		 tire2Suspension.rot.RotateXYZ(rot);
	     wheel2World = body2World * suspension2Body * tire2Suspension * cylinder2Tire;
		 //wheel2World = body2World *tire2Suspension;
		 SteerDir[i] = wheel2World.rot[0];
		}
}

void CCar::CarController()
{
	neT3 body2World = carBody->GetRealTransform();
	carBody->BeginIterateSensor();
	CBaseSensor * sn;
	neV3 force, torque;
	force.SetZero();
	torque.SetZero();
	s32 i = 0;
	while (sn = carBody->GetNextSensor())
	{
		f32 k = 6.0f; //spring constant
		f32 u = 0.8f; //damping constant
		if (i == 2 || i == 3)
		{
			k = 10.0f;
		}
		// add spring force
		f32 depth = sn->GetDetectDepth();
		//gw.car.suspensionLength[i] = 1.0f - depth;
		suspensionLength[i] = 1.0f - depth;
		if (depth == 0.0f)
		{
			i++;
			continue;
		}
		neV3 groundNormal = sn->GetDetectNormal();
		neV3 lineNormal   = body2World.rot * sn->GetLineUnitVector();
		neV3 linePos      = body2World     * sn->GetLinePos();
		f32 dot           = lineNormal.Dot(groundNormal) * -1.0f;
		neV3 f = depth * lineNormal * -k ;//* dot;
		force += f;
		neV3 r = linePos - carBody->GetPos();
		torque += r.Cross(f);
		// add damping force
		f32 speed = carBody->GetVelocityAtPoint(r).Dot(lineNormal);
		f = -speed * lineNormal * u;
		force += f;
		torque += r.Cross(f);
		// add friction force
		neV3 vel = carBody->GetVelocityAtPoint(sn->GetDetectContactPoint() - carBody->GetPos());
		vel.RemoveComponent(groundNormal);
		if (i == 0 || i == 1)
			vel.RemoveComponent(SteerDir[i]);
		else
			vel.RemoveComponent(body2World.rot[0]); //rear wheel always parallel to car body
		f = vel;
		f.Normalize();
		f *= -2.0f;
		if (i == 2 || i ==3)
			f *= (1.0f - slide);
		f[1] = 0.0f;
		force += f;
		r = sn->GetDetectContactPoint() - carBody->GetPos();
		torque += r.Cross(f);
		// driving force
		if (i == 2 || i == 3) // rear wheel
		{
			f = body2World.rot[0];
			f *= accel;
			force  += f;
			torque += r.Cross(f);
		}
		i++;
	}
	// drag
	f32 dragConstant = 0.5f;
	neV3 vel  = carBody->ReturnSpeed();//>GetVelocity();
	f32  dot  = vel.Dot(body2World.rot[0]);
	neV3 drag = dot * body2World.rot[0] * -dragConstant;
	force += drag;
	torque[1]*=10;
	controller->SetControllerForce(force);
	controller->SetControllerTorque(torque);

}

