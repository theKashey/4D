#include "ProjectSpace.h"
//#include "blopstriper.h"
#include "ProjectGraphic.h"
// #include "../PerfomanceCheck\NVTriStrip\include\NvTriStrip.h"


void RenderTech::IndexToList( WORD *IndexesIn,DWORD &NumIndexes)
{
	PrimitiveGroup* Groups;
	unsigned short  numGroups;
	SetListsOnly(true);
	SetCacheSize(CACHESIZE_GEFORCE3);
	GenerateStrips(IndexesIn,NumIndexes,&Groups,&numGroups,false);
	ASSERT(numGroups==1);
	memcpy(IndexesIn,Groups[0].indices,Groups[0].numIndices*sizeof(IndexesIn[0]));
	NumIndexes=Groups[0].numIndices;
}

void RenderTech::IndexToStrip( WORD *IndexesIn,DWORD &NumIndexes)
{
	PrimitiveGroup* Groups;
	unsigned short  numGroups;
	SetListsOnly(false);
	SetStitchStrips(true);
	SetCacheSize(CACHESIZE_GEFORCE3);
	GenerateStrips(IndexesIn,NumIndexes,&Groups,&numGroups,false);
	ASSERT(numGroups==1);
	memcpy(IndexesIn,Groups[0].indices,Groups[0].numIndices*sizeof(IndexesIn[0]));
	NumIndexes=Groups[0].numIndices;
}
