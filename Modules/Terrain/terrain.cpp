#include "StdAfx.h"
#include "ProjectGraphic.h"
#include "terrain.h"
#include "terrainholder.h"
#include "mudartloader.h"
#include "BlopLoader.h"

DWORD SYS_TERRAIN;

CTerrain::CTerrain(void)
{
    if(!SYS_TERRAIN)
     RegisterNewConsoleClass(SYS_TERRAIN,"TERRAIN",-1);

	Holder=new CMudArtLoader();
	Holder->Load("Data/Maps/1.rlf");
	Holder->SaveAsDefault("Data/Terrain/map1.blk");

	Print2Console(SYS_TERRAIN,"Created");
	Holder=new CBlopLoader();
	if(Holder->Load("Data/Terrain/map1.blk"))
		Print2Console(SYS_TERRAIN,"HOLDER: Load %s OK",Holder->GetMapFile());else
		Print2Console(SYS_TERRAIN,"HOLDER: Load %s FAIL",Holder->GetMapFile());

    Loaded=false;
	Drawer=new CTerrainDrawer();

}

CTerrain::~CTerrain(void)
{
	delete Holder;
	delete Drawer;
}

DWORD     CTerrain::Tick(float Time)
{
	if(!Loaded)
	{
	 MarkBlocks();
	 UploadBlocks(); 
     CheckVisible();
	 DoMarked();
	}
	Loaded=true;
	return 0;
}

bool CTerrain::IsVisible(int x,int y)
{
 if(y<2)return true;
 return false;
}

void CTerrain::CheckVisible()
{
	for(int i=0;i<LoadedBlocks.size();i++)
	{
		if(IsVisible(LoadedBlocks[i].x,LoadedBlocks[i].y))LoadedBlocks[i].Visible=true;
		else                                              LoadedBlocks[i].Visible=false;
	}
}

bool CTerrain::Start()
{
//	CGenericGraphic::RootNode.AddChild(Drawer,(PASS_Z_FIRST,PASS_AMBIENT,PASS_LIGHT));
	return true;
}

void CTerrain::UploadBlocks()
{
	 NumBlocks=0;
	 DWORD MA=Holder->GetMA();
 	 BYTE *M=Holder->MARKS;
	 int  LoadStart=-1;
	 DWORD i;
	 for(i=0;i<MA;i++,M++)
	 	if(*M && !isLoaded(i))
	 	{  if(LoadStart==-1) LoadStart=i;}
		else{
	 	if(LoadStart!=-1)
	 	{
	 		LoadLine(LoadStart,i);
	 		LoadStart=-1;
		} if (isLoaded(i))NumBlocks++;}
	 if(LoadStart!=-1){LoadLine(LoadStart,i);LoadStart=-1;}
}
void CTerrain::MarkBlocks  ()
{
	CCamera &Camera=CGenericGraphic::GetCurrentCamera();
	cPosition =Camera.GetPosition();
	cLook     =Camera.GetLookVector();
	cFrustum  =Camera.GetFrustumVector();
	Holder->MarkAll(0);
	MarkByLine(Camera.GetPosition(),cLook,cFrustum);
}

void CTerrain::MarkByLine  (bVector3f Start,bVector3f Look,bVector3f Frust)
{
 Holder->MarkAll(1);
}

void CTerrain::DoMarked    ()
{
	Drawer->SetNumObjects(NumBlocks);	
	//Drawer->Clear();
	for(int i=0;i<LoadedBlocks.size();i++)
		if(LoadedBlocks[i].Visible)
	      Drawer->Add(&LoadedBlocks[i]);
}

bool CTerrain:: LoadLine(DWORD Start,DWORD End)
{ 
	int M=End-Start;
	if(M<1) return false;
	NumBlocks+=M;
	CTerrainRawBlock Block;
	DWORD x,y;
	x=y=0;
	for(DWORD i=Start;i<End;i++)
	{
        Holder->TranslateI(i,x,y);
		Holder->LoadBlock(Block,x,y);
		MakeBlock(Block,x,y,i);
	}
	return false;
}

