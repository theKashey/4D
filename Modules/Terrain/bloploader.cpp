#include "StdAfx.h"
#include "Terrain.h"
#include "terrainholder.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "bloploader.h"


CBlopLoader::CBlopLoader(void)
{
	FileHandle=0;
	MapFile="";
}

CBlopLoader::~CBlopLoader(void)
{
	Unload();
}

LPCSTR CBlopLoader::GetMapFile()
{
	return MapFile.c_str();
}

bool CBlopLoader::Load(GString Name)
{
	MapFile=Name;
	FileHandle=_open(Name.c_str(),O_BINARY,_S_IREAD);
	DWORD W,H;
	if(FileHandle<1) return false;
	_read(FileHandle,&W,4);
    _read(FileHandle,&H,4);
	SetSizes(W,H);
	return true;
}
void CBlopLoader::Unload()
{
	if(FileHandle) _close(FileHandle);
	FileHandle=0;
}
void CBlopLoader::LoadBlock(CTerrainRawBlock &Block,int x,int y)
{
	lseek(FileHandle,8+(x+y*(MX-1))*sizeof(Block),SEEK_SET);
	_read(FileHandle,&Block,sizeof(Block));
}

void CBlopLoader::LoadBlockRow(CTerrainRawBlock *Block,int x,int y,int Num)
{			 
	lseek(FileHandle,8+(x+y*(MX-1))*sizeof(Block),SEEK_SET);
	_read(FileHandle,Block,sizeof(Block)*Num);
}

void CBlopLoader::UploadMarked()
{
};

