#pragma once

#include "ProjectGraphic.h"
#pragma pack(push,1)

struct CRawTerrainElement
{
	float Height;
	WORD Texture;
};

typedef float TerrainVertexType;
typedef float TerrainTextureType;

struct CTerrainOutElement
{
  TerrainVertexType   Vertex[3];
 // TerrainTextureType  Texture1[2];
};

#pragma pack(pop)

#define T_BLOCK_PX  8
#define T_BLOCK_PY  8

struct CTerrainRawBlock
{
	DWORD               TerrainSet;
	bVector3f           Scale;
	float               startZ;
	CRawTerrainElement  Elements[T_BLOCK_PX*T_BLOCK_PY];
};

struct CTerrainOutBlock
{
   SmartVector<CTerrainOutElement> Elements;
   DWORD                Texture;
   DWORD                TextureStage;
};

struct CTerrainImposter
{
	 bool Exists;
     bVector2d FromPosition;
	 DWORD     TextureID;
};

struct CTerrainDataBlock
{
	bool Exists;
    CTerrainOutBlock BlockOut;
	CTerrainDataBlock(){ Exists=false;}
	CTerrainDataBlock(CTerrainRawBlock &Block);
};

struct CTerrainBlockLoadInfo
{
	bool  Visible;
	DWORD RenderPos;
	DWORD UseTime;
	DWORD Sector;
	DWORD x,y,i;
    bVector3f Position;
	bVector3f Size;
	CTerrainImposter  *Imposter;
    CTerrainDataBlock *Raw;
	CTerrainRawBlock  *Data;
	CTerrainBlockLoadInfo()
	{
		RenderPos=0;
	}
};

typedef SmartVector<CTerrainBlockLoadInfo*> PBlockArray;

class CTerrainDrawer: public CSceneNode
{
	PBlockArray        Blocks;
public:
	CTerrainDrawer();
	~CTerrainDrawer();
	void Clear();
	void Add(CTerrainBlockLoadInfo* Block);
	void SetNumObjects(DWORD Num);
	//virtual PDrawableDataArray GetData(PDrawableDataArray Data,DWORD Layer);
};

typedef SmartVector<CTerrainBlockLoadInfo> BlockArray;

class CTerrainHolder;

class DLLTYPE CTerrain:public BlopBaseObject//public CDrawableObject
{
    bVector3f cPosition,cLook,cFrustum;

    BlockArray LoadedBlocks;
    DWORD      NumBlocks;
	bVector2d Position;
	CCamera   Lookup;
	float     FadingOp;
	CTerrainHolder *Holder;
	CTerrainDrawer *Drawer;
	void MakeBlock(CTerrainRawBlock &Block,DWORD x,DWORD y,DWORD i);
	void UploadBlocks();
	void MarkBlocks  ();
	void DoMarked    ();
	void MarkByLine  (bVector3f Start,bVector3f Look,bVector3f Frust);
	bool isLoaded(DWORD i);
	bool LoadLine(DWORD Start,DWORD End);

	bool Loaded;
	float GetLOD(int X,int Y);
	void  CheckVisible();
	bool  IsVisible(int x,int y);
public:
	CTerrain(void);
	~CTerrain(void);
    DWORD     Tick(float Time);
	bool     Start();
};



typedef CTerrain *PCTerrain;
