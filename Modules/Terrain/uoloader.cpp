#include "StdAfx.h"
#include "uoloader.h"

//---------------------------------------------------------------------------


#pragma hdrstop

#include "UoLoader.h"
#include <io.h>
#include <fcntl.h>
#include <SYS\STAT.H>
#include <stdio.h>
//#include "Unit2.h"

//---------------------------------------------------------------------------
#define CheckFlag(a,b) if(a & b)
#define CheckFlagAndAddSome(a,b,c,d,f,g) CheckFlag(a,b) strcat(g,c);else strcat(g,d);strcat(g,f)

WORD UO_StdOffX =2;
WORD UO_StdOffY =2;
WORD UO_OffX    =10;
WORD UO_OffY    =10;
WORD UO_OffZ    =1;


LPCSTR CUOLoader::ViewTileType(DWORD TID,LPSTR Buffer)
{
 Buffer[0]=0;
 if(TID>=UO_NUM_TEXTURES) return Buffer;
 DWORD Flag=TEXREF[TID].Flags;

 CheckFlagAndAddSome(Flag,UO_FLAG_Background  ,"BG","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Weapon      ,"WP","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Transparent ,"TR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Translucent ,"TL","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Wall        ,"WL","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Damaging    ,"DM","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Impassable  ,"!P","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Wet         ,"WT","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Unknown     ,"*1","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Surface     ,"SR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Bridge      ,"BR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_GenericStackable ,"GS","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Window      ,"WN","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_NoShoot     ,"NS","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_PrefixA     ,"PA","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_PrefixAn    ,"AB","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Internal    ,"IN","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Foliage     ,"FL","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_PartialHue  ,"PH","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Unknown1    ,"*2","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Map         ,"MP","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Container   ,"CT","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Wearable    ,"WR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_LightSource ,"LS","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Animated    ,"AN","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_NoDiagonal  ,"ND","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Unknown2    ,"*3","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Armor       ,"AR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Roof        ,"RF","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_Door        ,"DR","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_StairBack   ,"SB","__"," ",Buffer);
 CheckFlagAndAddSome(Flag,UO_FLAG_StairRight  ,"SR","__"," ",Buffer);
 return Buffer;
}

CUOLoader::CUOLoader()
{
 Clear();
 PrepareStaticArrays();
 OpenArtMul();
 OpenTexMapMul();
 OpenTexMipMul();
 OpenWallTexMul();
 OpenMapFiles();
 StaticBilder= new CUO_StaticBuilder(this);
}

CUOLoader::~CUOLoader()
{
 delete StaticBilder;
 CloseArtMul();
 CloseTexMapMul();
 CloseTexMipMul();
 CloseWallTexMul();
 CloseMapFiles();
}

void CUOLoader::LoadDataInformation()
{
 LoadArtIDX();
 LoadTexIDX();
 LoadWallIDX();
 LoadStaticIDX();
 LoadTexMapIDX();
 LoadTMIDX();
}

void CUOLoader::Clear()
{
 ZeroMemory(ARTREF       ,sizeof(ARTREF));
 ZeroMemory(TEXREF       ,sizeof(TEXREF));
 ZeroMemory(TileLoaded   ,sizeof(TileLoaded));
 ZeroMemory(TileData     ,sizeof(TileData));
 ZeroMemory(WallIndex    ,sizeof(WallIndex));
 ZeroMemory(StaticIDX    ,sizeof(StaticIDX));
 ZeroMemory(UOBlocks     ,sizeof(UOBlocks));
 ZeroMemory(LandVTex0    ,sizeof(LandVTex0));
 ZeroMemory(LandVertexes ,sizeof(LandVertexes));
 ZeroMemory(StaticTextures ,sizeof(StaticTextures));
 ZeroMemory(fhWallTexFile,sizeof(fhWallTexFile));


 UsedTiles=0;
 NumLandBlocks=0;
}

void CUOLoader::PrepareStaticArrays()
{
 for(DWORD y=0;y<9;y++)
 { DWORD yW=y*9;
 for(DWORD x=0;x<9;x++)
 {
  DWORD yk=x+yW;
  LandVTex0[yk].u=x/9.0f;
  LandVTex0[yk].v=y/9.0f;
 }
 }

}

