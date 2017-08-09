#include "ProjectSpace.h"
#include "grBleanding.h"
#include "coregraphics.h"

bool CgrBlending::Enabled=false;

PCgrBlending grBLENDER=NULL;


CgrBlending SysBlender;

CgrBlending::CgrBlending(void)
{
	if(!grBLENDER)grBLENDER=this;
	Enabled=false;
}

CgrBlending::~CgrBlending(void)
{
	Disable();
    if(grBLENDER==this)grBLENDER=NULL;
}

void CgrBlending::Enable()
{
	if(Enabled)return;
	glEnable(GL_BLEND);
	Enabled=true;
}
void CgrBlending::Disable()
{
	if(!Enabled)return;
	glDisable(GL_BLEND);
	Enabled=false;
}

void CgrBlending::Apply(const BlendingTypes A)//BYTE ParamA,BYTE ParamB)
{
	if(A!=BLEND_OFF)Enable();
	switch(A)
	{
     case BLEND_NULL  :
	 case BLEND_OFF   : Disable();break;
	 case BLEND_ADD   : glBlendFunc(GL_ONE      ,GL_ONE                 );break;
	 case BLEND_MUL   : glBlendFunc(GL_SRC_COLOR,GL_ONE                 );break;
	 case BLEND_MUL2  : glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR           );break;
	 case BLEND_MUL3  : glBlendFunc(GL_SRC_COLOR,GL_SRC_COLOR           );break;
	 case BLEND_ALPHA : glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );break;
	}
}
