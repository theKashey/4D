#pragma once

#pragma pack(push,1)

struct UO_ARTIDXITEM
{
 DWORD Lookup;
 DWORD Size;
 DWORD Unknown;
};

struct UO_TEX_INDEX
{
 DWORD Lookup;
 char  SizeType;//1 - 64x64,2-128x128,0 - error
};

struct UO_ARTIDX_new
{
 DWORD Lookup;
 WORD  Size;
};

struct UO_TEXMAP_ITEM
{
 DWORD Flags;
 WORD  Texture;
 char  TileName[20];
};

struct UO_TEXMAP_new
{
 DWORD Texture;
 DWORD Type;
 DWORD Width;
 DWORD Height;
 DWORD Flags;
 char  TileName[20];
};

struct UO_TEXMAP_map
{
 WORD color;
 char level;
};

struct UO_TEXMAP_block
{
 DWORD      Cap;
 UO_TEXMAP_map Block[8*8];
};

struct UO_TEXMAP_block_new
{
 DWORD     Cap;
 UO_TEXMAP_map Block[23*23];
 void CreateFrom(UO_TEXMAP_block *Blocks)
 {
  Cap=0;
  for(DWORD iy=0;iy<4;iy++)
   for(DWORD ix=0;ix<4;ix++)
    for(DWORD y=0;y<8;y++)
     for(DWORD x=0;x<8;x++)
     {
      if(ix*8+x<24 && iy*8*23+y<24)
      Block[ix*8+x+iy*8*23+y]=Blocks[ix+iy*4].Block[x+y*8];
     }
 }
};

struct UO_TEX_EX
{
 DWORD Flags;
 BYTE Weight;
 BYTE Quality;
 WORD Unknown;
 BYTE Unknown1;
 BYTE Quantity;
 WORD AnimID;
 BYTE Unknown2;
 BYTE Hue;
 WORD Unknown3;
 BYTE Height;
 char TileName[20];
};

struct UO_TileStorageData
{
 DWORD TileData[44*44];
};

struct UO_Static_IDX_uo
{
 DWORD Start;
 DWORD Size;
 DWORD Unc;
};

struct UO_Static_IDX
{
 DWORD Start;
 WORD  Objects;
};

struct UO_Static_object_uo
{
 WORD ObjectID;// (add 16384 to offset it for RADARCOL.MUL)
 BYTE X;// Offset in block (0..7)
 BYTE Y;// Offset in block (0..7)
 BYTE Altitude;// (-128..127 units above/below sea level, like MAP0)
 WORD Hues;
};

#define UO_Static_object UO_Static_object_uo

struct UO_WallIndex_pack
{
 WORD  Type;       //0 - none,1-wall,2-wall with window...
 char  Orientation;//0 - both,1-left,2-right,3-undef(may be tail),4- error
 WORD  Width,Height,Depth;
 DWORD Lookup;
 BYTE File;
};

struct UO_WallIndex:public UO_WallIndex_pack
{
 DWORD TexID;
};


#define UO_NUM_TEXTURE_MIPMAPS 4

struct UO_TEX_LOOKUP_pack
{
 DWORD Lookup[UO_NUM_TEXTURE_MIPMAPS];
};

struct UO_TEX_LOOKUP:public UO_TEX_LOOKUP_pack
{
 DWORD TexID;
};


struct UO_block
{
 DWORD x;
 DWORD y;
 DWORD TextureID;
 int   Loaded;
 DWORD VStart;
 BYTE  Subdiv;
 BYTE  Priv;
};

struct UO_vertex
{
 WORD x,y,z;
};

struct UO_FTex
{
 float u,v;
};

struct UO_vertex_tex:public UO_vertex,public UO_FTex
{};

struct UO_OffsetFindingRecord
{
 DWORD X,Y,Width,Height;
};

struct UO_WallInfo
{
 WORD Type;
 char Orientation;
 WORD ObjID;
 WORD bX,bY;
 BYTE X,Y,Z;
};

struct CStaticRenderblocks
{
 DWORD VBBegin;
 DWORD IBBegin;
 DWORD VNum;
 DWORD TextureID;
};

struct UO_ObjectIndex
{
 DWORD ID;
 DWORD TypeID;
 DWORD Orientation;
 DWORD ExtraType;
};

#pragma pack(pop)



#define UO_NUM_TEXTURES 32768
#define UO_MAX_CASHED_BLOCKS 100
#define UO_MAX_VIEW_BLOCKS   100
#define UO_MAX_LAND_VERTEXES 10000
#define UO_MAX_NUMBER_STATIC_OBJECTS 0xFFFF
#define UO_NUM_LEVELS 0xFF
#define UO_MAX_STATIC_VERTEXES 8*0xFFFF
#define UO_MAX_STATIC_BLOCKS   0xFF
//10x10per block, 10x10 max blocks

