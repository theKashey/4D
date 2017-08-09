
#include "stdafx.h"
#include "ProjectMemory.h"
#include "Core\Graphics\Textures\Textures.h"
#include "Core\Graphics\Textures\in_Textures.h"
#include "ProjectGraphic.h"
#include "coregraphics.h"


using namespace Textures;

DWORD Mode2Type(CDWORD Mode,CDWORD DefMode)
{
	if(Mode==0) return DefMode;
	switch(Mode)
	{
	 case TMU_TEXTURE1D    : return GL_TEXTURE_1D      ; 
	 case TMU_TEXTURE2D	   : return GL_TEXTURE_2D      ; 
	 case TMU_TEXTURE3D	   : return GL_TEXTURE_3D      ; 
	 case TEX_CUBEMAP1     : return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	 case TEX_CUBEMAP2     : return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	 case TEX_CUBEMAP3     : return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	 case TEX_CUBEMAP4     : return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	 case TEX_CUBEMAP5     : return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	 case TEX_CUBEMAP6     : return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	 case TMU_TEXTURECUBE  : return GL_TEXTURE_CUBE_MAP; 
	 case TMU_TEXTUREDEPTH : return GL_TEXTURE_2D      ;
	 default : return DefMode; 
	}
}

inline float _MAPENV(int ENV)
{
 switch(ENV)
 {
  case TEX_ENV_MODULATE: return GL_MODULATE;
 }
 return GL_MODULATE;
}

inline DWORD _MAPWRAP(int WRAP)
{
 switch(WRAP)
 {
  case TEX_WRAP_NONE        : ASSERT(FALSE);return 0;
  case TEX_WRAP_REPEAT      :  return GL_REPEAT;
  case TEX_WRAP_CLAMP       :  return GL_CLAMP;
  case TEX_WRAP_CLAMP2EDGE  :  return GL_CLAMP_TO_EDGE; 
  case TEX_WRAP_CLAMP2BORDER:  return GL_CLAMP_TO_BORDER;
  case TEX_WRAP_MIRROR      :  return GL_MIRRORED_REPEAT;
  case TEX_WRAP_MIRRORONCE  :  return GL_MIRROR_CLAMP_ATI;
 }
 return GL_REPEAT;
}



int TEXTURE2D_MAX_SIZE=0;
bool TEXTURE2D_MAX_SIZE_READED=false;

CGLTextureContainer::CGLTextureContainer()
{
  TextureID  =0;
  TextureType=GL_TEXTURE_2D;
  Width=Height=Depth=0;
}

void CGLTextureContainer::SetTextureType(CDWORD Type)
{
	switch(Type)
	{
	 case TMU_TEXTURE1D    : TextureType=GL_TEXTURE_1D      ;break; 
	 case TMU_TEXTURE2D	   : TextureType=GL_TEXTURE_2D      ;break; 
	 case TMU_TEXTURE3D	   : TextureType=GL_TEXTURE_3D      ;break; 
	 case TEX_CUBEMAP1:
	 case TEX_CUBEMAP2:
	 case TEX_CUBEMAP3:
	 case TEX_CUBEMAP4:
	 case TEX_CUBEMAP5:
	 case TEX_CUBEMAP6:
	 case TMU_TEXTURECUBE  : TextureType=GL_TEXTURE_CUBE_MAP;break; 
	 case TMU_TEXTUREDEPTH : TextureType=GL_TEXTURE_2D      ;break; 
	 default : TextureType=GL_TEXTURE_2D      ;break; 
	}
}

CGLTextureContainer::~CGLTextureContainer()
{
 Unload();
}

void CGLTextureContainer::Bind()
{
 Enable();
 glBindTexture(TextureType,TextureID);
}

void CGLTextureContainer::Enable()
{
 glEnable(TextureType);
}

void CGLTextureContainer::Disable()
{
 glDisable(TextureType);
}

DWORD CGLTextureContainer::GetInnerID()
{
  return TextureID;
}

DWORD CGLTextureContainer::GetTextureType()
{
	return TextureType; 
}

void CGLTextureContainer::Unload()
{
 glDeleteTextures(1,(const unsigned int*)&TextureID);
}

bool CGLTextureContainer::SubTexture(CDWORD Mode,CDWORD Target,LPCBYTE Src,CDWORD Width,CDWORD Height,CDWORD offx,CDWORD offy)
{
  glTexSubImage2D(Mode2Type(Mode,TextureType),Target,offx,offy,Width,Height,0,0,Src);
  return true;
}
bool CGLTextureContainer::ScreenGrab(CDWORD Mode,CDWORD Target,CDWORD Width,CDWORD Height,CDWORD srcx,CDWORD srcy,CDWORD offx,CDWORD offy)
{
  glCopyTexSubImage2D(Mode2Type(Mode,TextureType), Target, srcx, srcy, offx, offy, Width, Height);
  //glCopy
  return true;
}

