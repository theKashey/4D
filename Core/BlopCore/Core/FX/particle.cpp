/*===========================================================================
    Заголовок: Particles.cpp
    Автор: Kashey
    Информация:	 Particles

===========================================================================*/
#include "ProjectSpace.h"
#include "ProjectGraphic.h"
#include "particles.h"
#include "../Parser/parser.h"
#include "../Parser/Particles/ParticleShader.h"

void InitParticleSybset();

bool ParticleSyssetInited=false;
DWORD SYS_PARTICLES=0;

//extern InitParticleSybset();

SmartVector<CParticleEmiter*> ParticlesSybset;
typedef CBSPDividorLookUpArray<char,int> _PARTICLENAMEBSP;
_PARTICLENAMEBSP  InParticleNameBSP;
_PARTICLENAMEBSP  CallBackNameBSP;
_PARTICLENAMEBSP  CallBackNameParamsBSP;
SmartVector<float> ARParticlesCallBackParams;
SmartVector<ExParticleControlerCallback> ARParticlesCallBack;

int RegisterParticleSybset(CParticleEmiter *Set,LPCSTR SetName)
{
	ParticlesSybset.push_back(Set);
	GString N=SetName;
	N.MakeLower();
	InParticleNameBSP.PushAs(ParticlesSybset.size(),N.c_str(),N.length());
	return ParticlesSybset.size();
}

CParticleEmiter *GetEmiter(LPCSTR Name)
{
    GString T=Name;
	T.MakeLower();
	DWORD ID=InParticleNameBSP.GetIDFor(T.c_str(),T.length());
	if(ID)
	{
		return ParticlesSybset[ID-1]->Duplicate();
	}
	return new   CParticleEmiter;
}

PBaseParticleSystem LoadSybSet(CParticleShader *sh,DWORD Count)
{
	CBaseParticleSet *New=new CBaseParticleSet(Count);
	New->SetEmiter    (GetEmiter(sh->GetEmiterName    ()));
	New->SetController(GetEmiter(sh->GetControllerName()));
	New->SetRegister  (GetEmiter(sh->GetRegisterName  ()));
	New->SetKiller    (GetEmiter(sh->GetKillerName    ()));
	
	return New;
}

PBaseParticleSystem PARTICLES::LoadParticleSybset(LPCSTR Name,DWORD OverCount)
{
	if(!ParticleSyssetInited)
	{
		InitParticleSybset();
		ParticleSyssetInited=true;
	}
	if(!SYS_PARTICLES)
		RegisterNewConsoleClass(SYS_PARTICLES,"PARTICLES",-1);

	CParticleShader *sh=GetParticleShader(Name);
	if(!sh)
	{
		Error2Console(SYS_PARTICLES,"no shader %s",Name);
		return NULL;
	}
	else
	{
	   PBaseParticleSystem system=LoadSybSet(sh,sh->GetDefCreteCount());
	   if(!system)
	   {
		Error2Console(SYS_PARTICLES,"no such sybset %s-%s",Name,sh->GetSybset());
		return NULL;
	   }
	   else
	   {
		   if(!system->AssertShader((DWORD)sh))
		   {
		   		Error2Console(SYS_PARTICLES,"sybset %s - asserting failed",Name);
				system->Block();
         		return NULL;
		   }
		   system->Add2List();
		   return system;
	   }
	}
}

int PARTICLES::RegisterCallBack(LPCSTR Name,ExParticleControlerCallback CB)
{
	ARParticlesCallBack.push_back(CB);
	GString N=Name;
	N.MakeLower();
	CallBackNameBSP.PushAs(ARParticlesCallBack.size(),N.c_str(),N.length());
	return ARParticlesCallBack.size();
}
int PARTICLES::RegisterCallBackParam(LPCSTR Name,float Param)
{
	ARParticlesCallBackParams.push_back(Param);
	GString N=Name;
	N.MakeLower();
	CallBackNameParamsBSP.PushAs(ARParticlesCallBack.size(),N.c_str(),N.length());
	return ARParticlesCallBackParams.size();
}

ExParticleControlerCallback GetRegisteredCallBack(LPCSTR Name)
{
    GString T=Name;
	T.MakeLower();
	//look in tree for this name and return stored ID
	DWORD ID=CallBackNameBSP.GetIDFor(T.c_str(),T.length());
	if(ID)
	{
		return ARParticlesCallBack[ID-1];
	}
	else return 0;
}

