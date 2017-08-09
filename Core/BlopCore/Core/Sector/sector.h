//---------------------------------------------------------------------------

#ifndef SectorH
#define SectorH
//---------------------------------------------------------------------------

//All map divided by worlds
//worlds divided by sectors
//sectors divided by regions
//regions divided by blocks
//blocks divided by 16x16 tiles

#define BLOP_TILES_PER_SECTOR 256
#define BLOP_TILES_PACK       8

#define MAP_EMPTY                               0
#define MAP_LOAD                                1   // load data
#define MAP_UNLOAD                              2   // unload data
#define MAP_UNLOAD_NULL_LOWER_BOTTOM        3   // be ready to unload bottom data

template <class T>
class SmartMapVector:public SmartVector<T>
{
 public:
  inline T FindByID(DWORD ID){ for(int x=0;x<_size;x++) if(data[x]->ID==ID)return data[x]; return NULL;}
};

template <class T>
class SmartMapCashe:public SmartMapVector<T>,public SmartCasheBase<T>
{
public:
  inline T CasheFind(DWORD ID)
  {
   for(int x=0;x<_size;x++)
    if(data[x]->ID==ID)
    {
     T &R=data[x];
     return R;
    }
     return NULL;
  }
};

struct CMapTile
{
 WORD  Height;
 DWORD TileInformation;
};

class CMapWorld;
class CMapSector;
class CMapRegion;
class CMapBlock;

struct CMapObjectPackInformation
{
 WORD  FileID;
 DWORD ObjectID;
 DWORD Offset;
};

struct CMapObjectInformation
{
 DWORD ID;
 WORD  Usage;
 DWORD ShemeID;
 DWORD SchemeX,SchemeY,SchemeZ;
 DWORD Icon;
 char  Name[0xFF];
 WORD  NumObjectsIn;
 CMapObjectPackInformation Objects[1];
};

struct CMapQuadMap
{
 bVector2d Start,End;
 bool *Holes;
};

class CMapContainer
{
public:
 DWORD ID;
 DWORD Loading;
};

class CMapWorld    : public CMapContainer
{
public:
 GString                     Name;
 SmartMapVector<CMapRegion*> RegionsIn;
 SmartMapCashe <CMapRegion*> RegionCashe;
 CMapRegion                 *CurrentRegion;

 bool                    Load();//{ return false;}
 CMapRegion             *GetRegion(DWORD RegionID,WORD LoadMode){return NULL;}
 DWORD                   CheckStatus(){ return 0;}
};

class CMapRegion   : public CMapContainer
{
public:
 CMapQuadMap                 RegionDimensionMap;
 SmartMapVector<CMapSector*> SectorsIn;
 SmartMapCashe <CMapSector*> SectorCashe;
 CMapSector                 *CurrentSector;
 bool                       Load(){return false;}
 CMapSector                *GetSector(bVector2s SectorID,WORD LoadMode){ return NULL;}
 DWORD                      CheckStatus(){ return 0;}
};

class CMapSector   : public CMapContainer
{
public:
 bVector3s                   Dimensions;
 CMapBlock                  *Blocks;
 CMapBlock                  *GetBlock(bVector2s BlockID,WORD LoadMode){return NULL;}
 DWORD                       CheckStatus(){ return 0;}
};


class CMapBlock    : public CMapContainer
{
public:
 CMapTile *Tiles;
 //no static
 bool               Load(){return NULL;}
 DWORD              CheckStatus(){ return 0;}
};


class CMapManager
{
public:
 SmartMapVector<CMapWorld*> WorldsIn;
 SmartMapCashe<CMapWorld*>  WorldsCashe;

 CMapWorld *CurrentWorld;

 CMapWorld *GetWorld(DWORD WorldID,WORD LoadMode);


 WORD LoadBlock  (DWORD World,DWORD Region,bVector2s Sector,bVector2s Block);
 WORD UnLoadBlock(DWORD World,DWORD Region,bVector2s Sector,bVector2s Block);

 DWORD CheckStatus(){ return 0;}
};

#endif
