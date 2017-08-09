#include "projectspace.h"
#include "CullElement.h"
//#include "../include/k_vbo.h"
//#include "../../shared/include/CollisionDetection.h"

using namespace CULL;

int  CSurCullElement::ThisFrame=0;


CSurCullElement::CSurCullElement():CCullElement()
{
 OccID=0; 
}

CSurCullElement::~CSurCullElement()
{
	OccID=0;
}

bool CSurCullElement::IsVisibled()
{
//	if(!world->frustum.SphereInGLFrustum(Position[0],Position[1],Position[2],Rad))return false;
	float min[3],max[3];for(int i=0;i<3;i++){ min[i]=Position[i]-Rad;max[i]=Position[i]+Rad;}
	//if(CollisionDetection::IsPointInsideAABB(world->current_cam->pos,min,max)) return true;

	if((OccStatus&1)==1)
	{
	  BYTE Ret=0;
	  BYTE LastFrameVisible=OccStatus&6;
	  const BYTE inFrameVis=LastFrameVisible;
	  const bool v2= (LastFrameVisible&2)==2;
	  const bool v4= (LastFrameVisible&4)==4;
	  if(v2 && !v4) Ret=1;
	  else
	  if(v4 && !v2) Ret=2;
	  else         
//		  Ret=k_GetOcclusionStatus(OccID);
	  {}

	  if(CSurCullElement::ThisFrame-FirstChecked>FrameOCCFreq)
	  {	
		  OccStatus&=0xF9;LastFrameVisible=0;
	  }
	  else if(v4)
		  if(CSurCullElement::ThisFrame-FirstChecked>FrameOCCFreqHalf)
		  {	
			  OccStatus&=0xF9;LastFrameVisible=0;
		  }

   	  if(!LastFrameVisible)
	  {}
//	    k_SetOcclusion(OccID,Position,Rad);
	  OccStatus&=0xF9;
	  if(Ret==2 && (OccStatus&0x80)!=0x80)
		  Ret=1;
	  if(Ret==1){if((LastFrameVisible&2)!=2){FirstChecked=CSurCullElement::ThisFrame;OccStatus|=2|4;}else OccStatus|=2; return true;}
	  if(Ret==2){if((LastFrameVisible&4)!=4){FirstChecked=CSurCullElement::ThisFrame;}else OccStatus|=4; return false;}
	  return false;
	}
	return true;

};


SmartVector<CSurCullElement> CullVector;

PCullElement SurCullFarbic()
{
	static CSurCullElement Elem;
	static CSurCullElement *SrcElem=NULL;
	if(!SrcElem) SrcElem=new CSurCullElement();
	//memcpy(&Elem,SrcElem,sizeof(Elem));
	//CullVector.push_back(Elem);
	CSurCullElement *ret;//= (CullVector.end()-1);//new CSurCullElement();
	ret=new CSurCullElement();

	memcpy(ret,SrcElem,sizeof(Elem));
	return ret;
}

class CCullRegister
{
public:
   CCullRegister()
   {
	  RegisterCullFabric(SurCullFarbic);
   }
};

CCullRegister CullRegister;

