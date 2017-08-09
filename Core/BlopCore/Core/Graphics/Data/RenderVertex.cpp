#include "projectspace.h"
#include "projectgraphic.h"
#include "RenderVertex.h"

using namespace Draw;

CRenderStream::CRenderStream()
{
	ZeroMemory(this,sizeof(CRenderStream));
	VBOElem=0;

	VCount=3;
	V2Count=3;
	CCount=3;
	C2Count=3;
	NCount;
	BXCount=BYCount=BZCount=3;
	SBCount=SWCount=SBWCount=4;
	for(int i=0;i<32;++i)TCount[i]=2;
}


CRenderVertex::CRenderVertex(void)
{
	NumStreams =0;
	IndexOffset=0;
	Index      =0;
}

CRenderVertex::~CRenderVertex(void)
{
}

void CRenderVertex::Flush()const
{RENDER->FlushRV(this);
}

void CRenderVertex::FlushI()const
{
	RENDER->FlushIV(this);
}
