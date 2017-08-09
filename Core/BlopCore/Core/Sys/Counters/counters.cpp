/*===========================================================================
    Заголовок: Counters.cpp
    Автор: Kashey
    Информация: Engine counting

===========================================================================*/
#include "ProjectSpace.h"
#include "counters.h"

using namespace Counters;

float thisTime;

float c_PPS=0;

PFLOAT Get_c_PPS(){return &c_PPS;}

double PerformanceFrequency=0;
LARGE_INTEGER _PerformanceFrequency;
LARGE_INTEGER _Time,_StartTime;

void SetTime(LARGE_INTEGER &Time)
{
	QueryPerformanceCounter(&Time);
}

double Counters::GetTime()
{
  QueryPerformanceCounter(&_Time);
  return PerformanceFrequency *(_Time.QuadPart) ;
}

double GetTime(LARGE_INTEGER StartTime)
{
  QueryPerformanceCounter(&_Time);
  return  PerformanceFrequency*(_Time.QuadPart -StartTime.QuadPart) ;
}



void CycleReset()
{
	//thisTime=GetTime();
}

struct CycleInfo
{
	LARGE_INTEGER StartTime;
	LARGE_INTEGER _StartTime;
	double        CurrentValue,CV2,LastValue,TempValue;//[COUNTERS_STORAGE_TIME];
	float         Count;
    CycleInfo()
    {
		StartTime.QuadPart=0;
		CurrentValue=LastValue=TempValue=0;
	}
};

CycleInfo Cycles[0xFF];

#define COUNTERS_STORAGE_TIME  0

void InitCycles()
{
  QueryPerformanceFrequency(&_PerformanceFrequency);
  PerformanceFrequency=1.0/_PerformanceFrequency.QuadPart;
  QueryPerformanceCounter(&_StartTime);
  ZeroMemory(Cycles,sizeof(Cycles));
}

void DetectCycle(WORD Counter)
{
	if(Cycles[Counter].StartTime.QuadPart==0)
	{
		SetTime(Cycles[Counter].StartTime);
		SetTime(Cycles[Counter]._StartTime);
	}
	Cycles[Counter].TempValue+=GetTime(Cycles[Counter].StartTime);
	Cycles[Counter].Count+=1;

	if(Cycles[Counter].Count>COUNTERS_STORAGE_TIME)
	{
	 Cycles[Counter].LastValue=Cycles[Counter].CurrentValue;
	 //Cycles[Counter].CurrentValue=Cycles[Counter].TempValue  /Cycles[Counter].Count;
	 Cycles[Counter].CurrentValue =GetTime(Cycles[Counter]._StartTime)/Cycles[Counter].Count;
	 SetTime(Cycles[Counter]._StartTime);
	 Cycles[Counter].TempValue=0;
	 Cycles[Counter].Count    =0;
	}
	SetTime(Cycles[Counter].StartTime);
}

float GetCounter(WORD Counter,float MaxDelta)
{
	float CV= Cycles[Counter].CurrentValue;
	if(MaxDelta>0.0001 && Cycles[Counter].LastValue>0.00001)
	{
     if (CV > Cycles[Counter].LastValue *(1+MaxDelta)) CV = Cycles[Counter].LastValue *(1+MaxDelta);else
     if (CV < Cycles[Counter].LastValue *(1-MaxDelta)) CV = Cycles[Counter].LastValue *(1-MaxDelta);
	}
    //Cycles[Counter].LastValue=CV;
	return CV;
}