void CUOLoader::OpenArtMul()
{
 fhArtMul=open("art.mul",O_BINARY,_S_IREAD);
}

void CUOLoader::CloseArtMul()
{
 close(fhArtMul);
};

void CUOLoader::OpenTexMapMul()
{
 fhTexMapMul=open("texmaps.mul",O_BINARY,_S_IREAD);
}

void CUOLoader::CloseTexMapMul()
{
 close(fhTexMapMul);
};

void CUOLoader::OpenTexMipMul()
{
  char Buf[128];
  DWORD _W=64;
  for(DWORD i=0;i<UO_NUM_TEXTURE_MIPMAPS;i++,_W/=2)
  {
   sprintf(Buf,"TexMap%d.mul",_W);
   fhMipTexFile[i]=open(Buf,O_BINARY,_S_IREAD);
  }
}

void CUOLoader::CloseTexMipMul()
{
 for(DWORD i=0;i<UO_NUM_TEXTURE_MIPMAPS;i++)
  close(fhMipTexFile[i]);
}

void CUOLoader::OpenWallTexMul()
{
 char fBuf[0xFF];
 for(DWORD x=0;x<10;x++)
 {
  sprintf(fBuf,"WallIDX_tex%d.mul",x);
  fhWallTexFile[x]=open(fBuf,O_BINARY,_S_IREAD);
 }
}

void CUOLoader::CloseWallTexMul()
{
 for(DWORD x=0;x<10;x++)
  close(fhWallTexFile[x]);
}

void CUOLoader::OpenMapFiles()
{
  fhMapMul   =open("map0.mul",O_BINARY,_S_IREAD);
  fhStaticMul=open("statics0.mul",O_BINARY,_S_IREAD);
}

void CUOLoader::CloseMapFiles()
{
  close(fhMapMul);
  close(fhStaticMul);
}

void CUOLoader::LoadTMIDX()
{
 int  f=open("TexMipmapIDX.mul",O_BINARY,_S_IREAD);
 if(f<1) return;
 UO_TEX_LOOKUP WI[1527];
 int rc=read(f,WI,sizeof(WI));
 rc/=sizeof(WI[0]);
 for(DWORD x=0;x<1527;x++)
  TexMipIndex[WI[x].TexID]=WI[x];
 close(f);
}

void CUOLoader::LoadArtIDX()
{
 int  f=f=open("artidx.mul",O_BINARY,_S_IREAD);
 if(f<1) return;
 UO_ARTIDXITEM AID[0xFFFF];
 DWORD x=0;
 x=read(f,AID,(4*3)*0xFFFF);
 x/=sizeof(4*3);
 if(x>0xFFFF) x=0xFFFF;
 for(DWORD y=0;y<x;y++)
 { ARTREF[y].Lookup=AID[y].Lookup;
   ARTREF[y].Size  =AID[y].Size;
 }
 close(f);
}

void CUOLoader::LoadTexMapIDX()
{
 int  f=f=open("texidx.mul",O_BINARY,_S_IREAD);
 if(f<1) return;
 UO_ARTIDXITEM AID[0x1000];
 DWORD x;
 x=read(f,AID,(4*3)*0x1000);
 x/=sizeof(4*3);
 if(x>0x1000) x=0x1000;
 for(DWORD y=0;y<x;y++)
 { TexIndex[y].Lookup=AID[y].Lookup;
   if(AID[y].Size== 0x2000) TexIndex[y].SizeType=1;else
   if(AID[y].Size== 0x8000)
                            TexIndex[y].SizeType=2;else
                            TexIndex[y].SizeType=0;
 }
 close(f);
}

