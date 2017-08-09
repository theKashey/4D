#include "ProjectSpace.h"
#include "MemOp.h"

LPVOID PtrMove2D(LPCVOID Pointer,WORD x,WORD y,WORD Width,char bpp)
{
 return (LPVOID )((char*)Pointer+(x+y*Width)*bpp);
}

void _FastMove(LPVOID Dest,LPCVOID Src,DWORD len)
{
 //
 memcpy(Dest,Src,len);
}
