#include "stdafx.h"
#include "QuestScene.h"


/*void CQuestActive::Load(LPCSTR _FName)
{
	GString FName=_FName;
   //load texture map
	if(Data)delete []Data;
	u_Texture_Loader Loader;
	Loader.LoadFromFile(FName+"active.tga");
	Data=new BYTE[Loader.Info.ImageSize];
	memcpy(Data,Loader.Info.pData,Loader.Info.ImageSize);
	bpp=Loader.Info.bpp/8;

	//end texture map
}
DWORD CQuestActive::ColorAtCursor(const DWORD x,const DWORD y)
{
	if(x<1024 && y<1024)
	{
		const DWORD line=(x+y*1024)*bpp;
		return Data[line]+(Data[line+1]<<8)+(Data[line+2]<<16);
	}
	else
		return 0;
}

DWORD  CQuestActive::ElemAtCursor(const DWORD x,const DWORD y,CQuestActiveElement &AE)
{
    return 0;
	AE.Color=ColorAtCursor(x,y);
	if(lua::getVdword("quest.stock.active.key[%d]",AE.Color))
	{
		AE.Hint  =lua::getVstring("quest.stock.active.value[%d].hint",AE.Color);
		AE.Mouse =lua::getVdword ("quest.stock.active.value[%d].mouse",AE.Color);
		AE.Script=lua::getVstring("quest.stock.active.value[%d].script",AE.Color);
        DWORD C=AE.Color;
	    AE.Color=0;
	    return C;
	}
	return 0;	
}
*/