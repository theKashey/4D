#pragma once
#ifndef _COUNTERS_H
#define _COUNTERS_H

#define DC_MAIN_CYCLE 1
#define DC_FPS        2
#define DC_PHC        3
#define DC_NAC        4

void CycleReset();
void InitCycles();

void  DLLTYPE DetectCycle(WORD Counter);
float DLLTYPE GetCounter(WORD Counter,float MaxDelt=0);

extern float c_PPS;
typedef float *PFLOAT ;
PFLOAT DLLTYPE Get_c_PPS();

namespace Counters
{
	double DLLTYPE GetTime();
};

#endif
