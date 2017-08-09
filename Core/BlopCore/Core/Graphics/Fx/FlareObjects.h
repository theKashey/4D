#ifndef _H_FLARE_OBJECT_
#define _H_FLARE_OBJECT_

#include "../Cull/CullElement.h"

enum light_flare_type
{
	LFT_NONE,
	LFT_NEON_MESH,
	LFT_TEXTURED_CORONA
};

class CFlareObject
{
	CCullElement  *MyCull;
	DWORD     FrameShowen;
	DWORD     FrameHidden;

	float     Rad;
	float     CurRad;
	float     FallOff;
	DWORD     Material;
	bVector3f Position;
	light_flare_type     flare_type;
	bVector4f Color;
	void Init();
	bool Visibled;
public:
   void SetOccFreq(const int F){MyCull->SetHalfOCCFreq(F);}
   void SetPosition(const float *Pos);
   void SetMaterial(DWORD Mat);
   void SetParentOcc(  CCullElement *Cull);
   void Render();
   CFlareObject();
   CFlareObject(light_flare_type FlareT);
   ~CFlareObject();
   void SetType(light_flare_type FlareT){flare_type=FlareT;}
   void SetColor(const float *C){Color=C;}
   bool CheckCull();
};


/*

void MyMemCpy(void *dest, const void *src, size_t size)
{
  char *dest_cptr = (char*)dest;
  const char *src_cptr = (char*)src;

#pragma ivdep //src и dest не пересекаются.
  for (size_t i = 0; i<size; ++i)
  {
    dest_cptr[i] = src_cptr[i];
  }
}

*/

#endif