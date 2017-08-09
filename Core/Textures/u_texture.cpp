#include "StdAfx.h"
#include "ProjectMemory.h"
#include "Core\Graphics\Textures\u_Texture.h"
//#include "u_texture.h"

#ifdef USE_IJL
#include "Texture/IJL.h"
#endif

#ifdef USE_PNG
#include "Texture/png/png.h"
#endif


#define SetPtr {if(Info.Data==NULL)Info.pData=NULL;else Info.pData=(LPBYTE)LPVOID(*Info.Data);}

bool pngSupported;
bool jpgLoaded;
bool jpgSupported;
int  jpgQuality;

//DWORD SYS_TEXTURES=0;

PFNIJLINIT ijlInit;
PFNIJLFREE ijlFree;
PFNIJLREAD ijlRead;
PFNIJLWRITE ijlWrite;
HINSTANCE jpgLib;

u_Texture_Loader::u_Texture_Loader(void)
{
   ZeroMemory(&Info,sizeof(Info));
   TMode=TEX_NORMAL;
}

u_Texture_Loader::~u_Texture_Loader(void)
{
   Unload();
   SetSize(0,0,0);
}

void u_Texture_Loader::Unload()
{
// _!!_
}

void u_Texture_Loader::Clear(GLRGBQUAD ClearColor)
{
    SetPtr;
	int i;
	for(i=0;i<Info.ImageSize;++i)
		Info.pData[i]=ClearColor.red;// _!_
}

void u_Texture_Loader::AssertSize()
{
	SetSize(Info.Width,Info.Height,Info.bpp);
}

void u_Texture_Loader::SetExternData(LPVOID InD)
{
	if(DataMode!=TEX_EXTERNDATA && Info.Data)delete Info.Data;
	Info.pData=(BYTE*)InD;
	if(InD)DataMode=TEX_EXTERNDATA;
	else   DataMode=TEX_NORMAL;
}

void u_Texture_Loader::SetSize(DWORD x,DWORD y,WORD bpp)
{
 if(DataMode==TEX_EXTERNDATA && Info.pData){ return;}
 if(Info.Data) delete Info.Data;
 Info.Data=NULL;
 SetPtr;
 if(x*y*bpp==0)return;
 
 Info.Data = GetGlobalDataPool()->Alloc((WORD)x*y*bpp/8);
 if(!Info.Data)
 {
  //raise
  Error2Console(SYS_TEXTURES,"u_Texture_Loader::SetSize - unable to allocate data");
  return;
 }
 SetPtr;
}

void u_Texture_Loader::SubTexture(bVector2w To,bVector2w From,bVector2w Size,u_Texture_Loader *Dest)
{
 WORD sx=To[0];
 WORD dx=From[0];
 WORD lenx=Size[0]; if(lenx+To[0]>Info.Width ) lenx=Info.Width -To[0];
 WORD leny=Size[1]; if(leny+To[1]>Info.Height) leny=Info.Height-To[1];
 if(lenx+From[0]>Dest->Info.Width )lenx=Dest->Info.Width -From[0];
 if(leny+From[1]>Dest->Info.Height)leny=Dest->Info.Height-From[1];

 WORD sy,dy;
 for(sy=To[1],dy=From[1];sy<leny;++sy,++dy)
  _FastMove(PtrMove2D(      Info.pData,sx,sy,      Info.Width,      Info.bpp),
            PtrMove2D(Dest->Info.pData,dx,dy,Dest->Info.Width,Dest->Info.bpp),
            lenx);
// for(WORD x=Position.x;x<e.x;x++)
}

bool u_Texture_Loader::LoadFromFile(LPCSTR FileName)
{
  Sourse=FileName;
  char *ext;
  //ClearMem();
  ext = Sourse.PointAt(Sourse.GetLength()-3);

  if (!stricmp(ext, "bmp"))
    return LoadBMP(FileName);

  if (!stricmp(ext, "jpg"))
    return LoadJPG(FileName);

  if (!stricmp(ext, "png"))
    return LoadPNG(FileName);

  if (!stricmp(ext, "tga"))
    return LoadTGA(FileName);
  
  return false;
}

bool u_Texture_Loader::TouchFile(LPCSTR FileName)
{
  TMode=TEX_TOUCH;
  bool ret=LoadFromFile(FileName);
  TMode=TEX_NORMAL;
  return ret;
}