void CUOLoader::LoadTexIDX()
{
 int  f=f=open("tiledata.mul",O_BINARY,_S_IREAD);
 if(f<1) return;
 UO_TEXMAP_ITEM AID[32];
 DWORD x=0;
 DWORD y,z;
 DWORD i=0;
 for(y=0;y<512;y++)
 {
  read(f,AID,4);
  read(f,AID,0x1A*32);
  for(z=0;z<32;z++)
  {
   TEXREF[x].Texture=AID[z].Texture;
   TEXREF[x].Type   =UO_TYPE_TILE;
   TEXREF[x].Width  =44;
   TEXREF[x].Height =44;
   TEXREF[x].Flags  =UO_FLAG_Surface;
   strcpy(TEXREF[x].TileName,AID[z].TileName);
   x++;
   i++;
  }
 }
 UO_TEX_EX TID[32];
 while(1)
 {
  if(read(f,TID,4)<4) break;
  read(f,TID,0x25*32);
  for(y=0;y<32;y++)
  {
   //TEXREF[x].Texture=TID[y].AnimID;
   //if(!TEXREF[x].Texture)
    TEXREF[x].Texture=i;
   TEXREF[x].Type   =UO_TYPE_RUN;
   TEXREF[x].Width  =44;
   TEXREF[x].Height =44;
   TEXREF[x].Flags  =TID[y].Flags;
   strcpy(TEXREF[x].TileName,TID[y].TileName);
   x++;
   i++;
  }
 }
 NumTexRefs=x;
 close(f);
}

void CUOLoader::LoadWallIDX()
{
 char Buf[0xFF];
 for(DWORD xf=0;xf<10;xf++)
 {
  sprintf(Buf,"WallIDX%d.mul",xf);
  int  f=open(Buf,O_BINARY,_S_IREAD);
  if(f<1) continue;
  DWORD Size;
  read(f,&Size,sizeof(Size));
  UO_WallIndex  *WallIndexDat=new UO_WallIndex[Size];
  int rret=read(f,WallIndexDat,sizeof(WallIndex[0])*Size);
  rret/=sizeof(WallIndex[0]);
  for(DWORD x=0;x<Size;x++)
   if(WallIndexDat[x].TexID<0xFFFF)
    WallIndex[WallIndexDat[x].TexID]=WallIndexDat[x];
  delete []WallIndexDat;
  close(f);
 }
}

void CUOLoader::LoadStaticIDX()
{
 int  f=f=open("staidx0.mul",O_BINARY,_S_IREAD);
 if(f<1) return;
 UO_Static_IDX_uo *SID=new UO_Static_IDX_uo[512*768];
 ZeroMemory(SID,sizeof(SID[0])*512*768);
 int rret=read(f,SID,sizeof(SID[0])*512*768);
 rret/=sizeof(SID[0]);
 for(DWORD x=0;x<768*512;x++)
 {
  StaticIDX[x].Start=SID[x].Start;
  StaticIDX[x].Objects=SID[x].Size/7;
 }
 delete []SID; 
 close(f);
}

DWORD _UO_STATIC_TEX_MAP_DATA[128*128];
WORD  _UO_STATIC_TEX_MAP_DATA_TEMP[128*128];

bool CUOLoader::GetTexMul         (DWORD **Data,DWORD TexID,DWORD Width)
{
 DWORD fi=0;
 switch(Width)
 {
  case 64: fi=0;break;
  case 32: fi=1;break;
  case 16: fi=2;break;
  case 8 : fi=3;break;
  default : return false;
 }
 lseek(fhMipTexFile[fi],TexMipIndex[TexID].Lookup[fi],SEEK_SET);
 DWORD ReadingSize=Width*Width*4;
 read(fhMipTexFile[fi],_UO_STATIC_TEX_MAP_DATA,ReadingSize*4);
 *Data=_UO_STATIC_TEX_MAP_DATA;
 return true;
}

DWORD _UO_STATIC_WALLTEX_DATA_LOAD[256*256];
DWORD _UO_STATIC_WALLTEX_DATA     [128*128];

DWORD* CUOLoader::GetWallTexture  (DWORD WID,DWORD &W,DWORD &H,DWORD &D)
{
 lseek(fhWallTexFile[WallIndex[WID].File],WallIndex[WID].Lookup,SEEK_SET);
 DWORD w=WallIndex[WID].Width;
 DWORD h=WallIndex[WID].Height;
 DWORD d=WallIndex[WID].Depth;
 DWORD ReadingSize=w*(h+d)*4;
 read(fhWallTexFile[WallIndex[WID].File],_UO_STATIC_WALLTEX_DATA_LOAD,ReadingSize);
 W=w;
 H=h;
 D=d;
 ZeroMemory(_UO_STATIC_WALLTEX_DATA,sizeof(_UO_STATIC_WALLTEX_DATA));
 DWORD _H=h+d;
 if(_H>128) _H=128;
 for(DWORD y=0;y<_H;y++)
  for(DWORD x=0;x<w;x++)
  {
   _UO_STATIC_WALLTEX_DATA[x+y*32]=_UO_STATIC_WALLTEX_DATA_LOAD[x+y*w];
  }
 return _UO_STATIC_WALLTEX_DATA;
}

