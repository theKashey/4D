/*===========================================================================
    Заголовок: Prifiler.cpp
    Автор: Kashey
    Информация: Profiles and guards

===========================================================================*/
#include "ProjectSpace.h"
#include "ProjectCounter.h"
#include "profiler.h"

#define MAX_PROFILE_SIZE 1024

using namespace _Profilers;



typedef SmartVector<ProfilerSection> _ProfilerSections; 

char Guard[1024];
_ProfilerSections *ProfilerSections=NULL;
_ProfilerSections *SafePS;

_ProfilerSections *SectionProfilerSections=NULL;
_ProfilerSections *SafeSECPS;



#define PF_ACTION_ENTER 1
#define PF_ACTION_LEAVE 2

ProfilerProgress Profile[MAX_PROFILE_SIZE];
DWORD CurrentSet=0;

DWORD SYS_PROFILE=0;
time_stamp ProfileStartTime,ProfileEndTime,ProfileEndTime2;
double ProfileStartTTime,ProfileEndTTime,ProfileEndTTime2;



__forceinline _Profilers::time_stamp GetTimeStamp () 
{
	__asm rdtsc;
};

SmartVector<_Profilers::CFormatedProgress> PVector;

bool prof_CashedTree=false;

void EndProfile()
{
  ProfileEndTime=GetTimeStamp();
  ProfileEndTTime=Counters::GetTime();
}

void StartProfile()
{
  if(!SYS_PROFILE)
      RegisterNewConsoleClass(SYS_PROFILE,"PROFILER",-1);
  if(!ProfilerSections)
	  ProfilerSections=new _ProfilerSections;
  if(!SectionProfilerSections)
	  SectionProfilerSections=new _ProfilerSections;
  //SafePS=ProfilerSections;
  prof_CashedTree=false;
  if(PVector._Allocated()==0) PVector.SetSize(100);
  PVector.SetSize(GetFormatedProfileTree(PVector.begin(),PVector._Allocated()),false);
  prof_CashedTree=true;
  CurrentSet=0;
  ProfileStartTime =GetTimeStamp();
  ProfileStartTTime=Counters::GetTime();
  for(_ProfilerSections::iterator it=SectionProfilerSections->begin();it!=SectionProfilerSections->end();it++)
      if(it->Ref)((CSectionProfile*)it->Ref)->Reset();
}

DWORD AddAction(const DWORD ID,const DWORD Action,const time_stamp UsedTime)
{
	if(CurrentSet>MAX_PROFILE_SIZE)return MAX_PROFILE_SIZE+1;
	Profile[CurrentSet].ID       =ID;
	Profile[CurrentSet].Action   =Action;
	Profile[CurrentSet].UsedTime =UsedTime;
	//Profile[CurrentSet].StartTime=StartTime;
	CurrentSet++;
	return CurrentSet-1; 
}

void UpdateAction(const DWORD ID,const DWORD Action,const time_stamp UsedTime)
{
	if(ID>MAX_PROFILE_SIZE)return;
	Profile[ID].Action   =Action;
	Profile[ID].UsedTime =UsedTime;
	//Profile[ID].StartTime=StartTime; 
}

DWORD CreateProfiler(LPCSTR Name,CProfiler *ref)
{
	if(!ProfilerSections)
	  ProfilerSections=new _ProfilerSections;
	ProfilerSection PS;
	PS.ID=ProfilerSections->size();
	PS.Name=Name;
	PS.Ref =ref;
	ProfilerSections->push_back(PS);
	return PS.ID;
}

void DeleteProfiler(DWORD ID)
{
  for(_ProfilerSections::iterator it=ProfilerSections->begin();it!=ProfilerSections->end();it++)
      if(it->ID==ID)
          {
            it->Ref=NULL;
            it->Name.Empty();
            return;
          }
};

DWORD CreateSectionProfile(LPCSTR Name,CSectionProfile *ref)
{
	if(!SectionProfilerSections)
	  SectionProfilerSections=new _ProfilerSections;
	ProfilerSection PS;
	PS.ID=SectionProfilerSections->size();
    if(Name!=NULL)
	 PS.Name=Name;
	PS.Ref =(CProfiler*)ref;
	SectionProfilerSections->push_back(PS);
	return PS.ID;
}

void DeleteProfilerSec(DWORD ID)
{
  for(_ProfilerSections::iterator it=SectionProfilerSections->begin();it!=SectionProfilerSections->end();it++)
     if(it->ID==ID)
     {
      it->Ref=NULL;
      it->Name.Empty();
      return;
     }
};

