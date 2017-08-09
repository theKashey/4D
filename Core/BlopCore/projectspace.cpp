/*===========================================================================
    Заголовок: PrjectSpace.cpp
    Автор: Kashey
    Информация: MainHeader 

===========================================================================*/
//---------------------------------------------------------------------------


#include "ProjectSpace.h"



bVector3f _v3ONE(1,1,1);
bVector4f _v4ONE(1,1,1,1);

const bVector3f &v3ONE_(){return _v3ONE;}
const bVector4f &v4ONE_(){return _v4ONE;}

void DLLTYPE CallMyPCHInclude()
{
};

HANDLE shMainThread=0;


HANDLE DLLTYPE QueryMainThreadHandle(){ return shMainThread;}
//---------------------------------------------------------------------------
