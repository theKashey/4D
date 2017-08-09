//---------------------------------------------------------------------------

#ifndef BlopClientH
#define BlopClientH
//---------------------------------------------------------------------------

#include "../System/Window/GenericWindow.h"
#include "../Graphics/GenericGraphic.h"
#include "../Sys/SmartVector.h"
#include "../Phisics/BasePhisic.h"

//typedef SmartVector<BlopBaseObject*> VObjects;
struct CoreVObject
{
   BlopBaseObject* Obj;
   _Profilers::CProfiler     * Profile;
   CoreVObject()
   {
	   Obj=NULL;
	   Profile=NULL;
   }
   ~CoreVObject()
   {
	   if(Obj    ) delete Obj;
	   if(Profile) delete Profile;
   }
   void operator =(BlopBaseObject *O)
   {
	   Obj=O;
	   Profile=new _Profilers::CProfiler(Obj->GetFormatName());
   }
};

#define VObjects SmartVector<CoreVObject>

struct CStartOrder
{
 int              Order;
 bool             AutoStartUp;
 BlopBaseObject** DefOn;
 WORD             DefOnSize;
 CStartOrder()
 {
	 Order      =0;
	 AutoStartUp=0;
	 DefOn      =NULL;
	 DefOnSize  =0;
 }
};

class DLLTYPE CBlopClient: public BaseObject
{
public:
// BlopTransfer    Transfer;
// CConnectKeeper  *ConKeeper;

 CGenericWindow  *Window;
 CGenericGraphic *Graphics;
 Phisic::CBasePhisicSim  *Phisic;

 CInput          *Input;


 VObjects         Objects;
 bool             Termination;
 bool             _TimeSwitch;

 void             AddIn(BlopBaseObject* Object,int StartUpOrder,bool AutoStart,BlopBaseObject** DefOn,LPCSTR ExName=NULL);
 bool             AddInModule(LPCSTR Name);
 void             AddInModulePack(LPCSTR Name);
 void             InitLuaSystem();
public:
 CBlopClient(BlopStartInformation &BSI);
~CBlopClient();


 DWORD Connect(LPCSTR ServerName,WORD ServerPort);

 bool  Start();
 void  ProvidePost();
 bool  TimeTick();
 bool  Init();
 bool  PrepareWorld();

 DWORD OnMessage(const CBlopMessage &Message);

 DWORD TimeSwitch();
};

#endif