float GetRegisteredCallBackParam(LPCSTR Name)
{
    GString T=Name;
	T.MakeLower();
	//look in tree for this name and return stored ID
	DWORD ID=CallBackNameParamsBSP.GetIDFor(T.c_str(),T.length());
	if(ID)
	{
		return ARParticlesCallBackParams[ID-1];
	}
	else return 0;
}

void PARTICLES::Prepare(float Time)
{
	 if(Time<0)Time=GetCounter(DC_MAIN_CYCLE);
	 ProjectParticles->Charge();
	 ProjectParticles->Tick(Time);
	 ProjectParticles->CheckDead();
}

void PARTICLES::DO()
{
	 ProjectParticles->Draw();
}

void AssertEmiterParams(CParticleEmiter *P,CParticleParams *PP)
{
	 if(!P || !PP) return;
	CParticleParams::PPLE *Param;
    PP->ResetGet();
	while(Param=PP->GetNextParam())
	 switch(Param->Type)
	 {
	  case 0:P->SetParamf      (Param->ID,Param->GetFloat());break;
	  case 1:P->SetParamVector (Param->ID,Param->GetVector());break;
	  case 2:P->SetParam2Vector(Param->ID,Param->GetVector(),Param->GetVector(1));break;
	}
}

bool CBaseParticleSet::AssertShader        (DWORD ID)
{
  CParticleShader *sh=(CParticleShader *)ID;
  CBaseParticleSystem::SetTexture(0,sh->GetMainTextureName());
 // SetParticlesNum(sh->GetDefCreteCount());
  fBlendType=sh->GetBlendingType ();
  fRotatable=sh->GetRotatableFlag()>0;
  EmiteType=sh->GetChargeType();
  EmiteCharge=sh->GetChargeCount();
  SetColored(sh->GetColorFlag()>0);
  SetColors(sh->Color);
  DefColorParticle=sh->PColor;
  AdvanceEmite=sh->AdvanceEmite;
  //Emiter
   AssertEmiterParams(GetEmiter    (),sh->GetEmiterParams());
  //Controller
   AssertEmiterParams(GetController(),sh->GetControllerParams());
  //Register
   AssertEmiterParams(GetRegister  (),sh->GetRegisterParams());
  //Killer 
   AssertEmiterParams(GetKiller    (),sh->GetKillerParams());
   SystemCenterUse=sh->GetCenterUsage();

   ExCallBack=GetRegisteredCallBack(sh->GetCallBack());
   ExCallBackTag=GetRegisteredCallBackParam(sh->GetCallBackParam());
  return true;
}



void CBaseParticleSystem::SetTexture(DWORD CID,LPCSTR TID)
{
	DWORD ID=Textures::LoadTexture(TID);
	SetTexture(CID,Textures::GetInerTexture(ID));
}

PBaseParticleSystem ProjectParticles=NULL;

void CBaseParticleSet::SetParticlesNum(DWORD NewCount)
{
	//Particles._resize(NewCount);
	Particles.SetSize(NewCount);
}
void CBaseParticleSet::UpdateParticlePos(DWORD ID,float x,float y,float z,float size)
{
	Particles[ID].Position[0]=x;
	Particles[ID].Position[1]=y;
	Particles[ID].Position[2]=z;
	Particles[ID].Size       =size;
}

float LastTimeC=0;

CParticle * CBaseParticleSet::Emite()         
	{
		switch(EmiteType)
		{
		 case  PD_EMITE_UNDEF:return NULL;
		 case  PD_EMITE_USER :return NULL;
		 case  PD_EMITE_ONCE :
			 if(OnceEmited)return NULL;
			 //Print2Console(0,"Emiting %f particles",EmiteCharge);
			 OnceEmited=true;
			 return Emite(EmiteCharge);
		 case  PD_EMITE_FRAME: 
             float EM=EmiteCharge*LastTimeC;
             DWORD dEM=EM;
             EmiteLasts+=EM-dEM;
             if(EmiteLasts>1)
             {EmiteLasts-=1;
              dEM++;
             }
             return Emite(dEM);
		}
		return NULL;
	}
	

	CParticle * CBaseParticleSet::GetByID(const DWORD ID)
	{
		if(!ID) return NULL;
		const ParticleList::iterator endit=Particles.end();
	    for(ParticleList::iterator it=Particles.begin(); it!=endit;++it)
		  if(it->ID==ID) return it;
		return NULL;
	}

void  CBaseParticleSet::SetTicable(bool Mode){Ticable=Mode;}
bool  CBaseParticleSet::IsEnabled ()         {return Enabled & !inBlock;}
bool  CBaseParticleSet::IsDrawable()         {return Drawable;}

