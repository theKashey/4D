#ifndef __BLOP_IMPORT
#define __BLOP_IMPORT


#ifndef DLLTYPE
#define DLLTYPE _declspec(dllexport)
#endif

#include "StartUpInfo.h"

//typedef bool(*InitBlopFunc)(BlopStartInformation&);
//typedef bool(*BlopTickFunc

bool DLLTYPE InitBlop(BlopStartInformation &BST);
bool DLLTYPE StartBlop();
bool DLLTYPE BlopTick();
void DLLTYPE BlopEnd();

#endif