CProfiler::CProfiler(LPCSTR Name)
{
	ID=CreateProfiler(Name,this);
}
CProfiler::~CProfiler(void)
{
  if(ID)
	DeleteProfiler(ID);
  ID=0;
}

void CProfiler::Enter()
{
	StartTime=GetTimeStamp();
	MyID=AddAction(ID,PF_ACTION_ENTER,0);
}

void CProfiler::Leave()
{
	const time_stamp EndTime =GetTimeStamp();
	const time_stamp UsedTime=EndTime-StartTime;
	AddAction(ID,PF_ACTION_LEAVE,UsedTime);
	UpdateAction(MyID,PF_ACTION_ENTER,UsedTime);
}


CSectionProfile::CSectionProfile(LPCSTR Name)
{
    ID=CreateSectionProfile(Name,this);
    Locked=0;
    Reset();
}
CSectionProfile::~CSectionProfile()
{    
     if(ID)
       DeleteProfilerSec(ID);
     ID=0;
}
void CSectionProfile::Enter()
{
    if( Locked) return;
    StartTime=GetTimeStamp();
    Locked++;
}
void CSectionProfile::Leave()
{
     Locked--;
     if(Locked)return;
	_Profilers::time_stamp EndTime=GetTimeStamp();
     TotalTime+=EndTime-StartTime;
     StartTime=EndTime;
}
    
void CSectionProfile::Reset()
{
     UsedNow  =0;
     TotalTime=0;
     Locked   =0;
}

//float FrameTotalTime=0;

void PrintProfiler(GString *Out)
{
	/*char Shift[0xFF];
	float TotalTime=0;
	time_stamp UsedTime=GetTimeStamp()-ProfileStartTime;
	float UsetTTime=Counters::GetTime()-ProfileStartTTime;
	float DeltaT=100.0f/UsedTime;
	Shift[0]=0;
	WORD CurrentShift=0;
	Print2Console(SYS_PROFILE,"Current FPS is %f",1.0/GetCounter(DC_MAIN_CYCLE));
	Print2Console(SYS_PROFILE,"Current PPS is %f",1.0/GetCounter(DC_PHC));
	Print2Console(SYS_PROFILE,"Strings: %s",GetStringInfo().c_str());
	GString Buf;
	for(DWORD i=0;i<CurrentSet;++i)
	{
		if(Profile[i].Action==PF_ACTION_ENTER)
		{
			Shift[CurrentShift++]=' ';
			Shift[CurrentShift]=0;
			float Time =Profile[i].UsedTime;//(*ProfilerSections)[Profile[i].ID].Ref->UsedTime;
			float STime=Profile[i].StartTime;//(*ProfilerSections)[Profile[i].ID].Ref->StartTime;
			if(CurrentShift==2) TotalTime+=Time*DeltaT;
			if(Time*1000>0.0001f)
			{
				if(Out)
				{
					Buf.Format(" %2.2f > %2.2f%%:%4.4f %s%s%s - %s\n",
				    (STime-ProfileStarTime)*DeltaT,
				    Time*DeltaT,
				    Time*1000,
				    Shift,
				    Shift,
				    Shift,
				    (*ProfilerSections)[Profile[i].ID].Name.c_str());
					*Out+=Buf;

				}
				else
			Print2Console(SYS_PROFILE," %2.2f > %2.2f%%:%4.4f %s%s%s - %s",
				(STime-ProfileStarTime)*DeltaT,
				Time*DeltaT,
				Time*1000,
				Shift,
				Shift,
				Shift,
				(*ProfilerSections)[Profile[i].ID].Name.c_str());
			}
		}
		else
		{
			Shift[--CurrentShift]=0;
		}
	};
	if(Out)
	{
		Buf.Format("Total detected %f\n",TotalTime);;
		*Out+=Buf;
	}
	else
	Print2Console(SYS_PROFILE,"Total detected %f",TotalTime);
	FrameTotalTime=TotalTime;
	*/
};

