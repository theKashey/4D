#ifndef _PATHSPACE_H_
#define _PATHSPACE_H_

#pragma once

namespace PATH
{
#define  PL_WHOLE   0
#define  PL_MOVENT  1
#define  PL_SECURE  2
#define  PL_MASHINE 3

	bool  UpdateMarkerInfo(const DWORD &Marker,const bVector3f &Pos,bool Add=false);
	DWORD AddMarker (LPCSTR Name,const bVector3f &Pos);
	DWORD GetMarkerByName(LPCSTR Name);
	bool  FindMarker(bVector3f &Dir,const DWORD &Marker); // найти позицию маркера


	//найти путь на заданом уровне, из точки 1 до 2, с узнать новую скорость и ориентацию
	bool TracePath (DWORD PathLevel,const bVector3f &SizeBox,
		            const bVector3f &From ,const bVector3f &To,
	             	const bVector3f &Speed,const bVector3f &Orientation,
					bVector3f &Dirrection,
					DWORD &Flags);
}

#endif