bool u_Texture_Loader::LoadBMP(const char *filename)
{
  BITMAPFILEHEADER fHeader;
  BITMAPINFOHEADER iHeader;
  DWORD bmpFile;
  int palSize;
  int x=sizeof(BITMAPINFOHEADER);
  if(!x) return false;

  if (!(bmpFile = bOpenFile(filename, "rb",TMode==TEX_NORMAL)))
  {
	Error2Console(SYS_TEXTURES,"%s not found",filename);
    //DoError("%f not found",filename);
    return false;
  }
                    // sizeof(BITMAPFILEHEADER)
  bReadFile(&fHeader, 1,14 , bmpFile);

  if (fHeader.bfType != BM)
  {
    Error2Console(SYS_TEXTURES,"%s is not BMP",filename);
    bCloseFile(bmpFile);
    return false;
  }

  bReadFile(&iHeader, 1, sizeof(BITMAPINFOHEADER), bmpFile);

  if (iHeader.biSize == sizeof(BITMAPCOREHEADER))
  {
	Error2Console(SYS_TEXTURES,"%s has mismatch header",filename);
    bCloseFile(bmpFile);
    return false;
  }

  Info.Width  = iHeader.biWidth;
  Info.Height = iHeader.biHeight;
  Info.bpp    = iHeader.biBitCount;
  Info.ImageSize = Info.Width*Info.Height*(Info.bpp/8);
  if(TMode==TEX_TOUCH)
  {
	  bCloseFile(bmpFile);
	  return true;
  }

  if (iHeader.biCompression != BI_RGB)
  {
    if (iHeader.biCompression == BI_BITFIELDS)
    {
      if (Info.bpp == 16)
      {
        bReadFile(&BMask.m_redMask  , 1, sizeof(DWORD), bmpFile);
        bReadFile(&BMask.m_greenMask, 1, sizeof(DWORD), bmpFile);
        bReadFile(&BMask.m_blueMask , 1, sizeof(DWORD), bmpFile);
      }
    }
    else
    {
	  Error2Console(SYS_TEXTURES,"%s has unsupported format",filename);
      bCloseFile(bmpFile);
      return false;
    }
  }

  if (Info.bpp < 16)
  {
    palSize = iHeader.biClrUsed*4;

    bReadFile(&BMask.m_colors, 1, palSize, bmpFile);
  }

  bSeekFile(bmpFile, fHeader.bfOffBits, SEEK_SET);

  Info.ImageSize = iHeader.biSizeImage;

  if (Info.ImageSize == 0)
    Info.ImageSize = Info.Height * GetLineWidth(Info.bpp);
  //Info.Data=
  //AssertSize();
  switch (Info.bpp)
  {
    case 32 :
      Load32(bmpFile);
      break;
    case 24 :
      Load24(bmpFile);
      break;
    case 16 :
      Load16(bmpFile);
      break;
    case 8:
      Load8(bmpFile);
    case 4:
      Load4(bmpFile);
    default :
		Error2Console(SYS_TEXTURES,"%s -has unsupported -%d- bpp",filename,Info.bpp);
      break;

  }

  bCloseFile(bmpFile);

  if (Info.pData)
    return true;
  else
  {
	Error2Console(SYS_TEXTURES,"%s -no data was loaded",filename);
    return false;
  }
}

