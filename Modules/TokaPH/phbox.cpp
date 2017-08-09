// PHBOX.cpp: implementation of the CPHBOX class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TokaPhisic.h"
#include "PHBOX.h"
#include "ProjectParticles.h"


extern float in_PPS;

#define FIREBALL_SIZE 10

//////////////////////////////////////////////////////////////////////
// construction/Destruction
//////////////////////////////////////////////////////////////////////

#define boxSizeY 0.8f
#define groundLevel -10.0f

struct BDataE
{
	float V[3];
	float C[3];
	float T[2];
};




//typedef CParticleSet<CPointParticleEmiter,PARD,CCloudRotater,PARD> CloudParticle;
CBaseParticleSystem *BaseCloudParticle=NULL;

CBaseParticleSystem *RocketFlareEmiter=NULL;
CBaseParticleSystem *RocketCoreEmiter=NULL;

WORD Ind[0xFF];
DWORD IndC;
BDataE BData[0xFF];
DWORD  BDataSize=0;

DWORD Compiled=0;
DWORD ExpSound=0;

CPHBOX::CPHBOX(CBasePhisic *ph2)
{
 mySound=0;
 mySoundPlaying=false;

 if(1!=(DWORD)ph2)
 {
 if(ph2) ph=ph2;
 else
 ph=DefSim->CreateRigid();
 if(!ph) ph=new CBasePhisic();
 ph->SetMass(1.0f);
 float Size=1.5;
 ph->SetCollisionClass(PH_COLL_OBJECT);
 ph->SetInertiaBox(bVector3f(boxSizeY,boxSizeY,boxSizeY));
 CBaseGemetry *G=ph->AddGeometry();
 if(G)
	 G->SetBox(bVector3f(boxSizeY,boxSizeY,boxSizeY));
 ph->UpdateGeometry();
 ph->Apply();
 ph->SetCollisionClass(1);
 ph->SetExUserData(this);
 }

 SetScale(boxSizeY*0.5f,boxSizeY*0.5f,boxSizeY*0.5f);
 if(!Compiled)
 {
	 Draw::CRenderStream S;
	 S.StrideSet (*BData);
	 S.MakeLocalGeom((BDataE*)BData,BDataSize);
	 S.SetVertex (BData[0].V);
	 S.SetColor  (BData[0].C);
	 S.SetTex    (0,BData[0].T);
	 Data.Object.AddStream(S);
  Data.Compile();
  Compiled=Data.GetListID();
  Data.DrawAsList();
 }
 else
  Data.DrawAsList(Compiled);
}


DWORD CPHBOX::OnMessage(const CBlopMessage &Message)
{
	bVector3f D;
	switch(Message.Msg)
	{
 	 case bCMD_MOVE:
		 D=*((const bVector3f*)Message.Data);
         //if(D.IsConsiderZero()) break;
         float FPS=in_PPS;
         FPS*=100;
         if(FPS<1000)
		  ph->SetSpeed(D*FPS);
//         bVector3f Pos;
		 //ph->GetPos(Pos);
         SendUpSignal(CObjectIDTwin(Message.Address.From,GetID()),bCMD_SYNC_POS,4*3,(LPVOID)&ph->GetPos());

		 break;
	}
	return 1;
}




DWORD RocketSound=0;

CRocket::CRocket()
{
	MyFireBallID=0;
 if(!RocketFlareEmiter)
 {
	 //
 }
 //ph=DefSim->CreateRigid();
 //if(!ph) ph=new CBasePhisic();
 ph->SetMass(1.1f);
 float Size=0.5;
#define boxSizeY2 0.1f
 ph->SetInertiaBox(bVector3f(boxSizeY2,boxSizeY2,boxSizeY2));
  ph->SetInertiaBox(bVector3f(boxSizeY,boxSizeY,boxSizeY));
 CBaseGemetry *G=ph->AddGeometry();
 if(G)
 {
	 G->SetSphere(boxSizeY2);//>SetBox(bVector3f(boxSizeY2,boxSizeY2,boxSizeY2));
     G->SetMaterial(0);
 }
 ph->UpdateGeometry();
 ph->Apply();
 SetScale(boxSizeY2*0.5f,boxSizeY2*0.5f,boxSizeY2*0.5f);
// CGenericGraphic::AddObject(this);
 ph->SetCollisionClass(PH_COLL_ROCKET);
 ph->EnableGravity(false);
 ph->SetExUserData(this);
 if(!RocketSound)
  RocketSound=Sound::CreateSound("Data/Sound/ir_inter.wav");//mshot.wav");
 mySound=Sound::DuplicateSound(RocketSound);
 mySoundPlaying=false;
// Sound::PlaySound(mySound);
 // CGenericGraphic::RemoveObject(this);
}

