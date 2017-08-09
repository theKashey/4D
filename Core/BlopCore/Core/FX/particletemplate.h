/*#ifndef _PARTICLE_TEMPLATE_H
#define _PARTICLE_TEMPLATE_H

#define PARTICLE_EMITE_BLOCK_SIZE      16
extern float LastTimeC;


class  CParticleSet:public CBaseParticleSet
{
public:

	virtual CBaseParticleSystem* Duplicate(DWORD Count){return new ThisSetClass(Count);}

    CParticleEmiter*   MyEmiter;
	CParticleEmiter*   MyDrawer;
	CParticleEmiter*   MyController;
	CParticleEmiter*   MyRegister;
	CParticleEmiter*   MyKiller;

	CParticleEmiter* GetEmiter    (){return MyEmiter;}
	CParticleEmiter* GetRegister  (){return MyRegister;}
	CParticleEmiter* GetController(){return MyController;}
	CParticleEmiter* GetKiller    (){return MyKiller;}

	CParticleSet(int _MaxParticles):CBaseParticleSet(_MaxParticles)
	{ LastTime=0;}

    CParticle *Emite(WORD Num)
    {
	  if(!Num)return NULL;
	  const float oneNum=1.0f/Num;
  	  CParticle NewParticle  [PARTICLE_EMITE_BLOCK_SIZE];
	  ZeroMemory(NewParticle,sizeof(NewParticle));
	  DWORD _Time=GetThisTime();
	  WORD Emited=0;
	  DWORD Started=Particles.size();
	  CParticle *EmitedP=NULL;
	  DWORD iN=0;
	  for(WORD i=0;i<Num;i+=Emited)
	  {
	   BYTE thisMax=PARTICLE_EMITE_BLOCK_SIZE;
	   if(i+PARTICLE_EMITE_BLOCK_SIZE>Num) thisMax=Num-i;
	   Emited=MyEmiter.Emite(NewParticle,thisMax);
	   if(Emited>0) Enabled=true;
	   if(!Emited)return EmitedP;
	   const bool DSET=MyEmiter.GetDeltaLife()>0;
	   for(int j=0;j<Emited;++j)
	   {
	    if(Particles.size()>MaxParticles-5) return EmitedP;
		NewParticle[j].LifeTime=DSET?(-MyEmiter.GetDeltaLife()*random()):0;//_Time;
		for(int k=0;k<3;++k)NewParticle[j].Color[k]=DefColorParticle[k]*0xFF;
        NewParticle[j].Color[3]=1;
	    if(MyRegister.Register(NewParticle[j],Particles.size()))
		if(AdvanceEmite)
        MyController.Move(NewParticle[j],LastTimeC*iN*oneNum);
        iN++;
	     Particles.push_back(NewParticle[j]);
		 if(EmitedP==NULL)
			 EmitedP=&Particles[Started];
	   }
	  }
	  return EmitedP;
	}



//#define IsDisabled(p) (p.Flags & PARTICLE_DISABLED)
//#define DoDisabled(p) p.Flags|=PARTICLE_DISABLED;
#define pAdd3x(x,y)  x[0]+=y[0]*TimeCompress;x[1]+=y[1]*TimeCompress;x[2]+=y[2]*TimeCompress;

 void Tick (float fTimeCompress)
 {
	const float TimeCompress=fTimeCompress;
	if(!Ticable)return;
	CBaseParticleSet::Tick(TimeCompress);
	if(!Particles.size() && !DeadParticles.size()) return;
    const ParticleList::iterator endit=Particles.end();
	if(!ExCallBack)
	for(ParticleList::iterator it=Particles.begin(); it!=endit;++it)
		MyController.Move(*it,TimeCompress);
	else
	for(ParticleList::iterator it=Particles.begin(); it!=endit;++it)
	{	MyController.Move(*it,TimeCompress);
	    if(ExCallBack(ExCallBackTag,it,TimeCompress))(it)->DoDisabled();
	}

    const ParticleList::iterator endit1=DeadParticles.end();
	for(ParticleList::iterator it1=DeadParticles.begin(); it1!=endit1;++it1)
		MyController.Move(*it1,TimeCompress);	

	if(!MyController.Rotater() && fRotatable)
	{
    const ParticleList::iterator endit=Particles.end();
	for(ParticleList::iterator it=Particles.begin(); it!=endit;++it)
		CDefRotated::Move(*it,TimeCompress);
    const ParticleList::iterator endit1=DeadParticles.end();
	for(ParticleList::iterator it1=DeadParticles.begin(); it1!=endit1;++it1)
		CDefRotated::Move(*it1,TimeCompress);	

	}
	CBaseParticleSet::AfterTick(TimeCompress);

 }

 void CheckDead()
 {
	TimeOut=MyEmiter.GetLife(); 
    CBaseParticleSet::CheckDead();
	int i=0;
	const ParticleList::iterator end2=Particles.end();
	if(Particles.size()>0)
	for(ParticleList::iterator it2=Particles.begin(); it2!=Particles.end();)
	{
		MyKiller.TryKill(*it2);
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
	//if(!Particles.size() && !DeadParticles.size()) Enabled=false;
 }

void KillParticle(CParticle &Particle,DWORD ID,bool KillAtOnce=false,bool DeadAllready=false)
{
	if(DeathType==PD_CLEAR) KillAtOnce=true;
	if(!DeadAllready)
	{
	if(KillAtOnce )
	{
	 MyKiller.Kill(Particle);
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
	 MyKiller.Kill(Particle);
	 DeadParticles.erase(ID);
	}
}

void Draw()
{
	MyDrawer.SetTA(Texture,TAnimsX,TAminsY,AnimationSpeed);
	MyDrawer.SetParams(fRotatable,fBlendType,fColored,DefColor);
	MyDrawer.SetTRROT((SystemCenterUse &PD_CENTER_TR)?SystemCenter:NULL,(SystemCenterUse &PD_CENTER_RT)?SystemRot:NULL);
	if(Particles.size())
	 MyDrawer.Draw(Particles.begin(),Particles.size());
	if(DeadParticles.size())
  	 MyDrawer.Draw(DeadParticles.begin(),DeadParticles.size());
	if(CenterParticles.size())
     MyDrawer.Draw(CenterParticles.begin(),CenterParticles.size());
}
};

#endif
*/
