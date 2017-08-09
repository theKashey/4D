#pragma once

class CBlopLoader: public CTerrainHolder
{
    DWORD FileHandle;
	GString MapFile;
public:
	CBlopLoader(void);
	~CBlopLoader(void);
	bool Load(GString Name);
	void Unload();
	void LoadBlock(CTerrainRawBlock &Block,int x,int y);
	void LoadBlockRow(CTerrainRawBlock *Block,int x,int y,int Num);

	LPCSTR GetMapFile();
	void UploadMarked();
};
