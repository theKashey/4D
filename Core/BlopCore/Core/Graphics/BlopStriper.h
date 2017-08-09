#ifndef _BLOP_STRIPER_H_
#define	_BLOP_STRIPER_H_
#pragma once

//void DLLTYPE IndexToList(DWORD *IndexesIn,DWORD &NumIndexes);
void DLLTYPE IndexToList( WORD *IndexesIn,DWORD &NumIndexes);

//void DLLTYPE IndexToStrip(DWORD *IndexesIn,DWORD &NumIndexes);
void DLLTYPE IndexToStrip( WORD *IndexesIn,DWORD &NumIndexes);

#include "../../Utils/TriStriper/NvTriStrip.h"


#endif
