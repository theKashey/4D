#include "projectspace.h"
#include "projectgraphic.h"

namespace Draw
{

DWORD      GetMaterial        (LPCSTR Name)
{
	return 0;
}

CMaterial CMaterialDummy;
rCMaterial GetMaterialByID    (DWORD ID)
{
	return CMaterialDummy;
}


pCMaterial pGetMaterialByID    (DWORD ID)
{
	return &CMaterialDummy;
}

};