bool CGLTextureContainer::SubTexture(bVector2w To,u_Texture_Loader *Loader)
{
 switch(TextureType)
 {
  case GL_TEXTURE_2D:
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        To[0],
                        To[1],
                        Loader->Info.Width,
                        Loader->Info.Height,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        Loader->Info.pData);return true;
 }
 return false;
}

void CGLTextureContainer::GenerateTexture()
{
  GLuint t;
  glGenTextures(1, &t);
  TextureID=t;
  Enable();
  Bind();
}

void CGLTextureContainer::SetParams(CTextureParameters *Params)
{
    if(Params->GetWRAP_S()!=TEX_WRAP_NONE)glTexParameterf(TextureType,GL_TEXTURE_WRAP_S,_MAPWRAP(Params->GetWRAP_S()));
    if(Params->GetWRAP_T()!=TEX_WRAP_NONE)glTexParameterf(TextureType,GL_TEXTURE_WRAP_T,_MAPWRAP(Params->GetWRAP_T()));

    if(Params->GetWRAP_R()!=TEX_WRAP_NONE)glTexParameterf(TextureType,GL_TEXTURE_WRAP_R,_MAPWRAP(Params->GetWRAP_R()));
    //if(Params->GetWRAP_Q()!=TEX_WRAP_NONE)glTexParameterf(TextureType,GL_TEXTURE_WRAP_Q,_MAPWRAP(Params->GetWRAP_Q()));

	glTexParameteri(TextureType,GL_TEXTURE_MAX_ANISOTROPY_EXT,Params->GetAniso());

    glTexParameteri(TextureType, GL_GENERATE_MIPMAP_SGIS, Params->IsUseMipMapGeneration());
	switch(Params->GetFilter())
	{
	case TEX_FILTER_NEAR:
	    glTexParameterf(TextureType  , GL_TEXTURE_MAG_FILTER  , GL_NEAREST);
        glTexParameterf(TextureType  , GL_TEXTURE_MIN_FILTER  , GL_NEAREST);
		break;
	case TEX_FILTER_LINEAR:
	    glTexParameterf(TextureType  , GL_TEXTURE_MAG_FILTER  , GL_LINEAR);
        glTexParameterf(TextureType  , GL_TEXTURE_MIN_FILTER  , GL_LINEAR);
		break;
	case TEX_FILTER_BILINEAR:
	    glTexParameterf(TextureType  , GL_TEXTURE_MAG_FILTER  , GL_LINEAR);
        glTexParameterf(TextureType  , GL_TEXTURE_MIN_FILTER  , GL_LINEAR_MIPMAP_NEAREST);
		break;
	case TEX_FILTER_TRILINEAR:
	case TEX_FILTER_BICUBIC:
	    glTexParameterf(TextureType  , GL_TEXTURE_MAG_FILTER  , GL_LINEAR);
        glTexParameterf(TextureType  , GL_TEXTURE_MIN_FILTER  , GL_LINEAR_MIPMAP_LINEAR);
		break;
	}

    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,_MAPENV(Params->GetEnvMode()));
}

bool CGLTextureContainer::HaveAlpha()
{
  return Format==TEX_RGBA;
}

bool CGLTextureContainer::Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height,cint Depth,CTextureParameters *Params)
{
	static CTextureParameters DefParams((Textures::TEX_TYPE)TType);
	if(!Params)
		Params=&DefParams;
	glCheckError;
	SetTextureType(TType);
    GenerateTexture();
	glCheckError;
	SetParams(Params);
	glCheckError;
	DWORD  ifr ;
	DWORD  infr;
	switch(FORMAT)
	{
		case  TEX_RGB      : ifr=GL_RGB       ;infr=GL_RGB      ;break;
		case  TEX_RGBA     : ifr=GL_RGBA      ;infr=GL_RGBA     ;break;
		case  TEX_BGR      : ifr=GL_BGR       ;infr=GL_RGB      ;break;
		case  TEX_BGRA     : ifr=GL_BGRA      ;infr=GL_RGBA     ;break;
		case  TEX_ALPHA    : ifr=GL_ALPHA     ;infr=GL_ALPHA    ;break;
		case  TEX_LUM      : ifr=GL_LUMINANCE ;infr=GL_LUMINANCE;break; 
	};
	this->Format=FORMAT;
	glCheckError;
	switch(TextureType)
	{
	case GL_TEXTURE_1D:
		glTexImage1D(TextureType,0,infr,Width,Height,ifr,GL_UNSIGNED_BYTE,0);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP:
		glTexImage2D(TextureType,0,infr,Width,Height,0,ifr,GL_UNSIGNED_BYTE,0);
		break;
	case GL_TEXTURE_3D:
		glTexImage3D(TextureType,0,infr,Width,Height,Depth,0,ifr,GL_UNSIGNED_BYTE,0);
		break;
	}
    glCheckError;
	this->Width =Width;
	this->Height=Height;
	this->Depth =Depth;
	return true;
}

