#pragma once

class CTerrainHolder : public Draw::CDrawableObject
{
protected:
	DWORD MX, MY ,MA;
public:
	bool  TranslateI(DWORD i,DWORD &x,DWORD &y);
	DWORD GetMA(){return MA;}
    DWORD SetSizes(DWORD X,DWORD Y);
	bVector3f Sizes;
	BYTE  *MARKS;
	CTerrainHolder(void);
	~CTerrainHolder(void);
	void    MarkAll(BYTE Value);

	virtual bool Load(GString Name){return false;}

	virtual void SaveAsDefault(GString FileName){};
	virtual LPCSTR GetMapFile(){return NULL;}
	virtual void UploadMarked(){};

	virtual void LoadBlock(CTerrainRawBlock &Block,int x,int y){};
	virtual void LoadBlockRow(CTerrainRawBlock *Block,int x,int y,int Num){};
};
