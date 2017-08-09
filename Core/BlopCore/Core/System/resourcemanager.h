#pragma once

class CResourceManager
{
public:
	CResourceManager(void);
	~CResourceManager(void);
};

DWORD TouchResourse(LPCSTR Name);
void  FreeResourse (DWORD ID);
void  UseResourse  (DWORD ID);

