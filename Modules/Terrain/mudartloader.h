#pragma once

struct CMudVertex
{
	float Vertex[3];
	BYTE  Color[4];
};

class CMudArtLoader: public CTerrainHolder
{
	CMudVertex *VOut;
	DWORD *Index;
	DWORD IndexSize;
	float *Heights;
	DWORD Width,Height;
	float _MIN,_MAX;
public:
	CMudArtLoader(void);
	~CMudArtLoader(void);

	bool ConvertMap(LPCSTR FileName);
	bool LoadMap(LPCSTR FileName);


	bool Load(GString Name){return LoadMap(Name);}
	void SaveAsDefault(GString FileName);
};
