/*===========================================================================
    Заголовок: BFont.cpp
    Автор: Kashey
    Информация: Font для OpenGL

===========================================================================*/
#include "ProjectSpace.h"
#include "ProjectGraphic.h"
#include "bfont.h"
#include "coregraphics.h"

#define   FONT_DEF_Z_OFFSET -8

using namespace Text;

CBFont::CBFont(void)
{
	fontTexture=0;
}
CBFont::~CBFont(void)
{
	Textures::UnloadTexture(fontTexture);
}

void CBFont::SetTextData(float x,float y,LPCSTR Text)
{
}
void CBFont::SetSizeData(bVector3f Sizes)
{
	width =Sizes[0];
	height=Sizes[1];
}

void CBFont::Create    (LPCSTR FileName,int w,int h)
{
  fontTexture=Textures::LoadTexture(FileName);
  SetColor(bVector4f(1,1,1,0));
  width  =0.3f;
  height =0.5f;
  symwidth=width;
  _width =1.0/w;
  _height=1.0/h;
  _w=w;
  _h=h;
  w2=_width*0.15;
}
void CBFont::DrawText(float x,float y,LPCSTR Text,...)
{
    /*char szError[1024];
	va_list ap;
    va_start(ap, Text);
    vsprintf(szError, Text, ap);
    va_end(ap);
	*/
	DOFORMAT(Err,Text);
	LPCSTR szError=Err;
	DrawText(x,y,FONT_DEF_Z_OFFSET,szError);
}
void CBFont::DrawText(float x,float y,float z,LPCSTR Text,...)
{
	if(!Text || Text[0]==0) return;
	/*char szError[1024];
	va_list ap;
    va_start(ap, Text);
    vsprintf(szError, Text, ap);
    va_end(ap);
	*/
    DOFORMAT(Err,Text);
	LPCSTR szError=Err;
	if(strcmp(szError,Text)!=0)
	 Text=szError;
    glColor4fv((GLfloat*)&Color);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(1);
	Textures::BindTexture(fontTexture);
	glBegin(GL_QUADS);
	const char *C;
	for(C=Text;*C;C++)
	{
	 char c=*C;
	 if(c!=' ')
	 {
	  char che=c-'!';
      BYTE row    =che / _w+1;
      BYTE column =che % _w;

	  float tx =column*_width-w2;
      float ty =1-row*_height;
      float ttx=tx+_width;
	  float tty=ty+_height;
	  //bVector3f V;V[3]=Z;V[1]=y;
      glTexCoord2f(tx ,tty);glVertex3f(x,y+height,z);
      glTexCoord2f(ttx,tty);glVertex3f(x+width,y+height,z);
      glTexCoord2f(ttx,ty );glVertex3f(x+width,y,z);
      glTexCoord2f(tx ,ty );glVertex3f(x,y,z);
	 }
      x+=symwidth;
     // c++;
     //}
	}
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
}

void CBFont::SetColor(bVector4f _color)
{
 Color=_color;
}

void CBFont::SetSymWidth(float W)
{
 symwidth=W;
}