CPHBOX::~CPHBOX()
{
 if(mySound)
	 Sound::KillSound(mySound);
 DefSim->DeleteRigid(ph);
// CGenericGraphic::RemoveObject(this);

}

void   CPHBOX::SetTransform(neT3 &T)
{
	Tr=T;
}

Draw::PDrawableDataArray CPHBOX::GetData(Draw::CDrawableElement *Out)
{
	if(!ph || ph->IsDead()) 
	{
		delete this;
		return NULL;
	}
	neT3 TT=ph->GetRealTransform();
	TT.pos+=Tr.pos;
	nMatrix T;
	NET3_TO_MATRIX4X4f(T,TT);
	Data.Transform.SetTransform(ph->GetTransform());//T);
	Data.Transform.SetScaleEx(Scale);
	if(mySound)
	{
		Sound::SetSoundPos(mySound,ph->GetPos());
		if(!mySoundPlaying)
			Sound::PlaySound(mySound);
		mySoundPlaying=true;
	}
	Out=&Data;
	return NULL;
}


void    CPHBOX::Move(float x,float y,float z)
{
	ph->SetPos(bVector3f(x,y,z));
}

void    CPHBOX::SetScale(float x,float y,float z)
{
	Scale[0]=x;Scale[1]=y;Scale[2]=z;
}
void CreateBox()
{
 ZeroMemory(BData,sizeof(BData));
 BYTE i=0;
 BData[i].V[0]= 1;BData[i].V[1]= 1;BData[i].V[2]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[0]= 1;BData[i].V[1]= 1;BData[i].V[2]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[1]= 1;BData[i].V[2]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[1]= 1;BData[i].V[2]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;

 BData[i].V[0]= 1;BData[i].V[1]=-1;BData[i].V[2]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[0]= 1;BData[i].V[1]=-1;BData[i].V[2]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[1]=-1;BData[i].V[2]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[1]=-1;BData[i].V[2]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;

 BData[i].V[1]= 1;BData[i].V[0]= 1;BData[i].V[2]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[1]= 1;BData[i].V[0]= 1;BData[i].V[2]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[1]=-1;BData[i].V[0]= 1;BData[i].V[2]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[1]=-1;BData[i].V[0]= 1;BData[i].V[2]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;

 BData[i].V[1]= 1;BData[i].V[0]=-1;BData[i].V[2]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[1]= 1;BData[i].V[0]=-1;BData[i].V[2]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[1]=-1;BData[i].V[0]=-1;BData[i].V[2]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[1]=-1;BData[i].V[0]=-1;BData[i].V[2]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;

 BData[i].V[0]= 1;BData[i].V[2]= 1,BData[i].V[1]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[0]= 1;BData[i].V[2]= 1,BData[i].V[1]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[2]= 1,BData[i].V[1]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[2]= 1,BData[i].V[1]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;

 BData[i].V[0]= 1;BData[i].V[2]=-1,BData[i].V[1]= 1;BData[i].T[0]=0;BData[i].T[1]=0;i++;
 BData[i].V[0]= 1;BData[i].V[2]=-1,BData[i].V[1]=-1;BData[i].T[0]=0;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[2]=-1,BData[i].V[1]=-1;BData[i].T[0]=1;BData[i].T[1]=1;i++;
 BData[i].V[0]=-1;BData[i].V[2]=-1,BData[i].V[1]= 1;BData[i].T[0]=1;BData[i].T[1]=0;i++;
 BDataSize=i;
 for(int x=0;x<i;x+=4)
  for(int y=0;y<4;y++)
	 BData[x+y].C[0]=BData[x+y].C[1]=BData[x+y].C[2]=1;//(x+y*2)/(float)i;

 i=0;
 BYTE k=0;

 for(k=0;k<4*6;k+=4)
 {
 Ind[i]=k+0;i++;
 Ind[i]=k+1;i++;
 Ind[i]=k+2;i++;
 Ind[i]=k+0;i++;
 Ind[i]=k+2;i++;
 Ind[i]=k+3;i++;
 }
 IndC=i;
}

