// PBuffer.cpp: implementation of the CPBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "projectspace.h"
#include "projectgraphic.h"
#include "PBuffer.h"
#include "coregraphics.h"

using namespace Draw;
//DWORD SYS_GRAPHIC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define	MAX_ATTRIBS		20
#define	MAX_PFORMATS	20

template <typename T>
class	AttrList
{
	T	attrs [2*MAX_ATTRIBS];
	int	numAttrs;
public:
	AttrList ()
	{
		numAttrs = 0;
		clear ();
	}

	void	clear ()
	{
		for ( int i = 0; i < 2*MAX_ATTRIBS; i++ )attrs [i] = 0;
	}

	void	add ( int attr, T value )
	{
		attrs [2*numAttrs    ] = attr;
		attrs [2*numAttrs + 1] = value;
		numAttrs++;
	}

	int	getNumAttrs () const
	{
		return numAttrs;
	}

	const T * getAttrs () const
	{
		return attrs;
	}
};


CPBuffer::CPBuffer()
{
	Init=0;
}

CPBuffer::CPBuffer(WORD x,WORD y,DWORD Mode)
{
	Init=0;
	Create(x,y,Mode);
}
CPBuffer::~CPBuffer()
{
	if(RENDER->GRCollapsing)return;
	DeleteContex();
}

void CPBuffer::DeleteContex()
{
	if(Init)
	{
	  Disable();
	  wglDeleteContext       ( (HGLRC)hGLRC );
      wglReleasePbufferDCARB ( (HPBUFFERARB)pBuffer, (HDC)hDC );
	  wglDestroyPbufferARB   ( (HPBUFFERARB)pBuffer );
	}
	Init=0;
}

bool CPBuffer::Create(WORD x,WORD y,DWORD _Mode)
{
	DeleteContex();
	Mode=_Mode;
	RezX=x;
	RezY=y;
    HDC		hCurDc = wglGetCurrentDC      ();
	HGLRC	hCurRc = wglGetCurrentContext ();
	int		format;
	// check for extension support & initialization
	if ( wglChoosePixelFormatARB == NULL || wglCreatePbufferARB  == NULL ||
	     wglGetPbufferDCARB      == NULL || wglQueryPbufferARB   == NULL  ||
	     wglReleasePbufferDCARB  == NULL || wglDestroyPbufferARB == NULL )
		     return false;

/*	if ( (mode & modeTexture1D) || (mode & modeTexture2D) || (mode & modeTextureCubeMap) )
		if ( wglBindTexImageARB == NULL || wglReleaseTexImageARB == NULL || wglSetPbufferAttribARB == NULL )
			return false;
*/
											// query for pixel format
	AttrList <int>		intAttrs;
	AttrList <float>	floatAttrs;

	intAttrs.add ( WGL_SUPPORT_OPENGL_ARB,  GL_TRUE );
	intAttrs.add ( WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE );
	intAttrs.add ( WGL_PIXEL_TYPE_ARB,      WGL_TYPE_RGBA_ARB );
	intAttrs.add ( WGL_RED_BITS_ARB,        8 );
	intAttrs.add ( WGL_GREEN_BITS_ARB,      8 );
	intAttrs.add ( WGL_BLUE_BITS_ARB,       8 );
	intAttrs.add ( WGL_DOUBLE_BUFFER_ARB, (Mode & PBUFFER_DBUFFER ? GL_TRUE : GL_FALSE ) );

	const int AlphaMode=(Mode>>8)&3;
	if ( AlphaMode==3     )	intAttrs.add ( WGL_ALPHA_BITS_ARB, 16 );else
	if ( AlphaMode==2     )	intAttrs.add ( WGL_ALPHA_BITS_ARB, 8 );	else
	if ( AlphaMode==1     )	intAttrs.add ( WGL_ALPHA_BITS_ARB, 1 );

	const int DepthMode=(Mode>>10)&3;
	if ( DepthMode==3     )	intAttrs.add ( WGL_DEPTH_BITS_ARB, 32 );else
	if ( DepthMode==2     )	intAttrs.add ( WGL_DEPTH_BITS_ARB, 24 );else
	if ( DepthMode==1     )	intAttrs.add ( WGL_DEPTH_BITS_ARB, 16 );

	if ( IsStensilHaved() ) 	intAttrs.add ( WGL_STENCIL_BITS_ARB, 8 );

	//if ( mode & modeAccum      )	intAttrs.add ( WGL_ACCUM_BITS_ARB, 32 );

	if ( IsAlphaHaved()     )intAttrs.add ( WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE );
	else 			         intAttrs.add ( WGL_BIND_TO_TEXTURE_RGB_ARB , GL_TRUE );

	int			pixelFormats [MAX_PFORMATS];
	unsigned	numFormats = 0;

	if ( !wglChoosePixelFormatARB ( hCurDc, intAttrs.getAttrs (), floatAttrs.getAttrs (),
									MAX_PFORMATS, pixelFormats, &numFormats ) )
		return false;

	if ( numFormats < 1 )
		return false;

	format = pixelFormats [0];

	AttrList <int>	props;

	if ( IsTexture1D() || IsTexture2D() )
		if ( IsAlphaHaved() )props.add ( WGL_TEXTURE_FORMAT_ARB,  WGL_TEXTURE_RGBA_ARB );
		else   			     props.add ( WGL_TEXTURE_FORMAT_ARB,  WGL_TEXTURE_RGB_ARB );

	if ( IsTexture1D  () )props.add ( WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_1D_ARB );else
	if ( IsTexture2D  () )props.add ( WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB );else
	if ( IsTextureCUBE() )props.add ( WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_CUBE_MAP_ARB );

	if ( IsMipMapHaved() )
		props.add ( WGL_MIPMAP_TEXTURE_ARB, GL_TRUE );

	HDC			hdc;
	HGLRC		hGlRc;
	HPBUFFERARB	hBuffer;
	hBuffer = wglCreatePbufferARB ( hCurDc, format, RezX, RezY, props.getAttrs () );

	if ( hBuffer == NULL )
		return false;

	hdc = wglGetPbufferDCARB ( hBuffer );

	if ( hdc == NULL )
		return false;

	hGlRc = wglCreateContext ( hdc );

	wglShareLists ( hCurRc, hGlRc );


										// get real size
	wglQueryPbufferARB ( hBuffer, WGL_PBUFFER_WIDTH_ARB,  &RezX );
	wglQueryPbufferARB ( hBuffer, WGL_PBUFFER_HEIGHT_ARB, &RezY );

	pBuffer=(DWORD)hBuffer;
	hDC    =(DWORD)hdc;
	hGLRC  =(DWORD)hGlRc;

	Init=1;
	return true;

}

