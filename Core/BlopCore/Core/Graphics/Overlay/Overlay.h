#pragma once

#include "Core\Graphics\OpenGL\Env\grBleanding.h"

class DLLTYPE COverlay
{
	void *AllocQuad();
	bool AddQuad(void *Q);
	typedef SmartVector<DWORD> DVector;
	DVector quads;
	Draw::CVBOElement       Store;
	Draw::CBaseRenderIndex  IndexStore;

	_Profilers::CSectionProfile pAddOver,pAddQuad;

	float scb_dx,scb_dy;


	int   WorkLevel;

	int   WorkSkip;
	double LastUpdateTime;
public:
	static DWORD qdef_FONT;
	LPVOID EnterPoint;
	DWORD AvaiableV,
          UsedV,NumOperations;

	COverlay(void);
	~COverlay(void);

	void DoSwap();
	void Render(bool Lock=true);
	bool IsSkipingFrame()const;
	void CommitChanges();
	void UpdateSkiping();


	void Init  ();

	void SetWorkLevel(int L);
    void QuadT     (int Texture, float x, float y, float w, float h, bVector4f Color,float repx=1, float repy=1,float offx=0,float offy=0,BlendingTypes Blend=BLEND_NULL);
	void Quad      (int shader, float x, float y, float w, float h, bVector4f Color,float repx=1, float repy=1,float offx=0,float offy=0,BlendingTypes Blend=BLEND_NULL);
	void ScissorBox(float x,float y,float w,float h);
	void String    (float x, float y, float size,LPCSTR Text,...);

	//friend class DEF_RENDER;
	
};