CBasePhisic* CamSync;

//typedef CParticleSet<CSphereSpeedParticleEmiter,CRigidParticleControler,CRigidParticleControler,CRigidParticleControler> PRExplosion;
CBaseParticleSystem *ParExplosion=NULL;

void Rocketcol(phCollisionInfo *Info)
{
    CBasePhisic *Body=NULL;
	CBasePhisic *Body2=NULL;
	if(Info->bodyA->GetCollisionClass()==PH_COLL_ROCKET)
	{Body=Info->bodyA;Body2=Info->bodyB;}else
	if(Info->bodyB->GetCollisionClass()==PH_COLL_ROCKET)
	{Body=Info->bodyB;Body2=Info->bodyA;}
	if(!Body || !Body2)return;

		bVector3f Pos;
		Body->GetPos(Pos);
		//Pos[1]-=2;
		bVector3f Speed;
		Body->GetSpeed(Speed);
		//Body2->AddSpeed(Speed);
		//Body2->AddImpulse(Speed);
        CPHBOX* box=(CPHBOX*)Body->GetExUserData();
		Body->SetExUserData(NULL);
		if(box)
		 delete box;
		CBaseParticleSystem *Particle=ParExplosion->Duplicate(200);//w PRExplosion(85);
		//Pos[1]+=3;
        bVector3f RMIN(2,2,2);
        bVector3f RMAX(5,25,5);
        Particle->GetEmiter()->SetParamVector(PAREM_CENTER,Pos);
        Particle->GetEmiter()->SetParam2Vector(PAREM_RAD,RMIN,RMAX); 
		Particle->GetEmiter()->SetMass(0.01f);
		Particle->GetEmiter()->SetSize(0.3f);
		Particle->SetLifeTime(1000);
		Particle->SetDeathType(2000,PD_COLLAPCE);

		//DWORD ID=Textures::LoadTexture("Data/Textures/flare.bmp");
        Particle->SetTexture(0,"Data/Textures/flare.bmp");//Textures::GetInerTexture(ID));
		Particle->ChargeEmite(0,50);
		DWORD Blow=Sound::DuplicateSound(ExpSound);
		Sound::SetSoundPos(Blow,Pos);
		Sound::PlayOnce(Blow);
		Sound::SetSoundPos(Blow,Pos);
		Info->Sim->AddBlow(Pos,5,2);
		//Particle->Emite(20);
}

CRocket::~CRocket()
{
  CParticle * P=RocketCoreEmiter->GetByID(MyFireBallID);
	if(P)
		P->DoDisabled();
}

bool Fured=false;

void rocketcallback(CBasePhisic * Object,float PPS)
{
  //get direction
	if(Object->LifeTime>2)
	{
		Object->EnableGravity(true);
	//	return;
	}
	bVector3f Speed;
	Object->GetSpeed(Speed);
	bVector3f Pos;
	Object->GetPos(Pos);
    RocketFlareEmiter->GetEmiter()->SetParamVector(PAREM_CENTER,Pos);
    RocketFlareEmiter->GetEmiter()->SetParamVector(PAREM_SPEED,Speed*0.4f);
    bVector3f RAD(FIREBALL_SIZE,FIREBALL_SIZE,FIREBALL_SIZE);
    RocketFlareEmiter->GetEmiter()->SetParam2Vector(PAREM_RAD,RAD*0.6f,RAD*1.5f); 
	RocketFlareEmiter->Emite(10000.0f*PPS);
	CRocket *rok=(CRocket *)Object->GetExUserData();
	if(rok->MyFireBallID)
	{
    CParticle * P=RocketCoreEmiter->GetByID(rok->MyFireBallID);
	if(P)
	{
		P->Position[0]=Pos[0];
		P->Position[1]=Pos[1];
		P->Position[2]=Pos[2];
	}
	}
	Fured=true;
//    RocketCoreEmiter->GetEmiter()->SetParams(Pos,Speed);
}

