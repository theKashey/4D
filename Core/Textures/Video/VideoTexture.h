#pragma once

#include <vfw.h>	

struct VideoTwin
{
  PAVISTREAM		pavi;										// Handle To An Open Stream
  PGETFRAME			pgf;										// Pointer To A GetFrame Object
  VideoTwin();
  ~VideoTwin();
  void Close();
};

class DLLTYPE CVideoTexture:public CTextureContainer,public CAnimatedTexture
{
  VideoTwin       VideoData;
  static BYTE    *Data   [5];
  static HBITMAP  hBitmap[5];   //64,128,256,512,1024
  static HDC      hdc;
  static HDRAWDIB hdd;

  DWORD Width;
  DWORD Height;
  BYTE  thisHB;

  float lastframe;
  float thisframe;
  int   lastframegrabbed;

  int   mpf;

  DWORD ISIZE;
  bool  needDDD;

  CTextureContainer *DrawTarget;

  

public:
  bool OpenAVI(LPCSTR Name,DWORD _ISIZE=0);
  void GrabAVIFrame(int frame);

 CVideoTexture();
 ~CVideoTexture();
 void Close();

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

 bool Load(LPCSTR FileName);
 static void ShutDown();
};