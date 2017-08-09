#include "stdafx.h"
#include "pathspace.h"
#include "../WayPoint.h"


bool PATH::FindMarker(bVector3f &Dir,const DWORD &Marker) // найти позицию маркера
{
  return GetWaypointPosition(Marker,Dir);
}

bool PATH::UpdateMarkerInfo(const DWORD &Marker,const bVector3f &Dir,bool Add)
{
  return UpdateWayPoint(Marker,Dir)>0;
}

DWORD PATH::AddMarker (LPCSTR Name,const bVector3f &Pos)
{
	DWORD mid;
	if(mid=FindWayPointByName(Name))
	  return UpdateMarkerInfo(mid,Pos);
	else
	return AddWayPoint(Name,Pos);
}
DWORD PATH::GetMarkerByName(LPCSTR Name)
{
	return FindWayPointByName(Name);
}

	//найти путь на заданом уровне, из точки 1 до 2, с узнать новую скорость и ориентацию
bool PATH::TracePath (DWORD PathLevel,const bVector3f &SizeBox,
		            const bVector3f &From ,const bVector3f &To,
	             	const bVector3f &Speed,const bVector3f &Orientation,
					bVector3f &Dirrection,
					DWORD &Flags)
{
	Dirrection=To-From;
	Flags     =0;
	return true;
}

