#pragma once

#define BLOP_DEF_OPENGL

#define RENDER_CLEAR_DEPTH	  1
#define RENDER_CLEAR_COLOR	  2
#define RENDER_CLEAR_STENCIL  4

#include "Camera.h"
#include "GraphicTypes.h"
#include "Overlay/Overlay.h"



#define PASS_Z	        1
#define PASS_Z_FIRST	2
#define PASS_AMBIENT	3
#define PASS_LIGHT		4

class DLLTYPE CSceneNode
{
protected:
	struct CSceneNodeTwin
	{
	  DWORD ID;
	  DWORD Type;
	  DWORD Class;
	  CSceneNodeTwin()
	  {
		  ID=Type=Class=0;
	  }
	} ID;
	typedef SmartVector<CSceneNode*>CSceneNodes;
	typedef SmartVector<CSceneNodeTwin> CNodeHistory;


	CSceneNodes Childs;
private:
	CSceneNode *Parent;
	bool Enabled,Visibled;
	BYTE fNeedUpdate;
	static CNodeHistory ConstHostory;
	bool Collapsing;

public:
	bool IsEnabled ();
	bool IsVisible ();
	bool NeedUpdate();

	void Hide();
	void Show();
	void Enable();
	void Disable();
	void SetUpdate();
	void SetAllwaysUpdate();

	virtual void Update(CNodeHistory *H);
	virtual int  Draw  (CNodeHistory *H);

	virtual bool AddChild   (CSceneNode *Child,const DWORD *Passes=NULL);
	virtual void RemoveChild(CSceneNode *Child,const DWORD *Passes=NULL);

	CSceneNode();
	~CSceneNode();

};

class COpenGL;
#ifdef BLOP_DEF_OPENGL
#define DEF_RENDER COpenGL   //CGenericGraphic
#define DEF_VIRTUA 
#else
#define DEF_RENDER CGenericGraphic
#define DEF_VIRTUA virtual

#endif


#define RENDER (CGenericGraphic::GetDriver()) 

typedef void(*grRightBeforeSwapIntr)(DWORD Tag);
struct grRightBeforeSwapIntrStruct
{
  grRightBeforeSwapIntr CallBack;
  DWORD                 Tag;
  grRightBeforeSwapIntrStruct(grRightBeforeSwapIntr C,DWORD T)
   {
     CallBack=C;
     Tag     =T;
   }
  void Call()
  {
   if(CallBack)
    CallBack(Tag);
  }
};

class DLLTYPE CGenericGraphic : public BaseObject
{
	protected:
//    static _CDrawableArray              Objects;
//	static _CDrawableLayer              Layers;
	int  SessionDepth;
	virtual void SessionCleanUp(){};
//	static CCamera                     *Camera;
	static COverlay                    *Overlay;
	static DEF_RENDER                  *Driver;
public:
	static CCamera                     *Camera;
	static DEF_RENDER*                  GetDriver();
	static CSceneNode                   RootNode;
	static bool                         GRCollapsing;
	static COverlay * GetOverlay();
	CGenericGraphic(BlopBaseObject *_Driver);
	~CGenericGraphic(void);

	static CSceneNode* GetRoot();

	DEF_VIRTUA bool Init();
	DEF_VIRTUA bool Start();
	static CCamera& GetCurrentCamera();
    DEF_VIRTUA void ApplyTransform(Draw::CTransformer &TR){};
	DEF_VIRTUA bool IsSupported(DWORD ID){return false;}

	void    StartSession();
	void    EndSession  ();

	DEF_VIRTUA void SetMaterial(const DWORD M){};
	DEF_VIRTUA void SetTexture (const DWORD T){};
	DEF_VIRTUA void FlushRV(const Draw::CRenderVertex *RV){};

	//IM
	DEF_VIRTUA void SetIMColor(const bVector4f &Color){};
};

extern DWORD SYS_GRAPHIC;

#ifdef BLOP_DEF_OPENGL
#include "opengl/gropengl.h"
#endif

