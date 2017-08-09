#ifndef _H_CULL_ELEMENT_
#define _H_CULL_ELEMENT_


#include "Core/Sys/Cull/blopCULL.h"
using namespace CULL;

class CSurCullElement:public CULL::CCullElement
{

  //occlusion query
	DWORD     OccID;
public:
	static  int  ThisFrame;
	CSurCullElement();
	~CSurCullElement();
	bool IsVisibled();
};

#endif