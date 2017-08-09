#pragma once

struct CEroHeight
{
 float Height;
 WORD  NumParticles;
};

struct CEroOut
{
	float Pos[3];
	BYTE  Color[3];
};

struct CWaterParticle
{
	float Weight;
	float Mud;
	float X;WORD cX;
	float Y;WORD cY;DWORD WH;
	float Z;
	bVector3f Speed;
	bool  Enabled;
};



class CParticleErosion :public CParamAssertableTicableDrawableObject
{
	DWORD           Width,Height;
    CWaterParticle *WaterParticles;
	DWORD           NumParticles;
	DWORD           UsedParticles;

	CEroOut       * VOut;
	DWORD         * Index;
	DWORD           IndexSize;

	float           FacX,FacY,FacZ;

	CEroHeight     *Land;
	void  SpawnWater(DWORD Count);
	void  MoveWater();

	CParticleSet   *PS;
public:
	CParticleErosion(void);
	~CParticleErosion(void);

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
