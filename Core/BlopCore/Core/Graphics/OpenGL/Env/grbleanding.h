#pragma once

enum BlendingTypes
{
 BLEND_NULL   =0, //unknown
 BLEND_OFF    =1, //disable
 BLEND_ADD    =2, //GL_ONE,GL_ONE
 BLEND_MUL    =3, //GL_SRC_COLOR,GL_ONE
 BLEND_MUL2   =4, //GL_DST_COLOR,GL_SRC_COLOR
 BLEND_MUL3   =5, //GL_SRC_COLOR,GL_SRC_COLOR
 BLEND_ALPHA  =6 //GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
};

class DLLTYPE CgrBlending
{
	static bool Enabled;
public:
	CgrBlending(void);
   ~CgrBlending(void);
    
	void Enable ();
	void Disable();
	void Apply(const BlendingTypes Blend);//BYTE ParamA,BYTE ParamB);

};


typedef CgrBlending* PCgrBlending; 

extern PCgrBlending grBLENDER;