#ifndef _WAYPOINT_H
#define _WAYPOINT_H

bool  GetWaypointPosition(const DWORD &WayPoint,bVector3f &Pos);
DWORD AddWayPoint(LPCSTR Name,const bVector3f &Pos);
DWORD UpdateWayPoint(const DWORD &WayPoint,const bVector3f &Pos);
DWORD FindWayPointByName(LPCSTR Name);

#endif