
#include "ProjectSpace.h"
#include "LtlMath.h"
#include "math.h"


#define LM_TP_P2X_SIZE 32

  typedef struct __LMTables
  {
    struct
    {
      DWORD *Pow2x;   // pow of power of 2
    }POW;  // POW table
    struct
    {
    }Trigonomenry;    // trigonometry
  }_LMTables;            // tables

struct CLittleMath
{
//tables
  static _LMTables Tables;

  void InitPow2()
  {
   Tables.POW.Pow2x=new DWORD[LM_TP_P2X_SIZE];
   DWORD z=1;
   for(int x=0;x<LM_TP_P2X_SIZE;++x)
   {
    Tables.POW.Pow2x[x]=z;
    z*=2;
   }
  }

  void InitPow()
  {
    InitPow2();
  }

  void InitTrigonometry()
  {
  }

  void InitTables()
  {
   InitPow();
   InitTrigonometry();
  }

  CLittleMath()
  {
   InitTables();
  }
}LittleMath;


_LMTables CLittleMath::Tables;

int round(float a)
{
 return (int)ceilf(a);
}

void Update2pow2(DWORD &A)
{
 for(int x=0;x<LM_TP_P2X_SIZE;++x)
  if(A<=CLittleMath::Tables.POW.Pow2x[x])
  {
   A=CLittleMath::Tables.POW.Pow2x[x];
   return;
  }
}

float DLLTYPE random(){ return rand()/(float)RAND_MAX;}
float DLLTYPE randomM(){ return 2*(random()-0.5);}

int   DLLTYPE roundrandom(int Max)
{
	return Max*rand()/((float)RAND_MAX);
}

