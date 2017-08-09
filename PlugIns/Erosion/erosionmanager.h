#pragma once

struct CEroVertex
{
	float Height;
	float WaterHeight;
	BYTE  PW;
};

struct CEroOut
{
	float Pos[3];
	BYTE  Color[3];
};

struct CMidHeight
{
	float Mid;
	bool  Set;
};

class CErosionManager: public CParamAssertableTicableDrawableObject
{
	DWORD px,py;
	float pFX,pFY,pFZ;
	CEroVertex * Points;
    CEroOut    * VOut;
	DWORD        Width,Height;
	float        FacX,FacY,FacZ;
	DWORD      * Index;
	DWORD        IndexSize;

	float      * FullHeights;
	float      * DeltaFull;
	float      * DeltaWater;
	float      * DeltaHeight;
	void  SpawnWater(DWORD Count);
	void  MoveWater();

	bool  Mode;
public:
	CErosionManager(void);
	~CErosionManager(void);
	void Create(DWORD x,DWORD y,float XF,float YF,float ZF);

	DWORD    Tick(float Time);
	LPCVOID  GetOutData();
	LPCVOID  GetIndex  ();
	DWORD    GetIndexCount();
	WORD     GetDataStride();
	BYTE     GetColorOff  ();

	void StartAssert(LPCSTR Mode=NULL);
	DWORD EndAssert();
	bool  AssertParamF(LPCSTR Name,float Val);
};