inline int Buf2WGL(const int &b)
{
	switch(b)
	{
  	  case BUFFER_FRONT_LEFT:return WGL_FRONT_LEFT_ARB;
	}
	return WGL_FRONT_LEFT_ARB;
}

bool CPBuffer::Bind(DWORD Texture,DWORD buf)
{
   buf=Buf2WGL(buf);
   TexID=Texture;
   if(Texture)
   {
	   if(IsTexture1D())glBindTexture(GL_TEXTURE_1D,Texture);else
	   if(IsTexture2D())glBindTexture(GL_TEXTURE_2D,Texture);else
	   if(IsTextureCUBE())glBindTexture(GL_TEXTURE_CUBE_MAP,Texture);
   }
   Locked=true;
   if(wglBindTexImageARB ( (HPBUFFERARB)pBuffer, buf ) != GL_FALSE)return true;
   else
   {
		Error2Console(SYS_GRAPHIC,"unable to bind pbuffer to texture\n");
		Locked=false;
		return false;
   }
}
bool CPBuffer::Unbind(int buf)
{
   buf=Buf2WGL(buf);
   if(TexID)
   {
	   if(IsTexture1D())glBindTexture(GL_TEXTURE_1D,TexID);else
	   if(IsTexture2D())glBindTexture(GL_TEXTURE_2D,TexID);else
	   if(IsTextureCUBE())glBindTexture(GL_TEXTURE_CUBE_MAP,TexID);
   }
	Locked=false;
	if(wglReleaseTexImageARB ( (HPBUFFERARB)pBuffer, buf ) != GL_FALSE)return true;
	else
	{
		Locked=true;
		Error2Console(SYS_GRAPHIC,"unable to unbind pbuffer from texture\n");
		return false;
	}
}
bool CPBuffer::SelectPlane(BYTE PlaneNum)
{
	if (!IsTextureCUBE())return false;
	glFlush ();
	return setAttr ( WGL_CUBE_MAP_FACE_ARB, GL_TEXTURE_CUBE_MAP_POSITIVE_X+PlaneNum );

}
bool CPBuffer::Enable ()
{
	if(isLost())
	{
//		Requery();
		Error2Console(SYS_GRAPHIC,"**************PBUFFER DEVICE LOST***************\n");
	}
	saveHdc   = (DWORD)wglGetCurrentDC      ();	// save previous target
	saveHglrc = (DWORD)wglGetCurrentContext ();
	return wglMakeCurrent ( (HDC)hDC, (HGLRC)hGLRC ) != 0;

}
bool CPBuffer::Disable()
{
    if ( saveHglrc == NULL || saveHdc == NULL )	return false;
	bool	result = wglMakeCurrent ( (HDC)saveHdc, (HGLRC)saveHglrc )!=0;
	saveHdc   = NULL;
	saveHglrc = NULL;
	return result;
}

bool CPBuffer::isLost () const
{
	int	lost = 0;
	wglQueryPbufferARB ( (HPBUFFERARB)pBuffer, WGL_PBUFFER_LOST_ARB, &lost );
	return lost != 0;
}


bool CPBuffer::setAttr ( int attr, int value )
{
    AttrList <int>	attrs;
	attrs.add ( attr, value );
	return wglSetPbufferAttribARB ( (HPBUFFERARB)pBuffer, attrs.getAttrs () ) != GL_FALSE;
}


PPBuffer CreatePBuffer(DWORD RezX,DWORD RezY,DWORD Type,DWORD Flags)
{
	return NULL;
//	return new CPBuffer(RezX,RezY,dNULL);
}

void     ReleasePBuffer(PPBuffer Target)
{
	delete Target;
}