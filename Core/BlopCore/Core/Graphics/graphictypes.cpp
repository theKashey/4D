/*===========================================================================
    Заголовок: GraphicTypes.cpp
    Автор: Kashey
    Информация: Base types for drawable objects 

===========================================================================*/
#include "ProjectSpace.h"
#include "GenericGraphic.h"
#include "GraphicTypes.h"
//#include "Textures\Textures.h"
#include "Textures\in_Textures.h"
using namespace Draw;

struct NamePack
{
	 GString Name;
	 DWORD ID;
};         
typedef SmartVector<NamePack> NameArray;
typedef SmartVector<CMaterial> MaterialArray;
//typedef SmartVector<CShader>  ShaderArray;
           
NameArray NameShaders;
NameArray NameMaterials;

MaterialArray MaterialPack;
//ShaderArray   ShadersPack;
           

//--------------------------------------------------------------------

void CTransformer::Prepare(){WorkMode=0;};
void CTransformer::SetTRS(const bVector3f &Trans,const float RA,const bVector3f &Rot,const bVector3f &_Scale)
{
	WorkMode=1;
	Trans .SetTo(Translate);
	Rot   .SetTo(Rotation+1);	Rotation[0]=RA;
	_Scale.SetTo(Scale);
}
void CTransformer::SetTransform(const nMatrix &Matrix)
{
	WorkMode=2;
	memcpy(&Translate[0],&Matrix,4*4*4);
	ScaleEx[0]=ScaleEx[1]=ScaleEx[2]=1;
}
void CTransformer::SetScaleEx(const bVector3f &_Scale)
{
	_Scale.SetTo(ScaleEx);
}

DWORD StartModelClass=0;

DWORD CModel::GenClass()
{
	return ++StartModelClass;
}
CModel::CModel()
{
	Class=0;
}
CModel::~CModel()
{

}

CDrawableElement::~CDrawableElement()
{
}
void CDrawableElement::Compile()
{
}
void  CDrawableElement::DrawAsList(int LID)
{
	if(LID==0)
	{//DeleteList(GetListID());
	 ListID=0;
	 DrawParams&=~DRAW_AS_LIST;
	 return;
	}
	if(LID!=-1)
	{
		ListID=LID;//=RefList(LID);
	}
	DrawParams|=DRAW_AS_LIST;

}

//--------------------------------------------------------------------


