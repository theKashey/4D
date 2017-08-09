// PluginLoader.cpp: implementation of the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////
#include "ProjectSpace.h"
#include "PluginLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CLoadObjectList DefPlugIn;

    PBlopBaseObject CLoadObjectList::GetObject       (const int i)
    {
        if(Modules[i].AF) return Modules[i].AF();
        if(Modules[i].BF) return Modules[i].BF;
        return NULL;
    }
    LPCSTR          CLoadObjectList::GetName         (const int i)
	{
	   return Modules[i].Name;
	}
    int             CLoadObjectList::GetObjectStartUp(const int i)
    {
        return Modules[i].PL;
    }
    bool            CLoadObjectList::IsAutoStart     (const int i)
    {
        return Modules[i].AS;
    }
    LPCSTR          CLoadObjectList::GetObjectName   (const int i)
    {
        return Modules[i].Name;
    }
    int             CLoadObjectList::GetNumObjects   ()
    {
        return Modules.size();
    }

    void            CLoadObjectList::AddModule(PBlopBaseObject Base,LPCSTR Name,int PL,bool AS)
    {
      PlugInModule Mod;
      Mod.AF=NULL;
      Mod.BF=Base;
      Mod.PL=PL;
      Mod.AS=AS;
      Mod.Name=Name;
      Modules.push_back(Mod);
    }
    void            CLoadObjectList::AddModule(addmodulefunc   Base,LPCSTR Name,int PL,bool AS)
     {
      PlugInModule Mod;
      Mod.AF=Base;
      Mod.BF=NULL;
      Mod.PL=PL;
      Mod.AS=AS;
      Mod.Name=Name;
      Modules.push_back(Mod);
    }

    bool           CLoadObjectList::SkipExport()
    {
      return (this==NULL) || (this==Skip());
    }

CLoadObjectList *CLoadObjectList::Skip(){return (CLoadObjectList *)1;}


CLoadObjectAutoDeclarer::CLoadObjectAutoDeclarer(CLoadObjectList *Def)
{
  DefPlugIn=Def;
}

static GString RelPref;

CLoadObjectAutoDeclarer& CLoadObjectAutoDeclarer::Add(PBlopBaseObject Base,LPCSTR Name,int PL,bool AS)
{
    RelPref="[D]";
    DefPlugIn->AddModule(Base,Name+RelPref,PL,AS);
    return *this;
}
CLoadObjectAutoDeclarer& CLoadObjectAutoDeclarer::Add(addmodulefunc   Base,LPCSTR Name,int PL,bool AS)
{
    RelPref="[D]";
    DefPlugIn->AddModule(Base,Name+RelPref,PL,AS);
    return *this;
}



CLoadObjectAutoDeclarer& CLoadObjectAutoDeclarer::RelAdd(PBlopBaseObject Base,LPCSTR Name,int PL,bool AS)
{
    RelPref="[R]";
    DefPlugIn->AddModule(Base,Name+RelPref,PL,AS);
    return *this;
}
CLoadObjectAutoDeclarer& CLoadObjectAutoDeclarer::RelAdd(addmodulefunc   Base,LPCSTR Name,int PL,bool AS)
{
    RelPref="[R]"; 
    DefPlugIn->AddModule(Base,Name+RelPref,PL,AS);
    return *this;
}



PLoadObjectList DLLTYPE _GetStoredClasses()
{
    return &DefPlugIn;
}
