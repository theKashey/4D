#include "ProjectSpace.h"
#include "ProjectGraphic.h"

#ifdef bLINK

namespace Textures
{
	DWORD Textures::LoadTexture  (LPCSTR Name)
	{
		return 0;
	}
	bool  Textures::UnloadTexture(DWORD ID)
	{
		return false;
	}

	bool  Textures::BindTexture  (DWORD ID)
	{
		return false;
	}

	GString Textures::GetTextureStat(DWORD MODE)
	{
		return "";
	}
    DWORD Textures::GetInerTexture(DWORD ID)
    {return 0;}
};

#endif