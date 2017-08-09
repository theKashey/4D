#define USE_IJL

#include "ProjectMemory.h"

#pragma once
#ifndef h_u_Texture
#define h_u_Texture


class DLLTYPE u_Texture_Loader
{
public:
    struct GLRGBTRIPLE
    {
      BYTE red;
      BYTE green;
      BYTE blue;
    };

    struct GLRGBQUAD
    {
      BYTE red;
      BYTE green;
      BYTE blue;
      BYTE alpha;
      GLRGBQUAD(){}
      GLRGBQUAD(BYTE r,BYTE g,BYTE b,BYTE a)
	  {
		  red=r;green=g;blue=b;alpha=a;
	  }
    };

    struct TBMPMASK
    {
        DWORD m_redMask, m_greenMask, m_blueMask;
    	RGBQUAD m_colors[255];
    };

#define TEXTYPE_RGB  1
#define TEXTYPE_RGBA 2
#define TEXTYPE_BRG	 3
#define TEXTYPE_BRGA 4

    struct TTexInfo
    {
       int Width,Height,Depth,bpp;
       int ImageSize,Type,tFormat;
       BYTE *pData;
       int IFormat;
	   SlotPoint *Data;
    };


    TTexInfo Info;
	GString  Sourse;

	enum TexModes
	{
		TEX_NORMAL,
		TEX_TOUCH,
		TEX_EXTERNDATA
	};
	TexModes TMode;
	TexModes DataMode;

     u_Texture_Loader(void);
    ~u_Texture_Loader(void);
    void Unload();
	void SetExternData(LPVOID InD);
    bool LoadFromFile(LPCSTR FileName);
	bool TouchFile   (LPCSTR FileName);
	DWORD GetTexLen(LPCSTR FileName);
    void Clear(GLRGBQUAD ClearColor);
    void SetSize(DWORD x,DWORD y,WORD bpp);
    bool IsEmpty(){return Info.pData==NULL;}
    WORD GetBPP(){return Info.bpp;}
	void AssertSize();

	

    void SubTexture(bVector2w To,bVector2w From,bVector2w Size,u_Texture_Loader *Dest);

private:

    #define BM 19778
    #define TGA_RGB 2

    TBMPMASK BMask;
    bool LoadBMP(const char *filename);
    bool LoadJPG(const char *filename);
    bool LoadPNG(const char *filename);
    bool LoadTGA(const char *filename);

    void Load32(DWORD imgFile);
    void Load24(DWORD imgFile);
    void Load16(DWORD imgFile);
    void Load8 (DWORD imgFile);
    void Load4 (DWORD imgFile);

    void ExpandTo32(RGBTRIPLE *image, int pad);
    void SetAlpha(BYTE alpha);

    inline int GetLineWidth(int bpp)
    {return ((Info.Width * Info.bpp + 31) & -32) >> 3; }

    inline int GetRightShiftCount(DWORD dwVal)
    {
      for (int i = 0; i < sizeof(DWORD)*8; ++i)
      {
        if (dwVal & 1)
          return i;
        dwVal = dwVal >> 1;
      }
      return -1;
    }

    inline int GetNumSet(DWORD dwVal)
    {
      int nCount = 0;
      while (dwVal != 0)
      {
        nCount += dwVal & 1;
        dwVal = dwVal >> 1;
      }
      return nCount;
    }

    inline int Scale8(int colorValue, int colorBits)
    {
      switch (colorBits)
      {
        case 1 : return ((colorValue) ? 255 : 0 );
        case 2 : return (colorValue << 6) | (colorValue << 4) | (colorValue << 2) | colorValue;
        case 3 : return (colorValue << 5) | (colorValue << 2) | (colorValue >> 1);
        case 4 : return (colorValue << 4) | colorValue;
        case 5 : return (colorValue << 3) | (colorValue >> 2);
        case 6 : return (colorValue << 2) | (colorValue >> 4);
        case 7 : return (colorValue << 1) | (colorValue >> 6);
        default : return colorValue;
      }
    }
};


#endif