void PlayercolFloor(phCollisionInfo *Info)
{

  CBasePhisic *Player=NULL;
  CBasePhisic *Body2=NULL;
  if(Info->bodyA->GetCollisionClass()==PH_COLL_PLAYER)
  {Player=Info->bodyA;Body2=Info->bodyB;}else
  if(Info->bodyB->GetCollisionClass()==PH_COLL_PLAYER)
  {Player=Info->bodyB;Body2=Info->bodyA;}
  float ZERO=0;
  Player->SetRotation(bVector3f(0,0,0),ZERO);
  CBasePhisicSim::CanselRotation(Player,bVector3f(0,0,0),Info->PPS);
  bVector3f Speed;
  Player->GetSpeed(Speed);
  if(Speed.Length()>50)
  {
	Speed.Normalize();
 	Speed*=50;		
  }else Speed-=Speed*(Info->PPS*10);
 // Speed[1]=0;

  Player->SetSpeed(Speed);
}

void playercallback(CBasePhisic * Object,float PPS)
{
  //get direction
	float ZERO=0;
  //Body->SetTorque
  CBasePhisicSim::CanselRotation(Object,bVector3f(0,0,0),PPS);
 // Object->SetRotation(bVector3f(0,0,0),ZERO);
}

DWORD LastRRfire=0;
DWORD RocketID=1;

void DLLTYPE FireRocket(bVector3f Pos,bVector3f dir)
{ 
 if(GetThisTime()-LastRRfire<100)return;
 LastRRfire=GetThisTime();
 CRocket * rocket=new CRocket();
 /*bVector3f Pos;
 CamSync->GetPos(Pos);*/
 rocket->ph->SetPos(Pos);
 dir.Normalize();
 dir*=600;//30;
 rocket->ph->SetSpeed(dir);
 DefSim->AddActionObject(rocket->ph);
 rocket->ph->SetActionCallBack(rocketcallback);
 rocket->ph->SetActionType(ACTION_CALLBACK);
 rocket->ph->SetLifeTime(10);

 rocket->ph->GetSpeed(dir);
// RocketCoreEmiter->GetEmiter()->SetParams(Pos,dir);
 //SmartObjectRegistrator::Active=(DWORD)this;
 CParticle *P=NULL;//RocketCoreEmiter->Emite(1);
 if(P) 
 {
	 P->ID=rocket->MyFireBallID=RocketID;
     RocketID++;
     if(RocketID>0xFFFFF) RocketID=1;
 }else rocket->MyFireBallID=0;
}

DWORD LastRBfire=0;

void DLLTYPE FireBullet(bVector3f dir)
{ 
 if(GetThisTime()-LastRBfire<1000)return;
 LastRBfire=GetThisTime();
 CRocket * rocket=new CRocket();
 rocket->ph->SetMass(0.001f);
 rocket->ph->Apply();
 rocket->SetScale(boxSizeY2*0.1f,boxSizeY2*0.1f,boxSizeY2*0.1f);
 rocket->ph->SetCollisionClass(PH_COLL_BULLET);
 bVector3f Pos;
 CamSync->GetPos(Pos);
 rocket->ph->SetPos(Pos);
 dir.Normalize();
 dir*=300;
 rocket->ph->SetSpeed(dir);
 rocket->ph->SetLifeTime(2);
}

