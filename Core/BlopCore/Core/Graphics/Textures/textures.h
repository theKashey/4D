#ifndef TexturesH
#define TexturesH

#include "u_Texture.h"

#include "in_Textures.h"
using namespace Textures;



class DLLTYPE CTextureContainer
{
public:
 virtual void Bind   ()=0;
 virtual void Enable ()=0;
 virtual void Disable()=0;
 virtual void Unload ()=0;
 virtual bool Load(u_Texture_Loader *Source,CTextureParameters *Params)=0;
 virtual bool SubTexture(bVector2w To,u_Texture_Loader *Loader)=0;
 virtual bool SubTexture(CDWORD Mode,CDWORD Target,LPCBYTE Src,CDWORD Width,CDWORD Height,CDWORD offx=0,CDWORD offy=0)=0;
 virtual bool ScreenGrab(CDWORD Mode,CDWORD Target,CDWORD Width,CDWORD Height,CDWORD srcx=0,CDWORD srcy=0,CDWORD offx=0,CDWORD offy=0)=0;
 virtual DWORD GetInnerID()=0;
 virtual DWORD GetTextureType()=0;
 virtual bool Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height=-1,cint Depth=-1,CTextureParameters *Params=NULL)=0;
 virtual bool SetData(LPCVOID Data,int MipMap=-1)=0;
 virtual bool HaveAlpha()=0;
 virtual bool Load(LPCSTR FileName);
 virtual void SetParams(CTextureParameters *Params)=0;
};

class DLLTYPE CGLTextureContainer: public CTextureContainer
{
  DWORD TextureID;
  DWORD TextureType;
  DWORD Width,Height,Depth,Format;
 public:
 CGLTextureContainer();
~CGLTextureContainer();
 void Bind()   ;
 void Enable() ;
 void Disable();
 void Unload() ;
 void SetTextureType (CDWORD Type);
 bool Load           (u_Texture_Loader *Source,CTextureParameters *Params);
 bool SubTexture     (bVector2w To,u_Texture_Loader *Loader);
 bool SubTexture     (CDWORD Mode,CDWORD Target,LPCBYTE Src,CDWORD Width,CDWORD Height,CDWORD offx=0,CDWORD offy=0);
 bool ScreenGrab     (CDWORD Mode,CDWORD Target,CDWORD Width,CDWORD Height,CDWORD srcx=0,CDWORD srcy=0,CDWORD offx=0,CDWORD offy=0);
 DWORD GetInnerID    ();
 DWORD GetTextureType();

 bool Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height=-1,cint Depth=-1,CTextureParameters *Params=NULL);
 bool SetData(LPCVOID Data,int MipMap=-1);
 void GenerateTexture();
 void SetParams(CTextureParameters *Params);

 bool HaveAlpha();
};

class DLLTYPE CAnimatedTexture
{
    float  currenttime;
    float  framespersecond;
    float  currentframe;
    float  maxframe;
    BYTE   EndMode;
public:
    CAnimatedTexture();
    virtual float GetFrame();
    virtual void Reset();
    void SetEndMode(BYTE Mode);
    void SetMaxFrame(float MF);
    void SetFreq    (float FREQ);
    void SetFrame   (float FR);
};


class CTexture;
typedef CBSPDividorLookUpArray<char ,DWORD> _TEXNAMETOID;

class DLLTYPE CTexture
{
  static _TEXNAMETOID LookUp;

 CTextureContainer *TextureContainer;
 u_Texture_Loader  *ThisLoader;
 WORD               CountID;
 WORD               Width,Height,Type;

 static CTexture *IsTextureLoaded(GString Source,CTextureParameters *Params);
 u_Texture_Loader *GetLoader(GString Source);

 bool LoadFrom(u_Texture_Loader *Loader,CTextureParameters *Params);
public:
 CTexture();
~CTexture();

 bool Load      (LPCSTR Source,CTextureParameters *Params=NULL);
 bool SubTexture(bVector2w To,u_Texture_Loader *Loader);
 u_Texture_Loader * GetData();

 WORD GetWidth();
 WORD GetHeight();

 void Bind();
 void Disable();
 DWORD GetInnerID();
 DWORD GetTextureType();
 DWORD GetType();
 void Unload();

 bool Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height=-1,cint Depth=-1,CTextureParameters *Params=NULL);
 bool SetData(LPCVOID Data,int MipMap=-1);
 bool HaveAlpha();
};

CTextureContainer *GetTextureContainer();
//extern CTextureParameters TEXTURE_DEFAULT_FONT;



#endif