void CBaseParticleSet::Add2List()
{
 ProjectParticles->AddSystem(this);
}

CBaseParticleSet::CBaseParticleSet(DWORD _MaxParticles)
{
	MyEmiter       =NULL;
	MyDrawer       =NULL;
	MyController   =NULL;
	MyRegister     =NULL;
	MyKiller       =NULL;
    int OutSize=_MaxParticles*4;
	MaxParticles=OutSize/4;
	AdvanceEmite=true;
//	OutBuf=new CParticleOut[OutSize];
//	OutCount=0;
	Enabled=Drawable=Ticable=true;
	Particles.SetSize(MaxParticles);
	Particles.clear();
	DeadParticles.SetSize(MaxParticles);
	DeadParticles.clear();
	DeathType=PD_CLEAR;
	LastTime=0;
	EmiteType=PD_EMITE_UNDEF;
	EmiteCharge=1;
	SystemCenterUse=PD_CENTER_NONE;
	DefColor.Set(1,1,1,1);

    SetTexture(0,0);
	SetTAmins (1,1,0);
	fBlendType=BLEND_ADD;
	fRotatable=false;
	fColored  =0;
	DefColor.Set(1,1,1,1);
	DefColorParticle.Set(1,1,1,1);
    EmiteLasts=0;
	ExCallBack=0;
	ExCallBackTag=0;

	CullElement=CULL::CreateCullElement();

	//OnceEmitenum   =10;
    //OnceSingle     =false;
	//OncePerFrame   =true;
	OnceEmited     =false;
	inBlock        =false;
}

CBaseParticleSet::~CBaseParticleSet(void)
{
	if(MyEmiter    ) delete MyEmiter;
	if(MyDrawer    ) delete MyDrawer;
	if(MyController) delete MyController;
	if(MyRegister  ) delete MyRegister;
	if(MyKiller    ) delete MyKiller;
	ProjectParticles->RemoveSystem(this);
//	delete []OutBuf;
}

bool CBaseParticleSet::IsFree              ()   
{
	return !Particles.size() && !DeadParticles.size();
}

void CBaseParticleSet::SetTexture(DWORD CID,DWORD TID)
{
	Texture=TID;
}
void CBaseParticleSet::SetTAmins (DWORD X,DWORD Y,float speed)
{
	TAnimsX=X;
	TAminsY=Y;
	AnimationSpeed=speed;
}

CBaseParticleSystem* CBaseParticleSet::Duplicate(DWORD Count)
{
	CBaseParticleSet *New=new CBaseParticleSet(Count);
	New->SetEmiter    (MyEmiter->Duplicate());
	New->SetRegister  (MyRegister->Duplicate());
	New->SetController(MyController->Duplicate());
	New->SetKiller    (MyKiller->Duplicate());
	New->SetDrawer    (MyDrawer->Duplicate());
	return New;
}

void CBaseParticleSet::Disable(){Enabled=false;};
void CBaseParticleSet::Enable(){Enabled=true;};

void CBaseParticleSet::SetSystemCenter     (const float *Pos)          { SystemCenter=Pos;}
void CBaseParticleSet::SetSystemOrientation(const float* Rot)          { SystemRot   =Rot;}
void CBaseParticleSet::SetSystemVelosity   (const float* Vel)          { SystemVel   =Vel;}
void CBaseParticleSet::UseSystemCenter     (const int    Type)         { SystemCenterUse=Type;}
void CBaseParticleSet::SetCenterParticles  (ParticleList *Particles)   { CenterParticles.CopyFrom(*Particles);}
void CBaseParticleSet::SetRotatable        (bool Rotatable)            { fRotatable=Rotatable;}
void CBaseParticleSet::SetBlendType        (DWORD BlendType)           { fBlendType=BlendType;}

void CBaseParticleSet::SetColored          (bool Colored)              { fColored=Colored;}
void CBaseParticleSet::SetColors           (const float *C)            { DefColor=C;}


DWORD ParticlesFrameOCCFreq=10;

bool CBaseParticleSet::IsVisibled()
{
	CullElement->SetPosition(occPosition);
	CullElement->SetRad(occRad);   
	CullElement->SetForm    (CULL::cullform_box);
	CullElement->SetFrameOcc( ParticlesFrameOCCFreq);
	return CullElement->IsVisibled();
}

void  CBaseParticleSet::AfterTick(const float TC)
{
}

