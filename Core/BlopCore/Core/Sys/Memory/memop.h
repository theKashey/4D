//---------------------------------------------------------------------------

#ifndef MemOpH
#define MemOpH
//---------------------------------------------------------------------------

LPVOID DLLTYPE  PtrMove2D(LPCVOID Pointer,WORD x,WORD y,WORD Width,char bpp);
void   DLLTYPE _FastMove(LPVOID Dest,LPCVOID Src,DWORD len);




#endif
