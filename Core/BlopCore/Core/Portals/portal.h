#pragma once

struct CVisibleFrustum
{
	bVector3f Start;
	bVector3f Dirrection;
	bVector3f Dif;
	bVector3f End;
};

struct CPortal
{
	bool      Enabled;
	DWORD     ROOMID;
	bVector3f Center;
	float     Sphere;
	bVector3f BBOX;
	bVector3f Orientation;
	BYTE      Culling;
	CPortal  *PortalTo;
	bool  TransformLook(CVisibleFrustum &LookAt);
};

#define do_TYPE_MAP     0
#define do_TYPE_OCT     1
#define do_TYPE_BSP     2
#define do_TYPE_HASH    3

#define do_LINK_SINGLE  0 << 2
#define do_LINK_OBJECT  1 << 2
#define do_LINK_STATIC	1 << 3
#define do_LINK_PORTAL	1 << 4
#define do_LINK_ALL     0x1C

struct CPortalRoom
{
	bool      Enabled;
	DWORD     RoomID;
	DWORD     SimulatorID;
	DWORD     NumPortals;
	DWORD     NumObjects;
	DWORD     NumStatic;
	SmartVector<CPortal> Portals;
    
	BYTE doTYPE;DWORD     DIMO[3];LPVOID DIVO;
	BYTE dsTYPE;DWORD     DIMS[3];LPVOID DIVS;
	BYTE dpTYPE;DWORD     DIMP[3];LPVOID DIVP;

	DWORD GetVisiblePortals(const CVisibleFrustum &LookAt,DWORD *Data,DWORD Len);
};
