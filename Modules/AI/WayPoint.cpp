// WayPoint.cpp: implementation of the CWayPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WayPoint.h"

typedef SmartVector<bVector3f> WayPointList;
typedef CBSPDividorLookUpArray<char,int> _WAYBSP;


WayPointList WayPoints;
_WAYBSP      WayBSP;


bool  GetWaypointPosition(const DWORD &WayPoint,bVector3f &Pos)
{
	if(!WayPoint) return false;
	Pos=WayPoints[WayPoint-1];
	return true;
}
DWORD AddWayPoint(LPCSTR Name,const bVector3f &Pos)
{
	WayPoints.push_back(Pos);
	WayBSP.PushAs(WayPoints.size(),Name,Istrlen(Name));
	return WayPoints.size();
}
DWORD UpdateWayPoint(const DWORD &WayPoint,const bVector3f &Pos)
{
	if(!WayPoint) return false;
	WayPoints[WayPoint-1]=Pos;
	return WayPoint;
}
DWORD FindWayPointByName(LPCSTR Name)
{
	return WayBSP.GetIDFor(Name,Istrlen(Name));
}