void CBaseParticleSet::SetLifeTime(WORD Time){TimeOut=Time;}
	   

void CBaseParticleSet::ChargeEmite(DWORD DeltaTime,int Num)
{
	CBaseParticleEmiteCharge Charge;
	Charge.ChargeTime=Time::Time()+DeltaTime;
	Charge.Num       =Num;
	Charges.push_back(Charge);
}

void CBaseParticleSet::BeforeTick(float TimeCompress)
{
    DWORD Time=Time::Time();
	float One2Time=1.0f/(float)TimeToDead;
	if(DeathType==PD_CLEAR)return;
	ParticleList::iterator end=DeadParticles.end();
	for(ParticleList::iterator it=DeadParticles.begin(); it!=end;it++)
	{
      switch(DeathType)
	  {
	   case PD_COLLAPCE: if((*it).Size>0)
							 (*it).Size*=(TimeToDead-((*it).LifeTime))*One2Time;
						//	 (*it).Size-=0.001*TimeCompress;
						 else (*it).DoDisabled();
	  }
	}

}

void CBaseParticleSet::Tick(float TimeCompress)
{
	static CDefRotated DefRotated;
//	const float TimeCompress=fTimeCompress;
	if(!Ticable)return;
	BeforeTick(TimeCompress);

	if(!Particles.size() && !DeadParticles.size()) return;

    const ParticleList::iterator endit=Particles.end();
	MyController->Move(Particles.begin(),Particles.size(),TimeCompress);

	if(!ExCallBack)
	for(ParticleList::iterator it=Particles.begin(); it!=endit;++it)
	    if(ExCallBack(ExCallBackTag,it,TimeCompress))(it)->DoDisabled();
	
	MyController->Move(DeadParticles.begin(),DeadParticles.size(),TimeCompress);	
    if(!MyController->Rotater() && fRotatable)
	{
     const ParticleList::iterator endit=Particles.end();
	 DefRotated.Move(Particles.begin(),Particles.size(),TimeCompress);
	 DefRotated.Move(DeadParticles.begin(),DeadParticles.size(),TimeCompress);	
	}
	AfterTick(TimeCompress);
}

void CBaseParticleSet::UnchargeEmite()
{
    DWORD Time=Time::Time();
	for(int i=0;i<Charges.size();)
		if(Time>Charges[i].ChargeTime)
		{
			Emite(Charges[i].Num);
			Charges.erase(i);
		}else ++i;
}


void CBaseParticleSet::SetDeathType(DWORD MaxDeathTime,DWORD _DeathType)
{
	TimeToDead=MaxDeathTime;
	DeathType =_DeathType;
}

#define PARTICLE_EMITE_BLOCK_SIZE ((BYTE)32)

CParticle NewParticle[PARTICLE_EMITE_BLOCK_SIZE];

CParticle * CBaseParticleSet::Emite(WORD Num)
{
      if(!Num)return NULL;
	  const float oneNum=1.0f/Num; 	  
	  ZeroMemory(NewParticle,sizeof(NewParticle));
	  DWORD _Time=Time::Time();
	  WORD Emited=0;
	  DWORD Started=Particles.size();
	  CParticle *EmitedP=NULL;
	  DWORD iN=0;
	  for(WORD i=0;i<Num;i+=Emited)
	  {
	    BYTE thisMax=PARTICLE_EMITE_BLOCK_SIZE;
	    if(i+PARTICLE_EMITE_BLOCK_SIZE>Num) thisMax=Num-i;
	    Emited=MyEmiter->Emite(NewParticle,thisMax);
	    if(Emited>0) Enabled=true;
	    if(!Emited)return EmitedP;
	    const bool DSET=MyEmiter->GetDeltaLife()>0;
	    for(int j=0;j<Emited;++j)
	    {
	      if(Particles.size()>MaxParticles-5) return EmitedP;
		  NewParticle[j].LifeTime=DSET?(-MyEmiter->GetDeltaLife()*random()):0;//_Time;
		  for(int k=0;k<3;++k)NewParticle[j].Color[k]=DefColorParticle[k]*0xFF;
          NewParticle[j].Color[3]=1;
	      //if(MyRegister->Register(NewParticle[j],Particles.size()))
		  if(AdvanceEmite)
            MyController->Move(&NewParticle[j],1,LastTimeC*iN*oneNum);
          iN++;
	      Particles.push_back(NewParticle[j]);
		  if(EmitedP==NULL)
			 EmitedP=&Particles[Started];
		}
	  }
	  return EmitedP;
}



