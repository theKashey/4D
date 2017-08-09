#include "StdAfx.h"
#include "Core/FX/Particles.h"
#include "particleerosion.h"

#define RAIN_DOWN_FORCE 0.01//0.0001

float fastlen(float *L)
{
 return sqrt(L[0]*L[0]*20+/*L[1]*L[1]*0.05+*/L[2]*L[2]*20);
}

float DestS(float *L)
{
	if(fabs(L[0])<0.1 && fabs(L[1])<0.1) return -fabs(L[0])-fabs(L[2]);
	return fabs(L[0])+fabs(L[2]);
}

CParticleErosion::CParticleErosion(void)
{
	FacX=25/128.0;
	FacY=FacX;
	FacZ=FacY;
	Width=128;
	Height=128;
	UsedParticles=0;
	NumParticles=5000;
	WaterParticles=new CWaterParticle[NumParticles];
	Land          =new CEroHeight    [Width*Height];
	ZeroMemory(WaterParticles,sizeof(CWaterParticle)*NumParticles);
	ZeroMemory(Land          ,sizeof(CEroHeight    )*Width*Height);

    PS            =new CParticleSet();

	VOut  =new CEroOut   [Width*Height];
	ZeroMemory(VOut,sizeof(CEroOut)*Width*Height);
	Index=new DWORD[Width*Height*6];
	IndexSize=Width*Height*6;
	DWORD i=0;
	for(int y=0;y<Height-1;y++)
	 for(int x=0;x<Width-1;x++)
	 {
		 Index[i++]=(x+0)+(y+0)*Width;
		 Index[i++]=(x+0)+(y+1)*Width;
		 Index[i++]=(x+1)+(y+1)*Width;

		 Index[i++]=(x+1)+(y+1)*Width;
		 Index[i++]=(x+1)+(y+0)*Width;
		 Index[i++]=(x+0)+(y+0)*Width;
	 }
    IndexSize=i;
}

CParticleErosion::~CParticleErosion(void)
{
	delete []WaterParticles;
	delete []Land;
	delete []VOut;
	delete []Index;
}

void CParticleErosion::SpawnWater(DWORD Count)
{
	DWORD WH=Width*Height;
	DWORD W2=2;//(Width-10)/2;
	DWORD H2=2;//(Height-10)/2;
  for(int i=0;i<Count;i++)
  {
	DWORD x=W2+roundrandom(Width-4);//Width-1);
	DWORD y=H2+roundrandom(Height-4);//Height-1);
	int UP=0;
	for(int j=0;j<NumParticles;j++)if(!WaterParticles[j].Enabled) break;
	if(j==NumParticles) break;
	if(j>UsedParticles) UsedParticles=j;
	ZeroMemory(&WaterParticles[j],sizeof(CWaterParticle));
	WaterParticles[j].X=x+0.5;
	WaterParticles[j].Y=y+0.5;
	WaterParticles[j].cX=x;
	WaterParticles[j].cY=y;
	WaterParticles[j].WH=x+y*Width;
	WaterParticles[j].Z=Land[WaterParticles[j].WH].Height;
	WaterParticles[j].Weight=1;
	WaterParticles[j].Enabled=true;
  //  UsedParticles++;
  }
}

DWORD CParticleErosion::Tick(float Time)
{
	for(int i=0;i<Time;i++)
  	{
	// SpawnWater(100);
     MoveWater();
	}
	return 0;
}

