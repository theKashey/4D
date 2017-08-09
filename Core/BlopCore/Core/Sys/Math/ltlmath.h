//---------------------------------------------------------------------------

#ifndef LtlMathH
#define LtlMathH
//---------------------------------------------------------------------------

//#include "..\..\..\stdafx.h"
#include <windows.h>
#include <malloc.h>
#include <memory.h>

#ifndef PI
#define PI   (double)3.14159265359
#endif

#define PI2  (double)6.28318530718
#define PI_2 (double)1.570796326795
#define PI_4 (double)0.7853981633975
#define PI2d180 (double)28.64788975653927499926637616517
#define PI2_f_180 (double)0.034906585039888888888888888888889
#define PI_f_180 (double)0.017453292519944444444444444444444

#include <math.h>

#define CheckMask(A,B) ((A & B)==B)

void  Update2pow2(DWORD &A);

float DLLTYPE random();
float DLLTYPE randomM();
int   DLLTYPE roundrandom(int Max);

inline DWORD float2int (float x) 
{
  int n;
  __asm 
  {
    fld     x
    fistp   n
  }
  return (DWORD)n;
}

#endif
