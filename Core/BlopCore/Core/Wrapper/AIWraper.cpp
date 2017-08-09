#include "ProjectSpace.h"
#include "ProjectAI.h"



AI::regEXTFunc  _fRegisterCreatureT=NULL;
AI::regbSTR     _fLoadScriptT      =NULL;
AI::regbSTR     _fRunScriptT       =NULL;
AI::regdSTRL    _fParseScriptT     =NULL;
AI::regSTRV		_fUpdateMarker     =NULL;

void           AI::RegisterCallBack_regExt(regEXTFunc  Func) {_fRegisterCreatureT=Func;}
void           AI::RegisterRunScript      (regbSTR     Func) {_fRunScriptT=Func;}
void           AI::RegisterLoadAIScript   (regbSTR     Func) {_fLoadScriptT=Func;}  
void           AI::RegisterParseScript    (regdSTRL    Func) {_fParseScriptT=Func;}  
void           AI::RegisterUpdateMarker   (regSTRV     Func) {_fUpdateMarker=Func;}  

DWORD          AI::RegisterCreature (CBotExecutor *Exc,LPCSTR Name)
{
    if(_fRegisterCreatureT)return _fRegisterCreatureT(Exc,Name);
    else return 0;
}

bool  DLLTYPE     AI::LoadAIScriptFile(LPCSTR Name) {if(_fLoadScriptT)return _fLoadScriptT(Name);else return false;}
bool  DLLTYPE     AI::RunScript(LPCSTR Name)		{if(_fRunScriptT )return _fRunScriptT(Name);else return false;} 
DWORD DLLTYPE     AI::ParseAIScript(LPCSTR Buf,DWORD Len){if(_fParseScriptT)return _fParseScriptT(Buf,Len);else return false;}
DWORD DLLTYPE     AI::UpdateMarker(LPCSTR Name,const bVector3f &Pos){if(_fUpdateMarker)return _fUpdateMarker(Name,Pos);else return 0;}


AI::regWAYPFunc fRegisterWayPoint=NULL;
void DLLTYPE AI::RegisterCallBack_regWayp(regWAYPFunc Func)
{
    fRegisterWayPoint=Func;
}
DWORD  AI::RegisterWayPoint(DWORD ID,LPCSTR Name,const float* Origin,LPCSTR Target)
{
    if(fRegisterWayPoint)
        return fRegisterWayPoint(ID,Name,Origin,Target);
    else
    return 0;
}