bool u_Texture_Loader::LoadJPG(const char *filename)
{
  RGBTRIPLE *image;
  int jErr;
  int pad;
  JPEG_CORE_PROPERTIES jpgProps;

  if (!jpgSupported)
    return false;

  jErr = ijlInit(&jpgProps);
  if (jErr != IJL_OK)
    return false;

  jpgProps.JPGFile = filename;

  jErr = ijlRead(&jpgProps, IJL_JFILE_READPARAMS);
  if (jErr != IJL_OK)
  {
    ijlFree(&jpgProps);
    return false;
  }

  Info.Width = jpgProps.JPGWidth;
  Info.Height = jpgProps.JPGHeight;
  Info.bpp = 24;

  pad = GetLineWidth(24) - Info.Width*3;

  //image = (RGBTRIPLE*) new unsigned char[GetLineWidth(24)*Info.Height];
  SetSize(Info.Width,Info.Height,24);
  if(!Info.pData)
	  return false;
  image=(RGBTRIPLE*)Info.pData;
  jpgProps.DIBWidth = Info.Width;
  jpgProps.DIBHeight = -Info.Height;
  jpgProps.DIBChannels = 3;
  jpgProps.DIBColor = IJL_RGB;
  jpgProps.DIBPadBytes = pad;
  jpgProps.DIBBytes = (unsigned char *)image;

  switch (jpgProps.JPGChannels) 
  {
    case 1: 
      jpgProps.JPGColor = IJL_G;
      break;
    case 3:
      jpgProps.JPGColor = IJL_YCBCR;
      break;
    default:
      jpgProps.DIBColor = IJL_COLOR(IJL_OTHER);
      jpgProps.JPGColor = IJL_COLOR(IJL_OTHER);
      break;
  }

  jErr = ijlRead(&jpgProps, IJL_JFILE_READWHOLEIMAGE);
  if (jErr != IJL_OK) 
  {
    SetSize(0,0,0);
    return false;
  }

  //ExpandTo32(image, pad);
  Info.bpp = 24;

//  delete [] image;

  ijlFree(&jpgProps);

  Info.IFormat = TEXTYPE_RGB;

  return true;
}