bool CUOLoader::GetTexMap         (DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type)
{
 lseek(fhTexMapMul,TexIndex[TexID].Lookup,SEEK_SET);
 DWORD ReadingSize;
 if(TexIndex[TexID].SizeType==1) {Width=Height=64;ReadingSize=64*64;}else
 if(TexIndex[TexID].SizeType==2) {Width=Height=128;ReadingSize=128*126;}else return false;
 read(fhTexMapMul,_UO_STATIC_TEX_MAP_DATA_TEMP,ReadingSize*4);
 WORD Color16;
 DWORD Color32;
 for(DWORD x=0;x<ReadingSize;x++)
  {
   Color16=_UO_STATIC_TEX_MAP_DATA_TEMP[x];
   Color32 = ((((Color16 >> 10) & 0x1F) * 0xFF / 0x1F) |((((Color16 >> 5) & 0x1F) * 0xFF / 0x1F) << 8) |((( Color16 & 0x1F) * 0xFF / 0x1F) << 16));
   _UO_STATIC_TEX_MAP_DATA[x]=Color32;
  }
 *Data=_UO_STATIC_TEX_MAP_DATA;
 return true;
}



DWORD _UO_STATIC_RUN_TILE_DATA[256*256];

bool CUOLoader::GetTile           (DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type)
{
 DWORD tid=TileLoaded[TexID];
 Width =TEXREF[TexID].Width;
 Height=TEXREF[TexID].Height;
 if(Type)
  *Type=TEXREF[TexID].Flags;
 if(tid)
 {
  *Data=(DWORD*)&TileData[tid-1].TileData;
  return true;
 }

 tid=TEXREF[TexID].Texture;
 if(lseek(fhArtMul,ARTREF[tid].Lookup,SEEK_SET)==-1)
  return false;

 DWORD FLAG;
 if(read(fhArtMul,&FLAG,4)<0)
  return false;

 if (FLAG > 0xFFFF || FLAG == 0)
 {
  //RawTile
  if(!(TEXREF[TexID].Flags & UO_FLAG_Surface))
  {
   TEXREF[TexID].Flags = UO_FLAG_Surface;
   if(Type)
    *Type=TEXREF[TexID].Flags;
  }
  TileLoaded[TexID]=UsedTiles+1;
  ZeroMemory(&TileData[UsedTiles],44*44*4);
  WORD BitData[44*44];
  DWORD x;
  WORD Color16;
  DWORD Color32;
  read(fhArtMul,BitData,44*44*2);
  for(x=0;x<44*44;x++)
  {
   Color16=BitData[x];
   Color32 = ((((Color16 >> 10) & 0x1F) * 0xFF / 0x1F) |((((Color16 >> 5) & 0x1F) * 0xFF / 0x1F) << 8) |((( Color16 & 0x1F) * 0xFF / 0x1F) << 16));
   TileData[UsedTiles].TileData[x]=Color32;
  }
  *Data=(DWORD*)&TileData[UsedTiles].TileData;
  UsedTiles++;
  if(UsedTiles>0xFF)  UsedTiles=0;
 }
 else
 {
  //RunTile
  WORD W,H;
  read(fhArtMul,&W,2);
  read(fhArtMul,&H,2);
  Width=W;
  Height=H;
  if (W == 0 || W >= 1024) { return false;}
  if (H == 0 || H >= 1024) { return false;}
  WORD LStart[1024];
  read(fhArtMul,&LStart,2*H);
  DWORD DStart=tell(fhArtMul);
  ZeroMemory(_UO_STATIC_RUN_TILE_DATA,W*H*4);
  int X = 0;
  int Y = 0;
  WORD XOffs,Run;
  WORD P[1024];
  X = 0;
  Y = 0;
  lseek(fhArtMul,DStart + LStart[Y] * 2,SEEK_SET);
  while (Y < H)
 {
      //for(int x=0;x<W;x++)_UO_STATIC_RUN_TILE_DATA[x+X+Y*W]=0;
      read(fhArtMul,&XOffs,2);
      read(fhArtMul,&Run  ,2);
      if (XOffs + Run >= 2048)
      {
       return false;
      }
      else if (XOffs + Run != 0)
      {
       X += XOffs;
       read(fhArtMul,P,Run*2);
       for(int x=0;x<Run;x++)
       {
         DWORD Color16=P[x];
         DWORD Color32 = ( (((Color16 >> 10) & 0x1F) * 0xFF / 0x1F) |((((Color16 >> 5) & 0x1F) * 0xFF / 0x1F) << 8) |((( Color16 & 0x1F) * 0xFF / 0x1F) << 16));
         _UO_STATIC_RUN_TILE_DATA[x+X+Y*W]=Color32;
       }
       X += Run;
      }
      else
      {
        X = 0;
        Y++;
        lseek(fhArtMul,DStart + LStart[Y] * 2,SEEK_SET);
     }
   }
   *Data=(DWORD*)_UO_STATIC_RUN_TILE_DATA;
 }
 return true;
}