bool CGLTextureContainer::SetData(LPCVOID Data,int MipMap)
{
	DWORD  ifr ;
	DWORD  infr;
	Bind();
	switch(this->Format)
	{
		case  TEX_RGB      : ifr=GL_RGB       ;infr=GL_RGB      ;break;
		case  TEX_RGBA     : ifr=GL_RGBA      ;infr=GL_RGBA     ;break;
		case  TEX_BGR      : ifr=GL_BGR       ;infr=GL_RGB      ;break;
		case  TEX_BGRA     : ifr=GL_BGRA      ;infr=GL_RGBA     ;break;
		case  TEX_ALPHA    : ifr=GL_ALPHA     ;infr=GL_ALPHA    ;break;
		case  TEX_LUM      : ifr=GL_LUMINANCE ;infr=GL_LUMINANCE;break; 
	};
	switch(TextureType)
	{
	case GL_TEXTURE_1D:
		glTexSubImage1D(TextureType,0,0,Width,ifr,GL_UNSIGNED_BYTE,Data);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP:
		glTexSubImage2D(TextureType,0,0,0,Width,Height,ifr,GL_UNSIGNED_BYTE,Data);
		break;
	case GL_TEXTURE_3D:
		glTexSubImage3D(TextureType,0,0,0,0,Width,Height,Depth,ifr,GL_UNSIGNED_BYTE,Data);
		break;
	}
     glCheckError;
	return true;
}

bool CGLTextureContainer::Load(u_Texture_Loader *Source,CTextureParameters *Params)
{
  GLint InternalFormat;
  GLint FColorModeA;
  
  Enable();
  if (!TEXTURE2D_MAX_SIZE_READED)
  {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&TEXTURE2D_MAX_SIZE);
    TEXTURE2D_MAX_SIZE_READED = true;
  }
  GenerateTexture();
  if (TextureID && !Source->IsEmpty())
  {
    SetParams(Params);
	Width =Source->Info.Width;
	Height=Source->Info.Height;
	Depth =Source->Info.Depth;

	bool AlphaCapable=!(Source->Info.IFormat & 0x01);

    if(AlphaCapable || (Params->IsUseAlpha() && AlphaCapable))
    {
      switch(Source->GetBPP())
      {
        case 8 : InternalFormat = GL_LUMINANCE8;break;
        case 16: InternalFormat = GL_RGBA4;     break;
        case 24: InternalFormat = GL_RGBA4;     break;
        case 32: InternalFormat = GL_RGBA8;     break;
      default:
        InternalFormat = GL_RGBA4;
      }
      if(Source->Info.IFormat==TEXTYPE_RGBA) {this->Format=TEX_RGBA;FColorModeA=GL_RGBA;}
      else									 {this->Format=TEX_RGBA;FColorModeA=GL_BGRA;};
    }
     else
    {
     switch(Source->GetBPP())
     {
       case 8 : InternalFormat = GL_LUMINANCE8;break;
       case 16: InternalFormat = GL_RGB5;      break;
       case 24: InternalFormat = GL_RGB8;      break;
       case 32: InternalFormat = GL_RGB10;     break;
     default:
       InternalFormat = GL_RGB8;
     }
     if(Source->Info.IFormat==TEXTYPE_RGB){this->Format=TEX_RGB;FColorModeA=GL_RGB;}
     else								  {this->Format=TEX_RGB;FColorModeA=GL_BGR;}
    }
     if (InternalFormat==GL_LUMINANCE8) FColorModeA=GL_LUMINANCE;

     if (Params->IsUseMipmaps())
     {
       if(TextureType==GL_TEXTURE_1D)
       gluBuild1DMipmaps(TextureType,
                        InternalFormat,
                        Source->Info.Width,
                        FColorModeA,
                        GL_UNSIGNED_BYTE,
                        Source->Info.pData); else
       gluBuild2DMipmaps(TextureType,
                        InternalFormat,
                        Source->Info.Width,
                        Source->Info.Height,
                        FColorModeA,
                        GL_UNSIGNED_BYTE,
                        Source->Info.pData);
     }
     else
     glTexImage2D(TextureType, 0, InternalFormat, Source->Info.Width, Source->Info.Height, 0,
        	      FColorModeA, GL_UNSIGNED_BYTE, Source->Info.pData);

     return true;
  }
/*  if(!TextureID)
  ConsoleOut->Printf("GL_Texture::Load - driver error %s\n",glGetLastErrorText().c_str());
  else
  ConsoleOut->Printf("GL_Texture::Load - no sourse\n");
*/
  return false;
}
