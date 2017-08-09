#include "projectspace.h"
#include "blopCull.h"

using namespace CULL;

_fCullFarbic cFabric=NULL;

void CULL::RegisterCullFabric(_fCullFarbic F)
{
  cFabric=F;
}

class CCullNewWraper:public CCullElement
{
public:
	CCullNewWraper():CCullElement()
	{
	}
};

PCullElement CULL::CreateCullElement()
{
	if(cFabric)return cFabric();
	else return new CCullNewWraper;
}