DWORD _UO_STATIC_RAW_COMP[44*44];

bool CUOLoader::GetCompensatedTile(DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type)
{
 DWORD *_Data;
 bool ret=GetTile(&_Data,TexID,Width,Height,Type);
 if(!ret) return false;
 if(!Type) { *Data=_Data;return true;}
 if(*Type == UO_FLAG_Surface)
 {
  DWORD y=0;
  DWORD k=2;
  int rev=1;
  int cl=0;
  for(DWORD x=0;x<44*44-4;y++)
   {
    for(DWORD z=0;z<k;z++)
    {
      _UO_STATIC_RAW_COMP[22-k/2+z+y*44]=_Data[x];
      x++;
    }
   k+=2*rev;
   if(k>42)
   {
    cl++;
    rev=0;
    if(cl>1) rev=-1;
   }
   if(k<2) break;
  }
  *Data=_UO_STATIC_RAW_COMP;
  return true;
 }
 *Data=_Data;
 return true;
}

int  CUOLoader::GetBlocks(UO_TEXMAP_block *Blocks,DWORD X,DWORD Y,DWORD Width,DWORD Height)
{
 DWORD BlockNumber;
 int lk=(4+8*8*3);
 int k=0;
 for(DWORD x=0;x<Width;x++)
 //for(DWORD y=0;y<Height;y++)
 {
  BlockNumber = ((X+x) * 512) + (Y);
  if(lseek(fhMapMul,BlockNumber*lk,SEEK_SET)==-1) { Blocks[x*Height].Cap=0;k+=1; continue;}
  if(read(fhMapMul,&Blocks[x*Height],lk*Height)<lk*Height) { Blocks[x*Height].Cap=1;k+=2; continue;}
 }
 return k;
}

bool  CUOLoader::GetBlockStatic(DWORD X,DWORD Y,UO_Static_object_uo* Static,DWORD &ObjectNum)
{
  DWORD BlockNumber = (X * 512) + (Y);
  if(BlockNumber>512*768) return false;
  int ret=StaticIDX[BlockNumber].Start;
  if(ret<0) return false;
  ret=lseek(fhStaticMul,StaticIDX[BlockNumber].Start,SEEK_SET);
  if(ret<0) return false;
  ret=7*StaticIDX[BlockNumber].Objects;
  ret=read( fhStaticMul,Static,ret);
  ret=errno;
  if(ret<0) return false;

  ObjectNum=StaticIDX[BlockNumber].Objects;
  return true;
}

