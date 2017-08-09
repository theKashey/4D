//---------------------------------------------------------------------------

#ifndef refID_H
#define refID_H
//---------------------------------------------------------------------------

//classes
#define REF_NULL             0

#define REFC_NULL            0	//null ref
#define REFC_CORE			 1<<28	//blop core system
#define REFC_GAME			 2<<28	//file core system
#define REFC_OBJECT          3<<28
#define REFC_TEMP            4<<28
#define REFC_USER            5<<28


#define REFB_MANAGER         0

//coreBUN
#define REFB_GRAPHIC         1
#define REFB_INTERFACE       2
#define REFB_MODULE          3
#define REFB_LUA             4
#define REFB_NET             5
#define REFB_WORLD           7
#define REFB_CONSOLE         8
#define REFB_OBJECT          9
#define REFB_CORE            10
#define REFB_UTIL            11

//#define REFB_




#endif