bool u_Texture_Loader::LoadPNG(const char *filename)
{
#ifdef USE_PNG
  png_structp png = 0;
  png_infop pngInfo = 0;
  DWORDpngFile;
  int rowBytes;
  int bpp;
  int colorType;
  png_byte **rowPtrs;
  png_byte sig[8];

  RGBTRIPLE *image;
  int i;
  double gamma;

  if (!filename) return false;

  if (!(pngFile = bOpenFile(filename, "rb")))
    return false;

  bReadFile(sig, 1, 8, pngFile);
  if (!png_check_sig(sig, 8))
  {
    bCloseFile(pngFile);
    return false;
  }

  png = png_create_read_struct("1.0.10", 0, 0, 0);
  if (!png)
  {
    bCloseFile(pngFile);
    return false;
  }

  pngInfo = png_create_info_struct(png);
  if (!pngInfo)
  {
    png_destroy_read_struct(&png, 0, 0);
    bCloseFile(pngFile);
    return false;
  }

  png_init_io(png, pngFile);
  png_set_sig_bytes(png, 8);

  png_read_info(png, pngInfo);
  unsigned long w, h;
  png_get_IHDR(png, pngInfo, &w, &h, &Info.bpp, &colorType, NULL, NULL, NULL);

  Info.Width = w;
  Info.Height = h;
  Info.ImageSize = Info.Width*Info.Height*(Info.bpp/8);
  if(TMode==TEX_TOUCH)
  {
	  png_destroy_read_struct(&png, &pngInfo, 0);
	  bCloseFile(pngFile);
	  return true;
  }

  if (Info.bpp == 16)
    png_set_swap(png);

  if (colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(png);

  if (Info.bpp < 8)
    png_set_packing(png);

  if (png_get_valid(png, pngInfo, PNG_INFO_tRNS) == PNG_INFO_tRNS)
    png_set_expand(png);

  if ((colorType == PNG_COLOR_TYPE_GRAY) || (colorType == PNG_COLOR_TYPE_GRAY_ALPHA)) 
    png_set_gray_to_rgb(png);

  if ((png_get_gAMA(png, pngInfo, &gamma) == 1))
    png_set_gamma(png, 2.2, gamma);
  else
    png_set_gamma(png, 2.2, 0.45455);

  png_read_update_info(png, pngInfo);

  rowBytes = png_get_rowbytes(png, pngInfo);
  bpp = rowBytes / Info.Width;

  switch (bpp)
  {
    case 4:
	  SetSize(rowBytes,Info.Height,8);
      //Info.pData = (GLRGBQUAD*) new unsigned char[rowBytes*Info.Height];
      if (!Info.pData)
      {
        bCloseFile(pngFile);
        png_destroy_read_struct(&png, &pngInfo, 0);
        return false;
      }

      rowPtrs = new png_byte * [Info.Height];
      if (!rowPtrs)
      {
        bCloseFile(pngFile);
        //delete [] Info.pData;
		SetSize(0,0,0);
        png_destroy_read_struct(&png, &pngInfo, 0);
        return false;
      }

    	for (i = 0; i < Info.Height; ++i)
		    rowPtrs[i] = (unsigned char *)Info.pData + i * rowBytes;

      png_read_image(png, rowPtrs);

      delete [] rowPtrs;
      rowPtrs = 0;

      Info.bpp = 32;
      break;
    case 3:
      image = (RGBTRIPLE*) new unsigned char[rowBytes*Info.Height];
      if (!image)
      {
        bCloseFile(pngFile);
        png_destroy_read_struct(&png, &pngInfo, 0);
        return false;
      }

      rowPtrs = new png_byte * [Info.Height];
      if (!rowPtrs)
      {
        bCloseFile(pngFile);
        png_destroy_read_struct(&png, &pngInfo, 0);
        delete [] image;
        return false;
      }

    	for (i = 0; i < Info.Height; ++i)
		    rowPtrs[i] = (unsigned char *)image + i * rowBytes;

      png_read_image(png, rowPtrs);

      delete [] rowPtrs;
      rowPtrs = 0;

      Info.bpp = 32;

      //Info.pData = new GLRGBQUAD[Info.Width*Info.Height];
	  SetSize(Info.Width,Info.Height,32);
      if (!Info.pData)
      {
        bCloseFile(pngFile);
        png_destroy_read_struct(&png, &pngInfo, 0);
        delete [] image;
        return false;
      }

      ExpandTo32(image, 0);
      delete [] image;
      SetAlpha(255);

      break;
  }
  
  Info.IFormat = TEXTYPE_RGB;
  
  png_read_end(png, pngInfo);
  png_destroy_read_struct(&png, &pngInfo, 0);
  
  bCloseFile(pngFile);

  return true;
#endif  
  return false;
}

#pragma pack (push, 1)
struct TGAHEADER
{
    BYTE id_length;
    BYTE tfColorMapType;
    BYTE tfImageType;
    WORD tfcolormap_index, 
		tfcolormap_length;
	BYTE tfcolormap_size;
    WORD tfOrigX;
    WORD tfOrigY;
    WORD tfWidth;
    WORD tfHeight;
    BYTE tfBpp;
    BYTE tfImageDes;
};
#pragma pack (pop, 1)

bool u_Texture_Loader::LoadTGA(const char *filename)
{
  DWORD tgaFile;
  TGAHEADER tHeader;
  int bytesRead;
//  RGBTRIPLE *image;

  if (!(tgaFile = bOpenFile(filename, "rb",TMode==TEX_NORMAL)))
    return false;

  bReadFile(&tHeader, 1, sizeof(tHeader), tgaFile);

  if (tHeader.tfImageType != TGA_RGB)
  {
    bCloseFile(tgaFile);
    return false;
  }

  if (tHeader.tfColorMapType != 0)
  {
    bCloseFile(tgaFile);
    return false;
  }

  Info.Width  = tHeader.tfWidth;//[0]  + tHeader.tfWidth [1] * 256;
  Info.Height = tHeader.tfHeight;//[0] + tHeader.tfHeight[1] * 256;
  Info.bpp = tHeader.tfBpp;
  Info.ImageSize = Info.Width*Info.Height*(Info.bpp/8);

   if(TMode==TEX_TOUCH)
  {
	  bCloseFile(tgaFile);
	  return true;
  }

  if ((Info.bpp != 24) && (Info.bpp != 32))
  {
    bCloseFile(tgaFile);
    return false;
  }

  switch (Info.bpp)
  {
    case 32:
      SetSize(Info.Width,Info.Height,32);
      if (!Info.pData)
        return false;

	  bytesRead = (int)bReadFile(Info.pData, 1, Info.ImageSize, tgaFile);
      if (bytesRead != Info.ImageSize)
      {
        bCloseFile(tgaFile);
        SetSize(0,0,0);
        return false;
      }
	   Info.IFormat = TEXTYPE_BRGA;
      break;
    case 24:

	  SetSize(Info.Width,Info.Height,32);      
      if (!Info.pData)
      {
        return false;
      }

      bytesRead = (int)bReadFile(Info.pData, 1, Info.ImageSize, tgaFile);
      if (bytesRead != Info.ImageSize)
      {
        bCloseFile(tgaFile);
		SetSize(0,0,0);
        return false;
      }
      Info.IFormat = TEXTYPE_BRG;
//      ExpandTo32(image, 0);
//      delete [] image;
      break;
    default :
      bCloseFile(tgaFile);
      return false;
  }
  bCloseFile(tgaFile);

  return true;
}

void u_Texture_Loader::Load32(DWORD imgFile)
{
  int bytesRead;

  SetSize(Info.Width,Info.Height,32);

  if (!Info.pData)
    return;

  bytesRead = (int)bReadFile(Info.pData, 1, Info.ImageSize, imgFile);
  if (bytesRead != Info.ImageSize)
  {
    bCloseFile(imgFile);
    Unload();
    return;
  }

  Info.IFormat = TEXTYPE_BRGA;

  SetAlpha(255);
}

void u_Texture_Loader::Load24(DWORD imgFile)
{
  int bytesRead;
  int pad;

  pad = GetLineWidth(24) - (Info.Width*3);

  //bgrBits = (RGBTRIPLE*) new unsigned char[Info.ImageSize];
  SetSize(Info.Width,Info.Height,32);
  if (!Info.pData)
  {
    Error2Console(SYS_TEXTURES,"Unable to alloc data of %d-%d",Info.Width,Info.Height);
    return;
  }

  bytesRead = (int)bReadFile(Info.pData, 1, Info.ImageSize, imgFile);
  if (bytesRead != Info.ImageSize)
  {
    bCloseFile(imgFile);
	Error2Console(SYS_TEXTURES,"readed mismach size %d!=%d",bytesRead,Info.ImageSize);
    return;
  }
 
  //ExpandTo32(bgrBits, pad);

 // delete [] bgrBits;
  Info.IFormat = TEXTYPE_BRG;
  Info.bpp = 24;
}

void u_Texture_Loader::Load16(DWORD imgFile)
{
  WORD *bgrBits;
  int bytesRead;
  int pad;
  
  int redBits, greenBits, blueBits;
  int redShr, greenShr;

  int x, y;

  WORD *source16;
  GLRGBQUAD *dest;

  bgrBits = (WORD*) new unsigned char[Info.ImageSize];

  if (!bgrBits)
      return;

  bytesRead = (int)bReadFile(bgrBits, 1, Info.ImageSize, imgFile);
  if (bytesRead != Info.ImageSize)
  {
    bCloseFile(imgFile);
    delete [] bgrBits;
    return;
  }

  SetSize(Info.Width,Info.Height,16);
  if (!Info.pData)
    return;

  pad = GetLineWidth(16) - (Info.Width*2);
  
  redShr = GetRightShiftCount(BMask.m_redMask);
  greenShr = GetRightShiftCount(BMask.m_greenMask);

  redBits = GetNumSet(BMask.m_redMask);
  greenBits = GetNumSet(BMask.m_greenMask);
  blueBits = GetNumSet(BMask.m_blueMask);

  source16 = bgrBits;
  dest     = (GLRGBQUAD*)Info.pData;

  for (x = 0; x < Info.Height; ++x)
  {
    for (y = 0; y < Info.Width; ++y)
    {
      dest->red = Scale8(*source16 & (int)BMask.m_blueMask, blueBits);
      dest->green = Scale8((*source16 & (int)BMask.m_greenMask) >> greenShr, greenBits);
      dest->blue = Scale8((*source16 & (int)BMask.m_redMask) >> redShr, redBits);
      dest->alpha = 255;
      ++source16;
      ++dest;
    }
    source16 = (WORD*)((BYTE*)source16 + pad);
  }

  delete [] bgrBits;
  Info.IFormat = TEXTYPE_BRG;
  Info.bpp = 32;
}

void u_Texture_Loader::Load8(DWORD imgFile)
{
  BYTE *bgrBits;
  int bytesRead;
  BYTE *source8;
  GLRGBQUAD *dest;
  
  int x, y;
  int pad;

  bgrBits = new BYTE[Info.ImageSize];

  if (!bgrBits) 
    return;

  bytesRead = (int)bReadFile(bgrBits, 1, Info.ImageSize, imgFile);
  if (bytesRead != Info.ImageSize)
  {
    bCloseFile(imgFile);
    delete [] bgrBits;
    return;
  }

  SetSize(Info.Width,Info.Height,32);
  if (!Info.pData)
    return;

  pad = GetLineWidth(8) - Info.Width;

  source8 = bgrBits;
  dest    = (GLRGBQUAD*)Info.pData;

  for (x = 0; x < Info.Height; ++x)
  {
    for (y = 0; y < Info.Width; ++y)
    {
      dest->red = BMask.m_colors[*source8].rgbBlue;
      dest->green = BMask.m_colors[*source8].rgbGreen;
      dest->blue = BMask.m_colors[*source8].rgbRed;
      dest->alpha = 255;
      ++dest;
      ++source8;
    }
    source8 += pad;
  }

  delete [] bgrBits;
  Info.IFormat = TEXTYPE_BRG;
  Info.bpp = 32;
}

void u_Texture_Loader::Load4(DWORD imgFile)
{
  BYTE *bgrBits;
  int bytesRead;
  BYTE *source4;
  GLRGBQUAD *dest;
  int palEntry;
  
  int x, y;
  int pad;

  bgrBits = new BYTE[Info.ImageSize];

  if (!bgrBits)
    return;

  bytesRead = (int)bReadFile(bgrBits, 1, Info.ImageSize, imgFile);
  if (bytesRead != Info.ImageSize)
  {
    bCloseFile(imgFile);
    delete [] bgrBits;
    return;
  }

  SetSize(Info.Width,Info.Height,32);
  if (!Info.pData)
    return;

  pad = GetLineWidth(4) - (Info.Width >> 1);

  source4 = bgrBits;
  dest    = (GLRGBQUAD*)Info.pData;

  for (x = 0; x < Info.Height; ++x)
  {
    for (y = 0; y < Info.Width; ++y)
    {
      if (y % 2)
        palEntry = (int)(*source4 & 0xF);
      else
        palEntry = (int)((*source4 >> 4) & 0xF);

      dest->red = BMask.m_colors[palEntry].rgbBlue;
      dest->green = BMask.m_colors[palEntry].rgbGreen;
      dest->blue = BMask.m_colors[palEntry].rgbRed;
      dest->alpha = 255;
      ++dest;

      if (y % 2)
        ++source4;
    }
    source4 += pad;
  }

  delete [] bgrBits;
  Info.IFormat = TEXTYPE_BRG;
  Info.bpp = 32;
}


void u_Texture_Loader::ExpandTo32(RGBTRIPLE *image, int pad)
{
  int x, y;
  RGBTRIPLE *pixel24;
  GLRGBQUAD *pixel32;

  pixel24 = image;
  pixel32 = (GLRGBQUAD*)Info.pData;

  for (x = 0; x < Info.Height; ++x)
  {
    for (y = 0; y < Info.Width; ++y)
    {
      pixel32->red = pixel24->rgbtBlue;
      pixel32->green = pixel24->rgbtGreen;
      pixel32->blue = pixel24->rgbtRed;
      pixel32->alpha = 255;

      ++pixel24;
      ++pixel32;
    }
    pixel24 = (RGBTRIPLE*)((BYTE*)pixel24 + pad);
  }
}

void u_Texture_Loader::SetAlpha(BYTE alpha)
{
  int i;
  GLRGBQUAD *pixel;

  pixel = (GLRGBQUAD*)Info.pData;
  for (i = 0; i < (Info.Width*Info.Height); i++)
  {
    pixel->alpha = alpha;
    pixel++;
  }
}


bool InitJPG()
{
  if(!jpgLoaded)
  {
   jpgLib = LoadLibrary("ijl15.dll");
   if (!jpgLib)
     return false;
 
   ijlInit = (PFNIJLINIT) GetProcAddress(jpgLib, "ijlInit");
   ijlFree = (PFNIJLFREE) GetProcAddress(jpgLib, "ijlFree");
   ijlRead = (PFNIJLREAD) GetProcAddress(jpgLib, "ijlRead");
   ijlWrite = (PFNIJLWRITE) GetProcAddress(jpgLib, "ijlWrite");
 
   if (ijlInit &&
       ijlFree &&
       ijlRead &&
       ijlWrite)
   {
     jpgLoaded=true;
     return true;
   }
  else
     return false;
  } else
  return false;
}