UO_block *CUOLoader::FindBlock(DWORD x,DWORD y,UO_OffsetFindingRecord &OFS)
{
 DWORD _x=x+OFS.X;
 DWORD _y=y+OFS.Y;
 DWORD i;
 for(i=0;i<UO_MAX_CASHED_BLOCKS;i++)
   if(UOBlocks[i].x==_x && UOBlocks[i].y==_y) return &UOBlocks[i];

 for(i=0;i<UO_MAX_CASHED_BLOCKS;i++)
   if(!UOBlocks[i].Loaded)                   return &UOBlocks[i];
 // no free block.. find any other free
 DWORD Lower=0xFF;
 DWORD LowerID=0;
 for(i=0;i<UO_MAX_CASHED_BLOCKS;i++)
 {
   if(UOBlocks[i].x<OFS.X     || UOBlocks[i].y<OFS.Y ||
      UOBlocks[i].x>OFS.Width || UOBlocks[i].y>OFS.Height)
      {
        UOBlocks[i].Loaded=0;
        return &UOBlocks[i];
      }
//       if(UOBlocks[i].Priv<Lower) { Lower=UOBlocks[i].Priv;LowerID=i;}
 }
 UOBlocks[LowerID].Loaded=0;
 return &UOBlocks[LowerID];
}

#define UO_LAND_BLOCK_XSIZE 8

DWORD UO_IMP_LAND_DATA[512*512];

bool CUOLoader::PrepareGraphics(DWORD StartX,DWORD StartY,DWORD Width,DWORD Height)
{
 UO_TEXMAP_block MapBlocks[10*10];
 GetBlocks(MapBlocks,StartX,StartY,Width,Height);
 DWORD x;
 UO_OffsetFindingRecord OFS;
 OFS.X     =StartX;
 OFS.Y     =StartY;
 OFS.Width =StartX+Width;
 OFS.Height=StartY+Height;

 //****************************************************************
 // Create Geometry
 DWORD GeometryOffset=0;
 NumLandBlocks=0;
 for(x=0;x<Width;x++)
 for(DWORD y=0;y<Height;y++)
 {
  UO_block *Block=FindBlock(x,y,OFS);
  if(!Block->Loaded)Block->Loaded=1;
  RenderBlocks[NumLandBlocks++]=Block;
  Block->x     =StartX+x;
  Block->y     =StartY+y;
  Block->VStart=GeometryOffset;
  int OffX=-UO_StdOffX+x*(UO_LAND_BLOCK_XSIZE);
  int OffY=-UO_StdOffY+y*(UO_LAND_BLOCK_XSIZE);
  for(DWORD y1=0;y1<UO_LAND_BLOCK_XSIZE+1;y1++)
  {
   DWORD yw1=(y1)*(UO_LAND_BLOCK_XSIZE);
   for(DWORD x1=0;x1<UO_LAND_BLOCK_XSIZE+1;x1++)
   {
    LandVertexes[GeometryOffset].x=(x1+OffX)*UO_OffX;
    LandVertexes[GeometryOffset].z=(y1+OffY)*UO_OffY;
    if(y1<=7 && x1<=7)LandVertexes[GeometryOffset].y=MapBlocks[y+x*Width        ].Block[x1+yw1].level*UO_OffZ;else
    {if(y1>7 && x1<=7)LandVertexes[GeometryOffset].y=MapBlocks[x*Width+    (y+1)].Block[x1    ].level*UO_OffZ;else
     if(y1<=7 && x1>7)LandVertexes[GeometryOffset].y=MapBlocks[(x+1)*Width+    y].Block[yw1   ].level*UO_OffZ;else
     if(y1>7 && x1>7) LandVertexes[GeometryOffset].y=MapBlocks[(x+1)*Width+(y+1)].Block[0     ].level*UO_OffZ;else
     LandVertexes[GeometryOffset].y=0;}
    GeometryOffset++;
   }
  }
 }

 //***************************************************************
 // Create textures

 for(x=0;x<Width;x++)
 for(DWORD y=0;y<Height;y++)
 {
  UO_block *Block=FindBlock(x,y,OFS);
  DWORD W=16;
  if(Block->Loaded!=2)
  {
   for(DWORD y2=0;y2<UO_LAND_BLOCK_XSIZE;y2++)
   {
    DWORD yW=y2*UO_LAND_BLOCK_XSIZE;
    for(DWORD x2=0;x2<UO_LAND_BLOCK_XSIZE;x2++)
    {
     DWORD *BitData;
     if(!GetTexMul(&BitData,MapBlocks[x*Width+y].Block[x2+yW].color,W)) continue;
     for(DWORD i=0;i<W;i++)
      memcpy(&UO_IMP_LAND_DATA[x2*W+(y2*W+i)*W*8],&BitData[i*W],W*4);
    }
   }
   Block->TextureID=GenerateTexture(Block->TextureID,UO_IMP_LAND_DATA,W*8,W*8);
   Block->Loaded=2;
  }
 }
 
 return true;
}

