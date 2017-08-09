#include "StdAfx.h"
#include "Terrain.h"
#include "terrainholder.h"

CTerrainHolder::CTerrainHolder(void)
{
	MARKS=NULL;
	Sizes.Set(10,10,10);
	MX=MY=0;
}

CTerrainHolder::~CTerrainHolder(void)
{
	if(MARKS) delete []MARKS;
}


void    CTerrainHolder::MarkAll(BYTE Value)
{
	if(!Value) ZeroMemory(MARKS,MX*MY);
	else
	{
		BYTE *M=MARKS;
		for(DWORD i=0;i<MA;i++,M++)
			*M=Value;
	}
}

DWORD  CTerrainHolder::SetSizes(DWORD X,DWORD Y)
{
	MX=X;
	MY=Y;
	MA=X*Y;
    MARKS=new BYTE[MA];
	return MA;
}

bool  CTerrainHolder::TranslateI(DWORD i,DWORD &x,DWORD &y)
{
 x=y=0;
 if(i>MA) return false;
 y=i/MX;
 x=i-y*MX;
 return true;
}