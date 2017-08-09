#ifndef _CBFONT_H
#define _CBFONT_H
#pragma once

namespace Text
{

class CBFont
{
	DWORD     fontTexture;
	bVector4f Color;
	float     width, height,symwidth,
		     _width,_height,w2;
	int		 _w    ,_h;
    int       lt;
public:
	CBFont(void);
	~CBFont(void);
	void SetTextData(float x,float y,LPCSTR Text);
    void SetSizeData(bVector3f Sizes);
    void Create    (LPCSTR FileName,int w,int h);
    void DrawText(float x,float y,LPCSTR Text,...);
	void DrawText(float x,float y,float z,LPCSTR Text,...);
    void SetColor(bVector4f color);
	void SetSymWidth(float W);
};

void DLLTYPE DrawSysText(float x,float y,LPCSTR Text,...);

};

#endif