int DLLTYPE GetFormatedProfileTree(CFormatedProgress *Begin,int NumElems)
{
	if(prof_CashedTree)
	{
		NumElems=MIN(NumElems,PVector.size());
		for(int i=0;i<NumElems;++i,++Begin)
		  *Begin=PVector[i];
		return PVector.size();
	}

	CFormatedProgress *Cur=Begin;
    int NumOut=0;
	float TotalTime=0;
	time_stamp UsedTime =ProfileEndTime-ProfileStartTime;
    if(UsedTime==0) return 0;
    double dUsedTime=1.0/UsedTime;
	double UsetTTime=ProfileEndTTime-ProfileStartTTime;
    if(!UsetTTime)return 0;
	double DeltaT=100.0/UsetTTime;
	WORD CurrentShift=0;
    if(Begin && NumOut<NumElems)
    {
		Cur->Name.Format("%2.6f msec of %3.2f frames",UsetTTime,(float)(1.0/UsetTTime));
		Cur->Offset=0;
		Cur->UsedTime   =UsetTTime;
		Cur->UsedPersent=1;
		Cur++;
	}
	NumOut++;

    if(Begin && NumOut<NumElems)
    {
	    double VTime=VARS::fvars(VARS::fv_FRAMETIME);
		
		Cur->Name.Format("Virtual:%2.6f msec of %3.2f frames,span=%d",(float)VTime,(float)(1.0/VTime),(DWORD)VARS::fvars(VARS::fv_FSPAN_d_20));
		Cur->Offset=0;																
		Cur->UsedTime   =UsetTTime;
		Cur->UsedPersent=1;
		Cur++;
	}
	NumOut++;

    if(Begin && NumOut<NumElems)
    {
        double DFPSn=(1.0/UsetTTime);
        double FPSt=lua::getdword("r_trisdrawn")*DFPSn;
        GString DFPS="tri";
        if(FPSt>10000000000)
        {
          FPSt*=0.000000001;
          DFPS="Gtri";
        }else
        if(FPSt>1000*1000*10)
        {
          FPSt*=0.000001;
          DFPS="Mtri";
        }else
        if(FPSt>1000*10)
        {
          FPSt*=0.001;
          DFPS="Ktri";
        }
            {
             GString outDFPS;
             outDFPS.Format("%2.2f%s",FPSt,(LPCSTR)DFPS);
             lua::setstring("r_strissecond",outDFPS);
            }
        Cur->Name.Format("%7.0d tris for %2.2f%s",lua::getdword("r_trisdrawn"),FPSt,(LPCSTR)DFPS);
		Cur->Offset=0;
		Cur->UsedTime   =UsetTTime;
		Cur->UsedPersent=1;
		Cur++;
	}
	NumOut++;
	for(_ProfilerSections::iterator it=SectionProfilerSections->begin();it!=SectionProfilerSections->end();it++)
      if(it->Ref)
	  {
        const float Per=((CSectionProfile*)it->Ref)->TotalTime*dUsedTime;
        if(Per>0.005f)
        {
		  if(Begin && NumOut<NumElems)
		  {
		    Cur->Name=it->Name;
		    Cur->Offset=CurrentShift;
		 	Cur->UsedTime   =(((CSectionProfile*)it->Ref)->TotalTime*dUsedTime)*UsetTTime;
			Cur->UsedPersent= Per;
			Cur++;
		  }
          NumOut++;
        }  
	  }
	if(Begin && NumOut<NumElems)
				{
					Cur->Name="------";
					Cur->Offset=0;
					Cur->UsedTime   =UsetTTime;
					Cur->UsedPersent=1;
					Cur++;
				}
				NumOut++;
	CurrentShift++;
	for(DWORD i=0;i<CurrentSet;++i)
	{
		if(Profile[i].Action==PF_ACTION_ENTER)
		{
			++CurrentShift;
			const double STime=Profile[i].UsedTime*dUsedTime;
			if(STime>0.005f)
			{
				
				if(Begin && NumOut<NumElems)
				{
					Cur->Name=(*ProfilerSections)[Profile[i].ID].Name;
					Cur->Offset     =CurrentShift;
					Cur->UsedTime   =STime*UsetTTime;
					Cur->UsedPersent=STime;
					Cur++;
				}
				NumOut++;
			}
		}
		else
		{
			--CurrentShift;
		}
	};

     if(Begin && NumOut<NumElems)
				{
					Cur->Name=" --- end ---";
					Cur->Offset     =0;
					Cur->UsedTime   =UsetTTime;
					Cur->UsedPersent=1;
					Cur++;
				}
				NumOut++;
	//FrameTotalTime=TotalTime;
	return NumOut;
}

int DLLTYPE GetStringSectionsProfile(GString &Str)
{
/*   float ftt=Counters::GetTime()-ProfileStarTime;
   if(ftt<0.000001) return 0;
   float onFrameTotalTime=1.0f/ftt;
   Str="";
   GString Buf;
   int i=0;
   for(_ProfilerSections::iterator it=SectionProfilerSections->begin();it!=SectionProfilerSections->end();it++)
      if(it->Ref)
	  {
		  //Str+=it->Name;Str+=":";
		  Buf.Format("%s : %f\n",it->Name.c_str(),((CSectionProfile*)it)->TotalTime*onFrameTotalTime);
		  Str+=Buf+"\n";
	  }
   return i;
   */
	return 0;
}