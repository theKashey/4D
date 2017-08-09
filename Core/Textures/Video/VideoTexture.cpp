#include "stdafx.h"
#include "ProjectGraphic.h"
#include "VideoTexture.h"

#pragma comment( lib, "vfw32.lib" )	


// Flips The Red And Blue Bytes ()
void flipIt(void* buffer,const WORD ISIZE)

{
	void* b = buffer;                                           // Pointer To The Buffer
    const DWORD IS=ISIZE*ISIZE;
	__asm														// Assembler Code To Follow
	{
		mov ecx, IS     										// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b												// Points ebx To Our Data (b)
		label:													// Label Used For Looping
			mov al,[ebx+0]										// Loads Value At ebx Into al
			mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
			mov [ebx+2],al										// Stores Value In al At ebx+2
			mov [ebx+0],ah										// Stores Value In ah At ebx
			
			add ebx,3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}

BYTE *   CVideoTexture::Data   [5];//=NULL;
HBITMAP  CVideoTexture::hBitmap[5];//=NULL;
HDC      CVideoTexture::hdc       =NULL;
HDRAWDIB CVideoTexture::hdd 	  =NULL;

typedef SmartVector<VideoTwin*> CVideoPack;
CVideoPack VideoPack;

VideoTwin::VideoTwin()
{
  pavi=NULL;
  pgf =NULL;
  VideoPack.push_back(this);
}
VideoTwin::~VideoTwin()
{
  for(CVideoPack::iterator it=VideoPack.begin();it!=VideoPack.end();++it)
      if(*it==this){*it=NULL;break;}
  Close();
}
void VideoTwin::Close()
{
 if(!GetCoreState(CS_GLOBALSTOP))
  {
   try{if(pgf)AVIStreamGetFrameClose(pgf );}catch(...){};
   try{if(pavi)AVIStreamRelease     (pavi);}catch(...){};
  }
}



void CVideoTexture::ShutDown()
{
 for(int i=0;i<5;++i)
 DeleteObject(hBitmap[i]);										// Delete The Device Dependant Bitmap Object
 DrawDibClose(hdd);
 try
 {
  for(CVideoPack::iterator it=VideoPack.begin();it!=VideoPack.end();++it)
      if(*it)(*it)->Close();
 }
 catch(...){}
 try
 {
  AVIFileExit();
 }catch(...){};
}

bool CVideoTexture::OpenAVI(LPCSTR _szFile,DWORD _ISIZE)										// Opens An AVI File (szFile)
{
    AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info  
    GString gFile=bGetRootPath()+_szFile;
    LPCSTR szFile=gFile;
    static bool AVIinited=false;
    if(!AVIinited)
    {
      AVIFileInit();												// Opens The AVIFile Library
      AVIinited=true;
      hdd = DrawDibOpen();
    }
	// Opens The AVI Stream
	if (AVIStreamOpenFromFile(&VideoData.pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
	{
		// An Error Occurred Opening The Stream
		VideoData.pavi=NULL;
        Error2Console(SYS_TEXTURES,"unable to open avi file [%s]",szFile);
        return false;
	}

	AVIStreamInfo(VideoData.pavi, &psi, sizeof(psi));						// Reads Information About The Stream Into psi
	Width =psi.rcFrame.right-psi.rcFrame.left;					// Width Is Right Side Of Frame Minus Left
	Height=psi.rcFrame.bottom-psi.rcFrame.top;					// Height Is Bottom Of Frame Minus Top
    needDDD=true;

    if(_ISIZE) ISIZE=_ISIZE;
    else
    {
     ISIZE=(MAX(Width,Height));     
    }
    Update2pow2(ISIZE);
    if(ISIZE<64  )ISIZE=64;
    if(ISIZE>1024)ISIZE=1024;
    switch(ISIZE)
    {
        case 64  :thisHB=0;break;
        case 128 :thisHB=1;break;
        case 256 :thisHB=2;break;
        case 512 :thisHB=3;break;
        case 1024:thisHB=4;break;
    }
    if(ISIZE==Width && ISIZE==Height)
        needDDD=false;
    else
        Error2Console(SYS_TEXTURES,"Warning : avi file [%s] have non-power-of-2 size(%d!=(%d,%d))",_szFile,ISIZE,Width,Height);

	lastframe=AVIStreamLength(VideoData.pavi);							// The Last Frame Of The Stream

	mpf=AVIStreamSampleToTime(VideoData.pavi,lastframe)/lastframe;		// Calculate Rough Milliseconds Per Frame
    CAnimatedTexture::SetMaxFrame(lastframe);
    CAnimatedTexture::SetFreq(mpf);
    if(!hdc)
    {
        hdc=CreateCompatibleDC(0);

        BITMAPINFOHEADER	bmih;
	    bmih.biSize        = sizeof (BITMAPINFOHEADER);				//Size Of The BitmapInfoHeader
	    bmih.biPlanes      = 1;										// Bitplanes	
	    bmih.biBitCount    = 24;									// Bits Format We Want (24 Bit, 3 Bytes)
	    bmih.biWidth       = 64;									// Width We Want (ISIZE Pixels)
	    bmih.biHeight      = 64;									// Height We Want (ISIZE Pixels)
	    bmih.biCompression = BI_RGB;								// Requested Mode = RGB
        for(int i=0;i<5;++i)
        {
	     hBitmap[i] = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&Data[i]), NULL, NULL);
	     bmih.biWidth      *= 2;
	     bmih.biHeight     *= 2;
        }
        // Select hBitmap Into Our Device Context (hdc)
    }
	VideoData.pgf=AVIStreamGetFrameOpen(VideoData.pavi, NULL);						// Create The PGETFRAME	Using Our Request Mode
	if (VideoData.pgf==NULL)
     return false;
    return true;
}

void CVideoTexture::GrabAVIFrame(int frame)									// Grabs A Frame From The Stream
{
    static MakeSectionProfile(graber,"video");
    Profile(graber);
    if(lastframegrabbed==frame) return;
    lastframegrabbed=frame;
	LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
	BYTE *pdata=NULL;
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(VideoData.pgf, frame);	// Grab Data From The AVI Stream
	pdata=(BYTE *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
    static DWORD lastHB=0;
    if(thisHB!=lastHB)
      SelectObject (hdc, hBitmap[thisHB]);
    lastHB=thisHB;
    if(needDDD)
    {
	  DrawDibDraw (hdd, hdc, 0, 0, ISIZE, ISIZE, lpbi, pdata, 0, 0, Width, Height, 0);
      pdata=Data[thisHB];
    }
	//flipIt(Data[thisHB],ISIZE);												// Swap The Red And Blue Bytes (GL Compatability)

	// Update The Texture
    if(DrawTarget)
        {
          glCheckError;
          DrawTarget->SetData(pdata,0);
          glCheckError;
        }
	//glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, ISIZE, ISIZE, GL_RGB, GL_UNSIGNED_BYTE, Data);
}

CVideoTexture::CVideoTexture():CAnimatedTexture()
{
  DrawTarget=NULL;
  VideoData.pavi=NULL;
  VideoData.pgf =NULL;
  Close();
}
CVideoTexture::~CVideoTexture()
{
  Close();
  if(DrawTarget)
      delete DrawTarget;
  DrawTarget=NULL;
}

 void CVideoTexture::Bind()
 {
   GrabAVIFrame(CAnimatedTexture::GetFrame());
   if(DrawTarget)
       {
         DrawTarget->Bind();
       }
 }
 void CVideoTexture::Enable() {if(DrawTarget)DrawTarget->Enable();}
 void CVideoTexture::Disable(){if(DrawTarget)DrawTarget->Disable();}
 void CVideoTexture::Unload() {if(DrawTarget)DrawTarget->Unload();}
 void CVideoTexture::SetTextureType (CDWORD Type){}
 bool CVideoTexture::Load           (u_Texture_Loader *Source,CTextureParameters *Params){return false;}
 bool CVideoTexture::SubTexture     (bVector2w To,u_Texture_Loader *Loader){return false;};
 bool CVideoTexture::SubTexture     (CDWORD Mode,CDWORD Target,LPCBYTE Src,CDWORD Width,CDWORD Height,CDWORD offx,CDWORD offy){return false;};
 bool CVideoTexture::ScreenGrab     (CDWORD Mode,CDWORD Target,CDWORD Width,CDWORD Height,CDWORD srcx,CDWORD srcy,CDWORD offx,CDWORD offy){return false;};
 DWORD CVideoTexture::GetInnerID    (){if(DrawTarget)return DrawTarget->GetInnerID();else return 0;}
 DWORD CVideoTexture::GetTextureType(){if(DrawTarget)return DrawTarget->GetTextureType();else return 0;}

 bool CVideoTexture::Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height,cint Depth,CTextureParameters *Params){return false;}
 bool CVideoTexture::SetData(LPCVOID Data,int MipMap){return false;}
 void CVideoTexture::GenerateTexture(){};
 void CVideoTexture::SetParams(CTextureParameters *Params){if(DrawTarget)DrawTarget->SetParams(Params);}
 bool CVideoTexture::HaveAlpha(){return false;}

 void CVideoTexture::Close()
 {
   VideoData.Close();

   lastframegrabbed=-1;
   VideoData.pgf =NULL;
   VideoData.pavi=NULL;
   CAnimatedTexture::Reset();
 }

 bool CVideoTexture::Load(LPCSTR FileName)
 {
   Close();
   thisframe=0;
   GString FN=FileName;
   LPCSTR L=strstr(FN,":");
   DWORD IS=0;
   if(L){IS=atoi(L+1);FN.SetAt((DWORD)L-(DWORD)FN.c_str(),0);}
   if(OpenAVI(FN,IS))
   {
      DrawTarget=new CGLTextureContainer();
      DrawTarget->Create(TMU_TEXTURE2D,TEX_BGR,ISIZE,ISIZE,0,NULL);
      return true;
   }
   return false;
 }