bool  CTerrain::isLoaded(DWORD i)
{
	BlockArray::iterator end=LoadedBlocks.end();
	for(BlockArray::iterator it=LoadedBlocks.begin();it!=end;it++)
		if((*it).i==i)
		{
			if((*it).Data) return true;
			return false;
		}
   return false;
}

void CTerrain::MakeBlock(CTerrainRawBlock &Block,DWORD x,DWORD y,DWORD i)
{
 CTerrainBlockLoadInfo BInfo;
 BInfo.Visible=false;
 BInfo.UseTime=0;
 BInfo.i      =i;
 BInfo.x      =x;
 BInfo.y      =y;
 BInfo.Sector =0;
 BInfo.Position.Set(x*7,0,y*7);
 BInfo.Size    =Block.Scale;
 BInfo.Imposter=NULL;
 BInfo.Data    =new CTerrainRawBlock(Block);//,GetLOD(x,y));
 BInfo.Raw     =new CTerrainDataBlock(Block);
 LoadedBlocks.push_back(BInfo);
}

float CTerrain::GetLOD(int X,int Y)
{
 return 1;
}

PCTerrain DLLTYPE GetTerrain()
{
	return new CTerrain();
}

DWORD IndC88=0;
DWORD Ind88[300];

void CreateIndexes()
{
	if(!IndC88)
	{
  	  DWORD i=0;
	  DWORD Width,Height;
	  Width=Height=8;
	  for(DWORD y=0;y<Height-1;y++)
	   for(DWORD x=0;x<Width-1;x++)
	   {
		   Ind88[i++]=(x+0)+(y+0)*Width;
		   Ind88[i++]=(x+0)+(y+1)*Width;
		   Ind88[i++]=(x+1)+(y+1)*Width;
 
  		   Ind88[i++]=(x+1)+(y+1)*Width;
		   Ind88[i++]=(x+1)+(y+0)*Width;
		   Ind88[i++]=(x+0)+(y+0)*Width;
	  }
      IndC88=i;
	}
}

CTerrainDrawer::CTerrainDrawer()
{
	CreateIndexes();
	//CGenericGraphic::AddObject(this,NULL);
}

CTerrainDrawer::~CTerrainDrawer()
{
	//CGenericGraphic::RemoveObject(this);
}

void CTerrainDrawer::Clear()
{
	Blocks.clear();
}

void CTerrainDrawer::SetNumObjects(DWORD Num)
{ 
	//Num=10;
/*	if(myData.size()<(int)Num)
	for(int i=myData.size();i<(int)Num;i++)
		myData.push_back(new Draw::CDrawableData());
	else
	{
		int M=myData.size();
	    for(int i=Num;i<M;i++)
		 delete myData[i];
	    myData.SetSize(Num);
	}
*/	
}

void CTerrainDrawer::Add(CTerrainBlockLoadInfo* Block)
{
	/*
	if(Blocks[Block->RenderPos]!=Block)
	 for(int i=0;i<Blocks.size();i++)
		 if(Blocks[i]==Block){ Block->RenderPos=i; return;}
    */
	Blocks.push_back(Block);
}


CTerrainDataBlock::CTerrainDataBlock(CTerrainRawBlock &Block)
{
	Exists=true;
	BlockOut.Texture     =Block.TerrainSet;
	BlockOut.TextureStage=0;
	BlockOut.Elements.SetSize(8*8);
	int x,y;
	x=y=0;
	float SX=Block.Scale[0];
	float SY=Block.Scale[1];
	float SZ=Block.Scale[2];
	float f;
	for(int i=0;i<8*8;i++)
	{
		BlockOut.Elements[i].Vertex[0]=f=x*SX;
		BlockOut.Elements[i].Vertex[1]=f=(Block.startZ+Block.Elements[i].Height*SY)*10;
		BlockOut.Elements[i].Vertex[2]=f=y*SZ;
		x++;
		if(x>7){x=0;y++;}
	}
}