void MakeStairs(int NumStairs)
{
	CBasePhisic *ph;
	CPHBOX * Box;
	int i;
	for (i = 0; i < NumStairs; i++)
	{
		ph=DefSim->CreateAnimated();
		ph->AddGeometry()->SetBox(bVector3f(20,0.25f,0.5f));
		ph->UpdateGeometry();
		ph->SetPos(bVector3f(0,groundLevel+i*0.5f,i));
		ph->SetCollisionClass(PH_COLL_STAIRS);
		Box=new CPHBOX(ph);
        Box->SetScale(10,0.25f,0.5f);
	}
	for (i = 0; i < NumStairs; i++)
	{
		ph=DefSim->CreateAnimated();
		ph->AddGeometry()->SetBox(bVector3f(20,0.25f,0.5f));
		ph->UpdateGeometry();
		ph->SetPos(bVector3f(0,groundLevel+(-i+NumStairs)*0.5f,(i+NumStairs)));
		ph->SetCollisionClass(PH_COLL_STAIRS);
		Box=new CPHBOX(ph);
        Box->SetScale(10,0.25f,0.5f);
	}

}

#include "RagDude.h"
#include "Car.h"

void RagHit(phCollisionInfo *Info)
{
    CBasePhisic *Body=NULL;
	if(Info->bodyA->GetCollisionClass()==PH_COLL_RAGDOLL)
		Body=Info->bodyA;else
	if(Info->bodyB->GetCollisionClass()==PH_COLL_RAGDOLL)
		Body=Info->bodyB;
	if(!Body)return;
	CRagDude *RD=(CRagDude *)Body->GetExUserData();
	RD->ChangeCollClass(PH_COLL_OBJECT);
	RD->MakeActive();
	Rocketcol(Info);
}