void CParticleErosion::MoveWater()
{
   CWaterParticle *Particle;
   CEroHeight     *LP;
   DWORD WH=Width*Height;
   //remap
   LP=&Land[0];
   for(int i=0;i<WH;i++,LP++)
   {
	   if(i==2080)
		   i=2080;
	   LP->NumParticles=0;
       Particle=&WaterParticles[0];
	   for(int j=0;j<UsedParticles;j++,Particle++)
		   if(Particle->Enabled && Particle->WH==i)LP->NumParticles++;
   }

   //move
   Particle=&WaterParticles[0];
   for(int i=0;i<UsedParticles;i++,Particle++)
   if(Particle->Enabled)
   {
	   //check collisions
	   float OldH;
	   OldH=Land[Particle->WH].Height;
	   if(Particle->Z>Land[Particle->WH].Height) Particle->Speed[1]-=0.01;
	   if(Particle->Z<=Land[Particle->WH].Height)
	   {
		   float Delta=DestS(Particle->Speed.data)*(Particle->Weight+Particle->Mud)*RAIN_DOWN_FORCE;
		   Land[Particle->WH].Height-=Delta;
		   Particle->Z=Land[Particle->WH].Height;
		   Particle->Mud+=Delta;
		   Particle->Speed[1]=0;
		   if(Land[Particle->WH].NumParticles>1)
		   {
             Particle->Speed[0]+=randomM()*0.01;
			 Particle->Speed[2]+=randomM()*0.01;
		   }     
		   if(Particle->Speed[0]>1)Particle->Speed[0]=1;
		   if(Particle->Speed[2]>1)Particle->Speed[2]=1;
	   }
	   //add speed
	   Particle->X+=Particle->Speed.data[0];
	   Particle->Y+=Particle->Speed.data[2];
	   Particle->Z+=Particle->Speed.data[1];
	   Particle->cX=floor(Particle->X);
	   Particle->cY=floor(Particle->Y);
	   Particle->WH=Particle->cX+Particle->cY*Width;
	   if(Particle->cX<1 || Particle->cX>Width-1 || Particle->cY<1 || Particle->cY>Height-1)Particle->Enabled=false;
	   if(!Particle->Enabled)continue;
	   float NewH=Land[Particle->WH].Height;
	   /*if(NewH==OldH){Particle->Speed.data[0]*=0.99; 
	                  Particle->Speed.data[1]*=0.99;
					  Particle->Speed.data[2]*=0.99;
	                 }else
*/
	   if(NewH>OldH) {Particle->Speed.data[0]-=Particle->Speed.data[0]*0.9;
                      Particle->Speed.data[2]-=Particle->Speed.data[2]*0.9;
					 }else
       if(    1    ) {Particle->Speed.data[0]+=Particle->Speed.data[0]*0.9;
                      Particle->Speed.data[2]+=Particle->Speed.data[2]*0.9;
	                 } 

   }
}

LPCVOID  CParticleErosion::GetOutData()
{
	float shiftx=-FacX*Width/2.0;
	float shifty=-FacY*Height/2.0;
    for(int y=0;y<Height;y++)
	 for(int x=0;x<Width;x++)
	 {
		 DWORD LW=y*Width;
		 VOut[x+LW].Pos[0]=shiftx+x*FacX;
		 VOut[x+LW].Pos[1]=(Land[x+LW].Height)*FacY;
		 VOut[x+LW].Pos[2]=shifty+y*FacZ;
		 VOut[x+LW].Color[0]=VOut[x+LW].Color[1]=VOut[x+LW].Color[2]=(x%2 +y%2==1)?0xFF:0;
	 }

	CWaterParticle *Particle;
    Particle=&WaterParticles[0];
	PS->SetParticlesNum(UsedParticles);
    for(int i=0;i<UsedParticles;i++,Particle++)
	{
		PS->UpdateParticlePos(i,shiftx+Particle->X*FacX,Particle->Z*FacZ,shifty+Particle->Y*FacY,1);
	}
	return (LPCVOID)&VOut[0].Pos[0];
}

LPCVOID  CParticleErosion::GetIndex()      { return (LPCVOID)Index;}
DWORD    CParticleErosion::GetIndexCount() { return IndexSize;}
WORD     CParticleErosion::GetDataStride() { return sizeof(CEroOut);}
BYTE     CParticleErosion::GetColorOff  () { CEroOut Pos;return (DWORD)&Pos.Color-(DWORD)&Pos;}

void CParticleErosion::StartAssert(LPCSTR Mode){}
DWORD CParticleErosion::EndAssert(){return 0;}
bool  CParticleErosion::AssertParamF(LPCSTR Name,float Val)
{
	if(strcmp(Name,"Emite")==0) SpawnWater(Val);
	return true;
}


/*CParticleErosion* DLLTYPE GetErrosion()
{
	return new CParticleErosion();
}*/

