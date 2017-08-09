#include "StdAfx.h"
#include "erosionmanager.h"

#define WATER_EPS 1
#define WATER_FLOAT_EPS 0.0001

CErosionManager::CErosionManager(void)
{
	Mode=false;
}

CErosionManager::~CErosionManager(void)
{
}

void CErosionManager::Create(DWORD x,DWORD y,float XF,float YF,float ZF)
{
	Points=new CEroVertex[x*y];
	FullHeights = new float[x*y];
	DeltaFull   = new float[x*y*4];
    DeltaWater  = new float[x*y];
    DeltaHeight = new float[x*y];
	ZeroMemory(Points,sizeof(CEroVertex)*x*y);
    VOut  =new CEroOut   [x*y];
	ZeroMemory(VOut,sizeof(CEroOut)*x*y);
	Width =x;
	Height=y;
	FacX  =XF;
	FacY  =YF;
	FacZ  =ZF;

	Index=new DWORD[x*y*6];
	IndexSize=x*y*6;
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
DWORD CErosionManager::Tick(float Time)
{
	for(int i=0;i<Time;i++)
  	{
	// SpawnWater(100);
     MoveWater();
	}
	return 0;
}

void CErosionManager::SpawnWater(DWORD Count)
{
	DWORD WH=Width*Height;
	DWORD W2=2;//(Width-10)/2;
	DWORD H2=2;//(Height-10)/2;
  for(int i=0;i<Count;i++)
  {
	DWORD x=30+roundrandom(4);//W2+roundrandom(Width-4);//Width-1);
	DWORD y=30+roundrandom(4);//H2+roundrandom(Height-4);//Height-1);
	WH=x+y*Width;
	Points[WH].WaterHeight+=random()*200;
  }
//	  Points[//(30+roundrandom(10))*Width+30+roundrandom(10)].WaterHeight+=random()*10;
//	  roundrandom(WH)].WaterHeight+=random()*10;
}

#define SurfaceDeltaAdd 0.02500

void CErosionManager::MoveWater()
{
   int i=0;
   for(int y=0;y<Height;y++)
   {
	 DWORD LW=y*Width;
	 for(int x=0;x<Width;x++)
	 {
	  //Points[x+LW].WaterHeight-=1;
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
             SurfaceDelta=1-SurfaceDelta/(SurfaceDelta+100);
			 float WaterDelta=Delta;
			 if(WaterDelta>Points[TPm].WaterHeight) WaterDelta=Points[TPm].WaterHeight;
			 WaterDelta*=0.1;//825;
			 float SurfaceD=SurfaceDeltaAdd*SurfaceDelta;
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
             DeltaHeight[TP[MaxID]]+=SurfaceD;// 10% of surface deleted
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

LPCVOID  CErosionManager::GetOutData()
{
	float shiftx=-FacX*Width/2.0;
	float shifty=-FacY*Height/2.0;
    for(int y=0;y<Height;y++)
	 for(int x=0;x<Width;x++)
	 {
		 DWORD LW=y*Width;
		 VOut[x+LW].Pos[0]=shiftx+x*FacX;
		 if(Mode)
		  VOut[x+LW].Pos[1]=(Points[x+LW].Height/*+Points[x+LW].WaterHeight*/)*FacY;
		 else
          VOut[x+LW].Pos[1]=(Points[x+LW].Height+Points[x+LW].WaterHeight-1)*FacY;
		 VOut[x+LW].Pos[2]=shifty+y*FacZ;
		 if(Mode)
		 {
			 VOut[x+LW].Color[0]=VOut[x+LW].Color[1]=VOut[x+LW].Color[2]=(x%2 +y%2==1)?0xFF:0;
		 }
		 else
		 {
		 VOut[x+LW].Color[0]=VOut[x+LW].Color[1]=VOut[x+LW].Color[2]=0xFF;//*(1-Points[x+LW].Height);
		 if(Points[x+LW].WaterHeight>1)
		 {
		  VOut[x+LW].Color[0]=0;//Points[x+LW].WaterHeight*2;
		  VOut[x+LW].Color[1]=0;//Points[x+LW].WaterHeight*2;
		  VOut[x+LW].Color[2]=Points[x+LW].WaterHeight*3;
		 }
		 }
	 }
	return (LPCVOID)&VOut[0].Pos[0];
}

LPCVOID  CErosionManager::GetIndex()      {	return (LPCVOID)Index;}
DWORD    CErosionManager::GetIndexCount() { return IndexSize;}
WORD     CErosionManager::GetDataStride() {	return sizeof(CEroOut);}
BYTE     CErosionManager::GetColorOff  () { CEroOut Pos;return (DWORD)&Pos.Color-(DWORD)&Pos;}


CErosionManager* DLLTYPE GetErrosion()
{
	return new CErosionManager();
}


void CErosionManager::StartAssert(LPCSTR Mode){}
DWORD CErosionManager::EndAssert(){ Create(px,py,pFX,pFY,pFZ);return 0;}
bool  CErosionManager::AssertParamF(LPCSTR Name,float Val)
{
	if(strcmp(Name,"Mode")==0) Mode=!Mode;else
	if(strcmp(Name,"Emite")==0) SpawnWater(Val);
	if(strcmp(Name,"X")==0) px=Val;else
	if(strcmp(Name,"Y")==0) py=Val;else
	if(strcmp(Name,"FX")==0) pFX=Val;else
	if(strcmp(Name,"FY")==0) pFY=Val;else
	if(strcmp(Name,"FZ")==0) pFZ=Val;else return false;
	return true;
}

