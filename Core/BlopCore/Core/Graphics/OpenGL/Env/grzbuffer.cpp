#include "ProjectSpace.h"
#include "grzbuffer.h"
#include "coregraphics.h"

CgrZBuffer::CgrZBuffer(void)
{
	Enabled=false;
}

CgrZBuffer::~CgrZBuffer(void)
{
    Disable();
    Enabled=false;
}

inline GLenum DecodeTo(DWORD Mode)
{
	switch(Mode)
	{
	case ZLESS    :return GL_LESS;
	case ZLEQUAL  :return GL_LEQUAL;
	case ZEQUAL   :return GL_EQUAL;
	case ZGREATER :return GL_GREATER;
	case ZGEQUAL  :return GL_GEQUAL;
	default:_assume(0);
	}
	return GL_LEQUAL;
}

void CgrZBuffer::Disable()          {glDisable(GL_DEPTH_TEST);Enabled=false;}
void CgrZBuffer::Enable ()          {if(!Enabled)glEnable(GL_DEPTH_TEST);Enabled=true;}
void CgrZBuffer::EnableMode (DWORD Mode){if(!Enabled)Enable();glDepthFunc(DecodeTo(Mode));}
void CgrZBuffer::Clear  ()           {};