/*
void CErosionManager::MoveWater()
{
   int i=0;
   for(int y=0;y<Height;y++)
   {
	 DWORD LW=y*Width;
	 for(int x=0;x<Width;x++)
	 {
	  Points[x+LW].WaterHeight-=0.1;
	  if(Points[x+LW].WaterHeight<0)Points[x+LW].WaterHeight=0;
      FullHeights[x+LW]=Points[x+LW].WaterHeight+Points[x+LW].Height;
	  if(x==0 || x==Width)FullHeights[x+LW]=0xFFFF;
	  if(y==0 || y==Height)FullHeights[x+LW]=0xFFFF;
	  Points[x+LW].PW=0;
	 }
   }
   ZeroMemory(DeltaWater  ,sizeof(*DeltaWater)*Width*Height);
   ZeroMemory(DeltaHeight ,sizeof(*DeltaHeight)*Width*Height);
   float  H [8],sH [8], M[8],MID,Min;
   int    MaxID;
   DWORD  TP[8],sTP[8];

	for(int y=1;y<Height-1;y++)
	{
		DWORD LW=y*Width;
	 for(int x=1;x<Width-1;x++)
	 {
		 DWORD TPm;
		 TPm=x+LW  ;
		 //remove some water
		 if(Points[TPm].WaterHeight<0) continue;	 
		 TP[0]=x-1+LW;            H[0]=FullHeights[TP[0]];
		                      float MH=FullHeights[TPm  ];
		 TP[1]=x+1+LW;            H[1]=FullHeights[TP[1]];
		 TP[2]=x+LW-Width;        H[2]=FullHeights[TP[2]];
		 TP[3]=x+LW+Width;        H[3]=FullHeights[TP[3]];
		 //mix it
		 BYTE MIX[4];
		 for(int i=0;i<4;i++){MIX[i]=roundrandom(4);if(MIX[i]>3) i--; else for(int ix=0;ix<i;ix++) if(MIX[i]==MIX[ix] || MIX[i]>3){ i--;break;} }
		 for(int i=0;i<4;i++){sH[i]=H[i];sTP[i]=TP[i];}
         for(int i=0;i<4;i++){H[i] =sH[MIX[i]];TP[i]=sTP[MIX[i]];}
		 //calc mids
		 for(int i=0;i<4;i++)M[i]=MH-H[i];
		 MaxID=-1;
		 Min=0;
         for(int i=0;i<4;i++)if(M[i]>0 && (M[i]>Min || MaxID==-1)){ MaxID=i;Min=M[i];}
		 if(MaxID!=-1 && M[MaxID]>0 && !((Points[TPm].PW >> MIX[MaxID])&0x01))
		 {
			 Points[TPm].PW|=1 << MIX[MaxID];
			 float Delta=M[MaxID];
			 float SurfaceDelta=Points[TPm].Height     -Points[TP[MaxID]].Height;
             //SurfaceDelta=1-SurfaceDelta/(SurfaceDelta+10000);
			 float WaterDelta=Delta;
			 if(WaterDelta>Points[TPm].WaterHeight) WaterDelta=Points[TPm].WaterHeight;
			 WaterDelta*=0.4;//825;
			 float SurfaceD=SurfaceDeltaAdd;
			 //if(WaterDelta>100) SurfaceD=0;
			 WaterDelta-=SurfaceDelta*0.5;
			 if(WaterDelta<0) WaterDelta=0;
             DeltaWater[TPm      ]-=WaterDelta;
			 if(x==1 && MIX[MaxID]==0) continue;
			 if(x==Width-1 && MIX[MaxID]==1) continue;
			 if(y==1 && MIX[MaxID]==2) continue;
			 if(y==Height-1 && MIX[MaxID]==3) continue;
             DeltaWater[TP[MaxID]]+=WaterDelta;
			 ///(Points[TPm      ].WaterHeight);
             DeltaHeight[TPm      ]-=SurfaceD;
             DeltaHeight[TP[MaxID]]+=SurfaceD*0.9;// 10% of surface deleted
			 //FullHeights[TPm]      -=WaterDelta+SurfaceD;
             //FullHeights[TP[MaxID]]+=WaterDelta+SurfaceD;
			 
		 }	 
	 }
	}

   for(int y=0;y<Height;y++)
   {
	 DWORD LW=y*Width;
	 for(int x=0;x<Width;x++)
	 {
	  if(DeltaWater [x+LW])Points[x+LW].WaterHeight+=DeltaWater [x+LW];
      if(DeltaHeight[x+LW])Points[x+LW].     Height+=DeltaHeight[x+LW];
	 }
   }
}



void CErosionManager::MoveWater()
{
   int i=0;
   for(int y=0;y<Height;y++)
   {
	 DWORD LW=y*Width;
	 for(int x=0;x<Width;x++)
	 {
	  DWORD TPm=x+LW  ;
	  //Points[TPm].WaterHeight-=1; //remove water
	  if(Points[TPm].WaterHeight<WATER_EPS)Points[TPm].WaterHeight=0;
      FullHeights[x+LW]=Points[TPm].WaterHeight+Points[TPm].Height;
	  Points[TPm].PW=0;
	  if(x==0 || x==Width )FullHeights[x+LW]=0xFFFF;
	  if(y==0 || y==Height)FullHeights[x+LW]=0xFFFF;
	 }
   }
   ZeroMemory(DeltaWater  ,sizeof(*DeltaWater)*Width*Height);
   ZeroMemory(DeltaHeight ,sizeof(*DeltaHeight)*Width*Height);
   float  H [8],sH [8], M[8],MID,Min;
   int    MaxID;
   DWORD  TP[8],sTP[8];
   bool   Passable[4];
	for(int y=1;y<Height-1;y++)
	{
		DWORD LW=y*Width;
	 for(int x=1;x<Width-1;x++)
	 {
		 DWORD TPm;
		 TPm=x+LW  ;
		 if(Points[TPm].WaterHeight<WATER_EPS) continue;
		 float MH=FullHeights[TPm  ];
		 TP[0]=x-1+LW;            H[0]=FullHeights[TP[0]];
		 TP[1]=x+1+LW;            H[1]=FullHeights[TP[1]];
		 TP[2]=x+LW-Width;        H[2]=FullHeights[TP[2]];
		 TP[3]=x+LW+Width;        H[3]=FullHeights[TP[3]];
		 ZeroMemory(M,sizeof(M));
		 float NumPassable=0;
		 for(int i=0;i<4;i++) Passable[i]=!((Points[TPm].PW >> i)& 0x01);
		 for(int i=0;i<4;i++) if(Passable[i]){M[i]=MH-H[i];if(M[i]<0)Passable[i]=false;else NumPassable++;}
		 if(!NumPassable)continue;
   	     MaxID=-1;Min=0;
/*		 if((Passable[0] && Passable[1] && M[0]-M[1]<WATER_FLOAT_EPS) &&
			(Passable[0] && Passable[2] && M[0]-M[2]<WATER_FLOAT_EPS) &&
			(Passable[0] && Passable[3] && M[0]-M[3]<WATER_FLOAT_EPS) &&
			(Passable[1] && Passable[2] && M[1]-M[2]<WATER_FLOAT_EPS) &&
			(Passable[1] && Passable[3] && M[1]-M[3]<WATER_FLOAT_EPS) &&
			(Passable[2] && Passable[3] && M[2]-M[3]<WATER_FLOAT_EPS) )MaxID=-2;
		 else
         for(int i=0;i<4;i++)if(Passable[i] && M[i]>0 && (M[i]<Min || MaxID==-1)){ MaxID=i;Min=M[i];}
		 if(MaxID==-1) continue;
         //min is found
		 float WaterDelta=M[MaxID];
		 if(WaterDelta<WATER_FLOAT_EPS) continue;
		 //if(MaxID==-2) {WaterDelta=M[0];NumPassable++;}
		 //else          {for(int i=0;i<4;i++)if(M[i]==M[MaxID])Passable[i]=false; WaterDelta=M[MaxID];}
		 if (WaterDelta>Points[TPm].WaterHeight) WaterDelta=Points[TPm].WaterHeight;
		 WaterDelta*=0.5;///=NumPassable;
		 float SurfaceD=SurfaceDeltaAdd;///NumPassable;
		 //WaterDelta-=SurfaceD;
		 if(WaterDelta<0) WaterDelta=0;
		 //find min
		 MaxID=-1;Min=0;
		 for(int i=0;i<4;i++)if(Passable[i] && M[i]>0 && (M[i]>Min || MaxID==-1)){ MaxID=i;Min=M[i];}
		 //for(int i=0;i<4;i++)
		 for(int i=MaxID;i<=MaxID;i++)
		 if(Passable[i])
		 {
			 Passable[i]=false;
			 Points[TP[i]].PW|=1 << i;
             DeltaWater [TPm  ]-=WaterDelta;
			 DeltaWater [TP[i]]+=WaterDelta;
             DeltaHeight[TPm  ]-=SurfaceD;
             DeltaHeight[TP[i]]-=SurfaceD;//*M[i];
		 }	 
	 }
	}

   for(int y=0;y<Height;y++)
   {
	 DWORD LW=y*Width;
	 for(int x=0;x<Width;x++)
	 {
	  if(DeltaWater [x+LW])Points[x+LW].WaterHeight+=DeltaWater [x+LW];
      if(DeltaHeight[x+LW])Points[x+LW].     Height+=DeltaHeight[x+LW];
	 }
   }
}
*/