void CUOLoader::ClearWalls()
{
 WallsNumber=0;
 UsedSTextures=0;
 STSC=0;
 SIBSC=0;
}

void CUOLoader::CreateWall(DWORD X,DWORD Y,UO_Static_object_uo &Obj)
{
 if(WallsNumber>UO_MAX_NUMBER_STATIC_OBJECTS) return;
 UO_WallIndex_pack &WL=WallIndex[Obj.ObjectID];
 switch(WL.Type)
 {
  case UO_TYPE_STATIC_WALL: StaticBilder->CreateWall(X,Y,Obj);break;
  case UO_TYPE_ROOF       : StaticBilder->CreateRoof(X,Y,Obj,WL);break;
 }
}

bool  CUOLoader::PrepareStatic(DWORD StartX,DWORD StartY,DWORD Width,DWORD Height,DWORD PlayerZ)
{
 UO_Static_object_uo StaticObj[0xFF];
 UO_OffsetFindingRecord OFS;
 OFS.X     =StartX;
 OFS.Y     =StartY;
 OFS.Width =StartX+Width;
 OFS.Height=StartY+Height;

 DWORD ObjectsNum;

 ClearWalls();
 StaticBilder->StartX=StartX;
 StaticBilder->StartY=StartY;
 
 for(DWORD iy=0;iy<Height;iy++)
 for(DWORD ix=0;ix<Width;ix++)
 {
 if(!GetBlockStatic(StartX+ix,StartY+iy,StaticObj,ObjectsNum)) continue;
 if(ObjectsNum>0xFF) continue;
 for(DWORD x=0;x<ObjectsNum;x++)
 {
  UO_Static_object_uo& Obj=StaticObj[x];
  Obj.ObjectID+=16384;
  if(Obj.ObjectID<0xFFFF)
  {
   if(WallIndex[Obj.ObjectID].Type)
   if(WallIndex[Obj.ObjectID].Height<300)
   if(Obj.Altitude<=PlayerZ)
    CreateWall(StartX+ix,StartY+iy,Obj);
  }
 }
 }
 // Wall information created
 // sort data and prepare for render
 DWORD LevelInformation[UO_NUM_LEVELS];
 if(PlayerZ==-1) PlayerZ=UO_NUM_LEVELS;
 PerformWallDecomposition(OFS,PlayerZ);
 return true;
}


void  CUOLoader::RenderData()
{
  DWORD x;
  for(x=0;x<NumLandBlocks;x++)
  {
   BindTex   (RenderBlocks[x]->TextureID);
   IdleRender((FVertex*)(LandVertexes+RenderBlocks[x]->VStart),(FTex*)LandVTex0);
  }

  DWORD VOff=0;
  DWORD IOff=0;
  for(x=0;x<STSC;x++)
  {
   BindTex   (StaticTextures[x]);
   IdleRender3((vertex_tex*)StaticVertexes+VOff,StaticVertexesIB+IOff,IBs[x]);
   VOff+=STS[x];
   IOff+=IBs[x];
  }

}

void CUOLoader::PrepareStaticRender()
{
 StaticCounter     =0;
 StaticBlockCounter=0;
 IBUsed            =0;
}

void CUOLoader::PerformWallDecomposition(UO_OffsetFindingRecord &OFS,DWORD PlayerZ)
{
 // 1. render object by types
 // 2. render packed
 // 3. if upper is roof, then up one side

 // so sort data Low 2 Hi
 // left 2 right
 // up down
/* DWORD WN=0;
 DWORD WNum=WallsNumber;
 for(DWORD y=OFS.Y;x<OFS.Height;x++)
  for(DWORD x=OFS.X;x<OFS.Width;x++)
   for(DWORD z=0;z<PlayerZ;z++)
    for(DWORD i=0;i<WNum;i++)
     {
      if(Walls[i].X
     }

*/
 PrepareStaticRender();
 for(DWORD x=0;x<WallsNumber;x++)
 {
  StaticBilder->BuildSurface(Walls[x]);
 }

}

