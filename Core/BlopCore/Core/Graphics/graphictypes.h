//BASIC DRAW SPACE

#pragma once

#include "Core\Graphics\Textures\in_Textures.h"
#include "Core\Graphics\Textures\Textures.h"

namespace Draw
{

#include "ShaderParams.h"

struct DLLTYPE CTransformer
{
	BYTE  WorkMode;
	float Translate[4];
	float Scale    [4];
	float Rotation [4];
	float W        [4];
	float ScaleEx  [3];
	void Prepare();
	void SetTRS(const bVector3f &Trans,const float RA,const bVector3f &Rot,const bVector3f &_Scale);
	void SetTransform(const nMatrix &Matrix);
	void SetScaleEx(const bVector3f &_Scale);
};

#include "Data/VBOElement.h"
#include "Data/RenderVertex.h"
#include "MaterialDef.h"
#include "Targets/PBuffer.h"

//material------------------------------------------------------

#define DRAW_AS_LIST 1

class DLLTYPE CDrawableElement
{
public:
	DWORD         Shader;
	CTransformer  Transform;
	BYTE          DrawParams;
	DWORD         ListID;
	CRenderVertex Object;
	CDrawableElement()
	{Shader=ListID=DrawParams=0;}
	~CDrawableElement();
	void Compile();
	DWORD GetListID(){return ListID;}
	void  DrawAsList(int LID=-1);
};

typedef SmartVector<CDrawableElement> CDrawableDataArray;
typedef	CDrawableDataArray *PDrawableDataArray;

class DLLTYPE CDrawableObject
{
public:
	CDrawableObject(){}
	~CDrawableObject(){}
   virtual void DoClear(){};
   virtual void DoUpdate(){};
   virtual PDrawableDataArray GetData(CDrawableElement *Out=NULL){return NULL;}
//   virtual PDrawableDataArray GetData(PDrawableDataArray Data,DWORD Layer){return NULL};
};

class DLLTYPE CModel:public CDrawableObject
{
	DWORD Class;
public:
	DWORD GetClass(){return Class;}
	void  SetClass(DWORD C){Class=C;}
	static DWORD GenClass();
	CModel();
	~CModel();
};

#include "shaders/cgShader.h"
#include "shaders/cgShader_mng.h"

#include "models/skinmodel.h"

};
