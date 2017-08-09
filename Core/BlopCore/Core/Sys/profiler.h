#pragma once


namespace _Profilers
{

typedef __int64 time_stamp;

class DLLTYPE CProfiler
{						   
	DWORD ID;
	DWORD MyID;
public:
	time_stamp StartTime;
	//time_stamp EndTime;

	//time_stamp UsedTime;
	CProfiler(LPCSTR Name);
	~CProfiler(void);
	void Enter();
	void Leave();
};

class DLLTYPE CSectionProfile
{
    DWORD ID;
    DWORD MyID;
    int   Locked;
public:
    time_stamp StartTime;
    time_stamp TotalTime;
    bool  UsedNow;
    CSectionProfile(LPCSTR Name);
    ~CSectionProfile();
	void Enter();
	void Leave();
    void Reset();
};

class CProfilerLocker
{
	CProfiler       *MyProfiler;
    CSectionProfile *MySectionProfiler;
public:
	CProfilerLocker(CProfiler *Pro)
	{
	   MyProfiler       =Pro;
       MySectionProfiler=NULL;
	   MyProfiler->Enter();
	}
	CProfilerLocker(CSectionProfile *Pro)
	{
	   MyProfiler       =NULL;
	   MySectionProfiler=Pro;
	   MySectionProfiler->Enter();
	}
	~CProfilerLocker()
	{
        if(MyProfiler)    	 MyProfiler->Leave();
		else
        if(MySectionProfiler)MySectionProfiler->Leave();
	}
};


struct ProfilerSection
{
	DWORD ID;
	GString Name;
	CProfiler *Ref;
};


struct ProfilerProgress
{
	DWORD ID;
	DWORD Action;
	time_stamp UsedTime;
	//time_stamp StartTime;
};


struct CFormatedProgress
{
    GString Name;
    BYTE  Offset;
    float UsedTime;
    float UsedPersent;
};
};

#define Profile(a) _Profilers::CProfilerLocker PLOCK(&a);
#define MakeProfile(a,b) _Profilers::CProfiler a(b);
#define MakeSectionProfile(a,b) _Profilers::CSectionProfile a(b);
#define MakeFunctionProfile _Profilers::CSectionProfile funcPro(__FUNCTION__);Profile(funcPro);

void DLLTYPE StartProfile();
void DLLTYPE EndProfile();
void DLLTYPE PrintProfiler(GString *outs=NULL);

int DLLTYPE GetSectionProfilers   (_Profilers::ProfilerSection* Begin);
int DLLTYPE GetAllProfilers       (_Profilers::ProfilerSection* Begin);
int DLLTYPE GetProfileTree        (_Profilers::ProfilerProgress *Begin);
int DLLTYPE GetFormatedProfileTree(_Profilers::CFormatedProgress *Begin,int NumElems);

int DLLTYPE GetStringSectionsProfile(GString &Str);