//--------------------------------------------------------------------------------------


void CBaseParticleSystem::AddSystem   (CBaseParticleSystem* Sys)
{
	Layers.push_back(Sys);
}
void CBaseParticleSystem::RemoveSystem(CBaseParticleSystem* Sys)
{Layers.eraseItem(Sys);}


void CBaseParticleSystem::Tick        (float TimeC)
{
	LastTimeC=TimeC;
	for(ParticlePack::iterator it=Layers.begin();it!=Layers.end();++it)
		if(*it)
		if((*it)->IsEnabled())
			(*it)->Tick(TimeC);
}
void CBaseParticleSystem::Charge      ()
{
	for(ParticlePack::iterator it=Layers.begin();it!=Layers.end();++it)
		if(*it)
		if((*it)->IsEnabled() )
		{
			(*it)->UnchargeEmite();
			(*it)->Emite();
		}
}

void CBaseParticleSystem::CheckDead   ()
{

	for(ParticlePack::iterator it=Layers.begin(); it!=Layers.end();++it)
		if(*it)
		if((*it)->IsEnabled() || !(*it)->IsFree())
			(*it)->CheckDead();
}

void CBaseParticleSet::KillParticle(CParticle &Particle,DWORD ID,bool KillAtOnce,bool DeadAllready)
{
	if(DeathType==PD_CLEAR) KillAtOnce=true;
	if(!DeadAllready)
	{
	if(KillAtOnce )
	{
	 MyKiller->Kill(Particle);
	 Particles.erase(ID);
	}
	else
	{
		Particle.DoEnabled();
		Particle.LifeTime=0;//GetThisTime();
		DeadParticles.push_back(Particle);
		Particles.erase(ID);
	}
	}
	else
	{
	 MyKiller->Kill(Particle);
	 DeadParticles.erase(ID);
	}
}


void CBaseParticleSystem::Draw()
{
	for(ParticlePack::iterator it=Layers.begin(); it!=Layers.end();++it)
		if(*it)
		if((*it)->IsEnabled())
			(*it)->Draw();
	//drawself

}

void CBaseParticleSet::CheckDead()
{
	CBaseParticleSystem::CheckDead();
	const DWORD _Time=Time::Time();
	if(!LastTime)LastTime=_Time;
	DeltaTime=(_Time-LastTime);
	const ParticleList::iterator end1=Particles.end();
	if(Particles.size()>0)
    for(ParticleList::iterator it1=Particles.begin(); it1!=end1;++it1)
	{
        (*it1).LifeTime+=DeltaTime;
		if((*it1).LifeTime>TimeOut) ((*it1)).DoDisabled();
	}
	LastTime=_Time;
	TimeOut=MyEmiter->GetLife(); 
	int i=0;
	const ParticleList::iterator end2=Particles.end();
	if(Particles.size()>0)
	for(ParticleList::iterator it2=Particles.begin(); it2!=Particles.end();)
	{
		//MyKiller.TryKill(*it2);
		if(((*it2)).IsDisabled()){ KillParticle(*it2,i);}
		else	                 {++i;++it2;}
		if(Particles.size()==0)break; //_!_
	}
	const ParticleList::iterator end3=DeadParticles.end();
	if(DeadParticles.size()>0)
	for(ParticleList::iterator it3=DeadParticles.begin(); it3!=end3;)
	{
		(*it3).LifeTime+=DeltaTime;
		if((*it3).LifeTime>TimeToDead)((*it3)).DoDisabled(); 
		if(((*it3)).IsDisabled())KillParticle(*it3,i,true,true);
		else       		{++i;++it3;}
		if(DeadParticles.size()==0)break;
	}

}

void CBaseParticleSet::Draw()
{
	CBaseParticleSystem::Draw();
	MyDrawer->SetTA(Texture,TAnimsX,TAminsY,AnimationSpeed);
	MyDrawer->SetParams(fRotatable,fBlendType,fColored,DefColor);
	MyDrawer->SetTRROT((SystemCenterUse &PD_CENTER_TR)?SystemCenter:NULL,(SystemCenterUse &PD_CENTER_RT)?SystemRot:NULL);
	if(Particles.size())
	 MyDrawer->Draw(Particles.begin(),Particles.size());
	if(DeadParticles.size())
  	 MyDrawer->Draw(DeadParticles.begin(),DeadParticles.size());
	if(CenterParticles.size())
     MyDrawer->Draw(CenterParticles.begin(),CenterParticles.size());
}