typedef void(*PROGRESS_CALLBACK_FUNC)(DWORD, DWORD);

class CUO_StaticBuilder;

class CUOLoader
{
public:
 UO_ARTIDX_new      ARTREF[0xFFFF];
 UO_TEXMAP_new      TEXREF[UO_NUM_TEXTURES];
 DWORD              NumTexRefs;
 WORD               TileLoaded[0xFFFF];
 UO_TileStorageData TileData  [0x200];
 DWORD              UsedTiles;
 DWORD              fhArtMul;     //File Handle
 DWORD              fhTexMapMul;  //File Handle
 DWORD              fhWallTexFile[0xFF];//File Handle
 DWORD              fhMapMul;     //File Handle
 DWORD              fhStaticMul;  //File Handle
 DWORD              fhMipTexFile[UO_NUM_TEXTURE_MIPMAPS];
 UO_WallIndex_pack  WallIndex [0xFFFF];
 UO_Static_IDX      StaticIDX [512*768];
 UO_TEX_INDEX       TexIndex  [0x1000];

 UO_TEX_LOOKUP_pack TexMipIndex  [0x1000];
 UO_block           UOBlocks     [UO_MAX_CASHED_BLOCKS+1];
 UO_block          *RenderBlocks [UO_MAX_VIEW_BLOCKS  +1];
 UO_FTex            LandVTex0    [100];
 UO_vertex          LandVertexes [UO_MAX_LAND_VERTEXES];

 UO_vertex_tex       StaticVertexes   [UO_MAX_STATIC_VERTEXES];
 DWORD               StaticVertexesIB [UO_MAX_STATIC_VERTEXES*3];
 CStaticRenderblocks StaticRenderblocs[UO_MAX_STATIC_BLOCKS];
 DWORD               StaticTextures   [0xFFFF];
 WORD                STS[0xFFF];
 DWORD               STSC;
 WORD                IBs[0xFFF];
 DWORD               SIBSC;

 DWORD               UsedSTextures;
 DWORD               StaticCounter;
 DWORD               StaticBlockCounter;
 DWORD               IBUsed;

 DWORD              NumLandBlocks;
 UO_WallInfo        Walls[UO_MAX_NUMBER_STATIC_OBJECTS];
// UO_WallInfo        Walls_tmp[UO_MAX_NUMBER_STATIC_OBJECTS];
 DWORD              WallsNumber;
// WORD               WallsComposition[UO_MAX_CASHED_BLOCKS*UO_LAND_BLOCK_XSIZE*UO_LAND_BLOCK_XSIZE*UO_NUM_LEVELS];
// bool               ZLevelsUsed[UO_NUM_LEVELS];
 CUO_StaticBuilder *StaticBilder;


 void LoadArtIDX();
 void LoadTexIDX();
 void LoadWallIDX();
 void LoadStaticIDX();
 void LoadTexMapIDX();
 void LoadTMIDX();

 void OpenArtMul();
 void CloseArtMul();
 void OpenTexMapMul();
 void CloseTexMapMul();
 void OpenTexMipMul();
 void CloseTexMipMul();
 void OpenWallTexMul();
 void CloseWallTexMul();
 void OpenMapFiles();
 void CloseMapFiles();

 void ClearWalls();
 void CreateWall(DWORD X,DWORD Y,UO_Static_object_uo &Obj);

 void PrepareStaticArrays();
 void PrepareStaticRender();
public:
 CUOLoader();
 ~CUOLoader();
 void Clear();
 int  GetBlocks(UO_TEXMAP_block *Blocks,DWORD X,DWORD Y,DWORD Width,DWORD Height);
 void LoadDataInformation();
 bool GetTile           (DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type);
 bool GetTexMap         (DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type);
 bool GetTexMul         (DWORD **Data,DWORD TexID,DWORD Width);
 bool GetCompensatedTile(DWORD **Data,DWORD TexID,DWORD &Width,DWORD &Height,DWORD *Type);
 DWORD* GetWallTexture  (DWORD WID,DWORD &W,DWORD &H,DWORD &D);

 bool MapRegeneration(PROGRESS_CALLBACK_FUNC Prg);

 UO_TEXMAP_new *AccessTexRefs() { return TEXREF;}
 DWORD          GetTexRefCount(){ return NumTexRefs;}
 LPCSTR         ViewTileType(DWORD TID,LPSTR Buffer);
 bool           GetBlockStatic(DWORD X,DWORD Y,UO_Static_object_uo* Static,DWORD &ObjectNum);

