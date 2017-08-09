// PluginLoader.h: interface for the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLUGIN_LOADER
#define _PLUGIN_LOADER


typedef PBlopBaseObject(*addmodulefunc)(); 

struct PlugInModule
{
    addmodulefunc   AF;
    PBlopBaseObject BF;
    int             PL;
    bool            AS;
    GString         Name;
};

struct DLLTYPE CLoadObjectList
{
    SmartVector<PlugInModule> Modules;
    PBlopBaseObject GetObject       (const int i);
	LPCSTR          GetName         (const int i);
    int             GetObjectStartUp(const int i);
    bool            IsAutoStart     (const int i);
    LPCSTR          GetObjectName   (const int i);
    int             GetNumObjects   ();
    void            AddModule(PBlopBaseObject Base,LPCSTR Name="",int PL=0,bool AS=true);
    void            AddModule(addmodulefunc   Base,LPCSTR Name="",int PL=0,bool AS=true);
    bool            SkipExport();
    static CLoadObjectList *Skip();
};

struct DLLTYPE CLoadObjectAutoDeclarer
{
  CLoadObjectList *DefPlugIn;
  CLoadObjectAutoDeclarer(CLoadObjectList *Def);
  CLoadObjectAutoDeclarer& Add(PBlopBaseObject Base,LPCSTR Name="",int PL=0,bool AS=true);
  CLoadObjectAutoDeclarer& Add(addmodulefunc   Base,LPCSTR Name="",int PL=0,bool AS=true);
  CLoadObjectAutoDeclarer& RelAdd(PBlopBaseObject Base,LPCSTR Name="",int PL=0,bool AS=true);
  CLoadObjectAutoDeclarer& RelAdd(addmodulefunc   Base,LPCSTR Name="",int PL=0,bool AS=true);

};

#define ONCE_PLUGIN                                  \
static bool ConstOnceChecker=false;                  \
if(ConstOnceChecker)return CLoadObjectList::Skip();  \
ConstOnceChecker=true;

#if _DEBUG
#define MAKE_PLUGIN                                  \
 static CLoadObjectList DefPlugIn;                   \
 static CLoadObjectAutoDeclarer DefAuto(&DefPlugIn); \
 static bool ConstRegisterChecker=false;             \
 if(!ConstRegisterChecker) {                         \
  DefAuto.Add
#else
#define MAKE_PLUGIN                                  \
 static CLoadObjectList DefPlugIn;                   \
 static CLoadObjectAutoDeclarer DefAuto(&DefPlugIn); \
 static bool ConstRegisterChecker=false;             \
 if(!ConstRegisterChecker) {                         \
  DefAuto.RelAdd
#endif

#define END_PLUGIN }return DefAuto.DefPlugIn;


typedef CLoadObjectList *PLoadObjectList;

typedef PLoadObjectList(*cobjectloadfunc)(void);

PLoadObjectList DLLTYPE _GetStoredClasses();

#endif 