PBasePhisic DLLTYPE GetCamSync(){ return CamSync;}
void CreateScene()
{
 return;
 //if(!BaseCloudParticle) BaseCloudParticle=CreateParticleSystem("CPointParticleEmiter;PARD;CCloudRotater;PARD;");
 //if(!BaseCloudParticle) BaseCloudParticle=CreateParticleSystem("CSphereSpeedParticleEmiter;PARD;CCloudRotater;PARD;");
 //if(!BaseCloudParticle) BaseCloudParticle=CreateParticleSystem("CSphereParticleEmiter;PARD;CCloudRotater;PARD;");
 if(!BaseCloudParticle) BaseCloudParticle=PARTICLES::LoadParticleSybset("CSphereParticleEmiter;PARD;CCloudRotater;PARD;");
 BaseCloudParticle->Disable();

 if(!ParExplosion) ParExplosion=PARTICLES::LoadParticleSybset("CSphereSpeedParticleEmiter;CRigidParticleControler;CRigidParticleControler;CRigidParticleControler;");
 ParExplosion->Disable();

 RocketCoreEmiter=PARTICLES::LoadParticleSybset("CPointSpeedEmiter;PARD;CCloudRotaterMin;PARD;")->Duplicate(5000);;
 RocketCoreEmiter->SetLifeTime(0xFFFF);
 RocketCoreEmiter->GetEmiter()->SetSize(FIREBALL_SIZE);

 //DWORD ID=Textures::LoadTexture("fireball1.bmp");
 RocketCoreEmiter->SetTexture(0,"fireball1.bmp");//Textures::GetInerTexture(ID));
 RocketCoreEmiter->SetTAmins(8,8,30);

 RocketFlareEmiter=BaseCloudParticle->Duplicate(25000);
 RocketFlareEmiter->SetLifeTime(500);
 RocketFlareEmiter->GetEmiter()->SetSize(2.5f);
	 //DWORD ID=Textures::LoadTexture("Data/Textures/flare.bmp");
 RocketFlareEmiter->SetTexture(0,"Data/Textures/flare.bmp");//Textures::GetInerTexture(ID));



  if(!RocketSound)
  RocketSound=Sound::CreateSound("sound/common/rocketshot");//"Data/Sound/ir_inter.wav");//mshot.wav");
  if(!ExpSound)
  ExpSound=Sound::CreateSound("sound/common/rocketblow");//"Data/Sound/ir_inter.wav");//mshot.wav");
/*	Draw::CreateMaterial("tokaMaterial",
 	                     "Textures/mylogo2.tga",//gbox.bmp",
						 (NULL));
	Draw::CreateShader("tokaBox",
		               "tokaMaterial");
  */
CBasePhisic *ph;
CPHBOX * Box;
 CreateBox();

 CBaseMaterial M;
 //DefSim->CreateMaterial(1,CBaseMaterial(10.1f,1.0f));
 /*GString T1;
 int ii=0;
 for(int x=0;x<5;x++)
 for(int y=0;y<1;y++)
 for(int i=0;i<5;i++)
 {
  CPHBOX * Box=new CPHBOX();
  Box->Move(boxSizeY*x*1,groundLevel + boxSizeY * (i + 0.5f) + 0.0,boxSizeY*y*1-5);
  if(i==14)
  {
   T1.Format("Box%d",ii++);
   Box->SetName(T1);
  }
 }*/
 
 ph=DefSim->CreateAnimated();
 ph->SetCollisionClass(PH_COLL_FLOOR);
 ph->SetMass(1);
 ph->SetInertiaBox(bVector3f(1,1,1));
 ph->AddGeometry()->SetBox(bVector3f(2000,2,2000));
 ph->SetPos(bVector3f(0,-11,0));
 ph->Apply();
 ph->UpdateGeometry();
 
 Box=new CPHBOX(ph);Box->SetScale(1000,1,1000);
 /*CRagDude *RD;
 RD=new CRagDude();
 RD->Make(bVector3f(20,-5,8));
 
 RD=new CRagDude();
 RD->Make(bVector3f(30,0,8));
 RD=new CRagDude();
 RD->Make(bVector3f(40,-5,8));*/

 

 
 MakeStairs(10);

 ph=DefSim->CreateRigid();
 Box=new CPHBOX((CBasePhisic*)1);
 Box->ph=ph;
 ph->SetMass(0.2f);
 ph->SetInertiaBox(bVector3f(2,2,2));
 CBaseGemetry *G=ph->AddGeometry();
 G->SetSphere(2);
 G->SetMaterial(0);
 ph->SetPos(bVector3f(10,10,10));
 ph->Apply();
 ph->SetCollisionClass(PH_COLL_PLAYER);
 ph->UpdateGeometry();
 DefSim->AddActionObject(ph);
 ph->SetActionCallBack(playercallback);
 ph->SetActionType(ACTION_CALLBACK);
 CamSync=ph;

 //CCar *Car=new CCar();
 //Car->MakeCar(bVector3f(20,groundLevel+10,0),bVector3f(0,0,0));
 //CamSync=Car->carBody;

 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_PLAYER  ,RESPONSE_IGNORE);
 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_ROCKET  ,RESPONSE_IGNORE);

 DefSim->SetCollisionAction(PH_COLL_BULLET,PH_COLL_PLAYER  ,RESPONSE_IGNORE);
 DefSim->SetCollisionAction(PH_COLL_BULLET,PH_COLL_BULLET  ,RESPONSE_IGNORE);

 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_FLOOR   ,RESPONSE_IMPULSE_CALLBACK,Rocketcol);
 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_OBJECT  ,RESPONSE_IMPULSE_CALLBACK,Rocketcol);
 DefSim->SetCollisionAction(PH_COLL_BULLET,PH_COLL_FLOOR   ,RESPONSE_IMPULSE);
 DefSim->SetCollisionAction(PH_COLL_BULLET,PH_COLL_OBJECT  ,RESPONSE_IMPULSE);
 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_PARTICLE,RESPONSE_IGNORE);
 DefSim->SetCollisionAction(PH_COLL_ROCKET,PH_COLL_RAGDOLL ,RESPONSE_IMPULSE_CALLBACK,RagHit);

 DefSim->SetCollisionAction(PH_COLL_PLAYER,PH_COLL_STAIRS,RESPONSE_IMPULSE_CALLBACK,PlayercolFloor);
 DefSim->SetCollisionAction(PH_COLL_PLAYER,PH_COLL_FLOOR ,RESPONSE_IMPULSE_CALLBACK,PlayercolFloor);
 DefSim->SetCollisionAction(PH_COLL_PLAYER,PH_COLL_OBJECT,RESPONSE_IMPULSE_CALLBACK,PlayercolFloor);

 extern DWORD SYS_PHISIC;
 Print2Console(SYS_PHISIC,"Level loaded");
}