 UO_block      *FindBlock(DWORD x,DWORD y,UO_OffsetFindingRecord &OFS);
 bool           PrepareGraphics(DWORD StartX,DWORD StartY,DWORD Width,DWORD Height);
 bool           PrepareStatic(DWORD StartX,DWORD StartY,DWORD Width,DWORD Height,DWORD PlayerZ);
 void           RenderData();

 void PerformWallDecomposition(UO_OffsetFindingRecord &OFS,DWORD PlayerZ);

 friend class CUO_StaticBuilder;
};

class CUO_StaticBuilder
{
  void CreateBox  (char Left,UO_WallInfo &WI,UO_WallIndex_pack &WL);
  void CreateRoofS(char Left,UO_WallInfo &WI,UO_WallIndex_pack &WL);
  void CreateRoofMidle(char Left,UO_WallInfo &WI,UO_WallIndex_pack &WL);
 public:
  DWORD            StartX;
  DWORD            StartY;
  CUOLoader        *Parent;
  CUO_StaticBuilder(CUOLoader* _Parent){ Parent=_Parent;}

  void CreateWallLeft(DWORD X,DWORD Y,UO_Static_object_uo &Obj);
  void CreateWallRight(DWORD X,DWORD Y,UO_Static_object_uo &Obj);


  void CreateWall(DWORD X,DWORD Y,UO_Static_object_uo &Obj);
  void CreateRoof(DWORD X,DWORD Y,UO_Static_object_uo &Obj,UO_WallIndex_pack &WL);


  void BuildWall   (UO_WallInfo &WI,UO_WallIndex_pack &WL);
  void BuildRoof   (UO_WallInfo &WI,UO_WallIndex_pack &WL);  
  void BuildSurface(UO_WallInfo &WI);

  friend class CUOLoader;
};

#define UO_TYPE_TILE 1
#define UO_TYPE_RUN  2
#define UO_TYPE_ANY  3


#define UO_FLAG_Background       0x00000001
#define UO_FLAG_Weapon           0x00000002
#define UO_FLAG_Transparent      0x00000004
#define UO_FLAG_Translucent      0x00000008
#define UO_FLAG_Wall             0x00000010
#define UO_FLAG_Damaging         0x00000020
#define UO_FLAG_Impassable       0x00000040
#define UO_FLAG_Wet              0x00000080
#define UO_FLAG_Unknown          0x00000100
#define UO_FLAG_Surface          0x00000200
#define UO_FLAG_Bridge           0x00000400
#define UO_FLAG_GenericStackable 0x00000800
#define UO_FLAG_Window           0x00001000
#define UO_FLAG_NoShoot          0x00002000
#define UO_FLAG_PrefixA          0x00004000
#define UO_FLAG_PrefixAn         0x00008000
#define UO_FLAG_Internal         0x00010000
#define UO_FLAG_Foliage          0x00020000
#define UO_FLAG_PartialHue       0x00040000
#define UO_FLAG_Unknown1         0x00080000
#define UO_FLAG_Map              0x00100000
#define UO_FLAG_Container        0x00200000
#define UO_FLAG_Wearable         0x00400000
#define UO_FLAG_LightSource      0x00800000
#define UO_FLAG_Animated         0x01000000
#define UO_FLAG_NoDiagonal       0x02000000
#define UO_FLAG_Unknown2         0x04000000
#define UO_FLAG_Armor            0x08000000
#define UO_FLAG_Roof             0x10000000
#define UO_FLAG_Door             0x20000000
#define UO_FLAG_StairBack        0x40000000
#define UO_FLAG_StairRight       0x80000000

#define UO_ORIENTATION_BOTH      0
#define UO_ORIENTATION_XLEFT      1
#define UO_ORIENTATION_XRIGHT     2
#define UO_ORIENTATION_YLEFT      3
#define UO_ORIENTATION_YRIGHT     4
#define UO_ORIENTATION_XMIDLE     5
#define UO_ORIENTATION_YMIDLE     6


#define UO_TYPE_STATIC_WALL          1
#define UO_TYPE_STATIC_WALL_WINDOW   2

#define UO_TYPE_ROOF_XRIGHT           3
#define UO_TYPE_ROOF_XLEFT            4
#define UO_TYPE_ROOF_YRIGHT           5
#define UO_TYPE_ROOF_YLEFT            6
#define UO_TYPE_ROOF_XMIDLE           7
#define UO_TYPE_ROOF_YMIDLE           8


#define UO_TYPE_ROOF                  3

extern WORD UO_StdOffX;
extern WORD UO_StdOffY;
extern WORD UO_OffX;
extern WORD UO_OffY;
extern WORD UO_OffZ;


//#endif
