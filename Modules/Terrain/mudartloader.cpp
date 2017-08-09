#include "StdAfx.h"
#include "Terrain.h"
#include "terrainholder.h"
#include "mudartloader.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>


CMudArtLoader::CMudArtLoader(void)
{
	VOut=NULL;
	Index=NULL;
	Heights=NULL;
}

CMudArtLoader::~CMudArtLoader(void)
{
	if(VOut )   delete []VOut;
	if(Index)   delete []Index;
	if(Heights) delete []Heights;
}

DWORD FileSize(int F)
{
	DWORD size=lseek(F,0,SEEK_END);
	lseek(F,0,SEEK_SET);
	return size;
}

bool CMudArtLoader::LoadMap(LPCSTR FileName)
{
	DWORD F=bOpenFile(FileName,"r",true);
	if(!F) return false;
	DWORD S=bTellFile(F);
    Width=Height=sqrt(S/4);
    VOut=new CMudVertex[Width*Height];
    Heights=new float[Width*Height];
    float *Line=new float[Width];
    float FacX=25/128.0;
    float FacY=FacX;
    float FacZ=10;//FacY;

    float shiftx=-FacX*Width/2.0;
    float shifty=-FacY*Height/2.0;
    CMudVertex *V=VOut;
    DWORD ix=0;
    for(DWORD y=0;y<Height;y++)
    {
	 bReadFile(Line,sizeof(*Line)*Width,1,F);
	 float  *L=Line;
	 for(DWORD i=0;i<Width;i++,L++,V++)
	 {
		if(ix==0)_MIN=_MAX=*L;
		if(*L>_MAX) _MAX=*L;
		if(*L<_MIN)_MIN=*L;
		Heights[ix++]=*L;
		V->Vertex[0]=shiftx+i*FacX;
    	V->Vertex[1]=*L*FacZ;
		V->Vertex[2]=shiftx+y*FacY;
		V->Color[0]=V->Color[1]=V->Color[2]=512*(*L);
	 }
	}
    Index=new DWORD[Width*Height*6];
  	IndexSize=Width*Height*6;
	DWORD i=0;
	for(DWORD y=0;y<Height-1;y++)
	 for(DWORD x=0;x<Width-1;x++)
	 {
		 Index[i++]=(x+0)+(y+0)*Width;
		 Index[i++]=(x+0)+(y+1)*Width;
		 Index[i++]=(x+1)+(y+1)*Width;

		 Index[i++]=(x+1)+(y+1)*Width;
		 Index[i++]=(x+1)+(y+0)*Width;
		 Index[i++]=(x+0)+(y+0)*Width;
	 }
    IndexSize=i;
	bCloseFile(F);
	return true;
}

void CMudArtLoader::SaveAsDefault(GString FileName)
{
	int F=_open(FileName.c_str(),O_CREAT|O_TRUNC | _O_WRONLY |O_BINARY,_S_IREAD |_S_IWRITE);
	CTerrainRawBlock Block;
	Block.TerrainSet=0;
	Block.Scale.Set(1,1,1);
	DWORD BX=Width/T_BLOCK_PX;
	DWORD BY=Height/T_BLOCK_PY;
	DWORD T_BLOCK_PY1=T_BLOCK_PY-1;
	DWORD T_BLOCK_PX1=T_BLOCK_PX-1;
	_write(F,&BX,4);
	_write(F,&BY,4);
	DWORD nb=0;
	DWORD x,y;
	for(y=0;y<Width-1;y+=T_BLOCK_PY)
	{
		for(x=0;x<Height-1 ;x+=T_BLOCK_PX)
		{
			int i=0;
			float MAX,MIN;
			MIN=_MAX;
			MAX=_MIN;
			//find max\min
			for(int iy=0;iy<T_BLOCK_PY;iy++)
				for(int ix=0;ix<T_BLOCK_PX;ix++)
				{
					float H=Heights[x+ix+(y+iy)*Width];
					if(H<MIN) MIN=H;
					if(H>MAX) MAX=H;
				}			
            float MM=0xFFFF/(float)(MAX-MIN);
			if(MAX==MIN)MM=1;
			MM=1;
			MIN=0;
			for(int iy=0;iy<T_BLOCK_PY;iy++)
				for(int ix=0;ix<T_BLOCK_PX;ix++)
				{
					float H=Heights[x+ix+(y+iy)*Width];
					Block.Elements[i].Height=H;//(H-MIN)*MM;
					Block.Elements[i].Texture=0;
					i++;
				}
				Block.Scale[0]=Block.Scale[2]=1;
				Block.Scale[1]=1;//1/MM;//(MAX-MIN)/(float)0xFF;
				Block.startZ=MIN;
				DWORD bs=sizeof(Block);
				bs=sizeof(Block.Elements);
				bs=sizeof(Block)-bs;
                _write(F,&Block,sizeof(Block));
				nb++;
		}

	}
		_close(F);
		nb=0;
}