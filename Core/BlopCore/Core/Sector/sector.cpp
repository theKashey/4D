//---------------------------------------------------------------------------




#include "ProjectSpace.h"
#include "Sector.h"

//---------------------------------------------------------------------------
#define CMapObject_GetObject_ObjectID(RetClass,RuleClass,FuncName,IDName,CurrentObject,CasheObject,ArrayObject) \
RetClass *RuleClass::FuncName(DWORD IDName,WORD LoadMode)                                                       \
{                                                                                                               \
 blp_ASSERT(CurrentObject);                                                                                     \
 if(CurrentObject->ID==IDName) return CurrentObject;                                                            \
 else                                                                                                           \
 {                                                                                                              \
  RetClass *Object;                                                                                             \
  Object=CasheObject.CasheFind(IDName);                                                                         \
  if(!Object)                                                                                                   \
   Object=ArrayObject.FindByID(IDName);                                                                         \
  if(LoadMode==MAP_LOAD)                                                                                        \
   if(Object->Load()) return NULL;                                                                              \
  return CurrentObject=Object;                                                                                  \
 }                                                                                                              \
}

CMapObject_GetObject_ObjectID(CMapWorld ,CMapManager,GetWorld ,WorldID ,CurrentWorld ,WorldsCashe,WorldsIn);
//CMapObject_GetObject_ObjectID(CMapRegion,CMapWorld  ,GetRegion,RegionID,CurrentRegion,RegionCashe,RegionsIn);

//bool       CMapRegion::Load(){ return false;}
//CMapSector CMapRegion::*GetSector(bVector2s SectorID,WORD LoadMode){ return false;}

//CMapBlock  CMapSector::*GetBlock(bVector2s BlockID,WORD LoadMode){ return false;}


//---------------------------------------------------------------------------
WORD CMapManager::LoadBlock  (DWORD WorldID,DWORD RegionID,bVector2s SectorID,bVector2s BlockID)
{
 CMapWorld   *World =        GetWorld (WorldID ,MAP_LOAD);
 if(!World)   return 1;
 CMapRegion  *Region=World ->GetRegion(RegionID,MAP_LOAD);
 if(!Region)  return 2;
 CMapSector  *Sector=Region->GetSector(SectorID,MAP_LOAD);
 if(!Sector)  return 3;
 CMapBlock   *Block =Sector->GetBlock (BlockID ,MAP_LOAD);
 if(!Block)   return 4;
 return 0;
}

WORD CMapManager::UnLoadBlock(DWORD WorldID,DWORD RegionID,bVector2s SectorID,bVector2s BlockID)
{
 CMapWorld   *World =        GetWorld (WorldID ,MAP_UNLOAD_NULL_LOWER_BOTTOM);
 if(!World)   return 1;
 CMapRegion  *Region=World ->GetRegion(RegionID,MAP_UNLOAD_NULL_LOWER_BOTTOM);
 if(!Region)  return 2;
 CMapSector  *Sector=Region->GetSector(SectorID,MAP_UNLOAD_NULL_LOWER_BOTTOM);
 if(!Sector)  return 3;
 CMapBlock   *Block =Sector->GetBlock (BlockID ,MAP_UNLOAD);
 if(!Block)   return 4;
 if(Sector->CheckStatus()==MAP_EMPTY)
    if(Region->CheckStatus()==MAP_EMPTY)
      if(World->CheckStatus()==MAP_EMPTY)
        CheckStatus();
 return 0;
}
