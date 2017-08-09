#include "ProjectSpace.h"
#include "ProjectGraphic.h"

CCamera *CWorld::Camera(){return CGenericGraphic::Camera;}

CWorld *MyWorld;

void InitWorld()
{
	MyWorld=new CWorld();
}

PWorld DLLTYPE World(){return MyWorld;}