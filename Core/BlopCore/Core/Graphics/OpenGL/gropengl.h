#pragma once

#include "Env\grBleanding.h"
#include "Env\grZBuffer.h"

struct COpenGLModes
{
	BYTE CullingMode;
	BYTE ZMode;
	BYTE ZOn;
	BYTE ZWrite;
	BYTE ColorWrite;
	int  MatrixDepth[8];
	BYTE CurMatrix;

	COpenGLModes();
//	void PushMatrix();
//	void PopMatrix();
    void PushMatrix(){++MatrixDepth[CurMatrix];}
    void PopMatrix (){++MatrixDepth[CurMatrix];}
};

class DLLTYPE COpenGL : public CGenericGraphic
{
	DWORD           GLRC;
	HDC             DC;
	CgrBlending     Blending;
	CgrZBuffer      ZBuffering;
	float           CurrentTick;
	COpenGLModes    Modes;
	Draw::cgShaderMng     shaderManager ;

    DWORD           NumTriDrawn,NumIndsDrawn,VDeltaCashe,NumBatches,NumShaderBatch;

    typedef SmartVector<grRightBeforeSwapIntrStruct> VgrRightBeforeSwapIntrStruct;
    VgrRightBeforeSwapIntrStruct RightBeforeSwapV;
    VgrRightBeforeSwapIntrStruct RightAfterSwapV;

	void  Swarp();
	void  Clear();
	void  Do   ();

    static bool core_IN_GL_READY;
protected:
	void SessionCleanUp();
public:
    void RegisterCallBack_RightBeforeSwap(grRightBeforeSwapIntrStruct Str);
    void RegisterCallBack_RightAfterSwap(grRightBeforeSwapIntrStruct Str);

	COpenGL(BlopBaseObject *_Driver);
	~COpenGL(void);

	Draw::cgShaderMng* Shaders();

	bool  Start();
	void  Stop ();
	DWORD  Tick (float Time);

	bool  Init ();
	void  DoParticles();
	void  DoVAObjects();

	void PushMatrix();
	void ApplyTransform(Draw::CTransformer &TR);
	void PopMatrix();

	bool CallShader(Draw::BShader &Shader);
	virtual bool IsSupported(DWORD ID);

	BINLINE void ForseClear(DWORD Mode);
	BINLINE void SetBlending(BlendingTypes MODE);


	BINLINE void CullFace  (BYTE Mode);
	BINLINE void SetZMode  (BYTE Mode);
	BINLINE void SetDrawing(BYTE CMode,BYTE DMode);

	BINLINE void EnableWrite (BYTE LAYER);
	BINLINE void DisableWrite(BYTE LAYER);


	DEF_VIRTUA void SetMaterial(const DWORD M);
	DEF_VIRTUA void SetTexture (const DWORD T);
	DEF_VIRTUA void FlushRV(const Draw::CRenderVertex *RV);
	DEF_VIRTUA void FlushIV(const Draw::CRenderVertex *RV);

    bool ReadPixel (const float x,const float y,const float W,const float H,BYTE MODE,DWORD &out);
    bool ReadPixel (const float x,const float y,const float W,const float H,BYTE MODE,DWORD *out);

	bool ReadPixelf(const float x,const float y,const float W,const float H,BYTE MODE,float &out);
    bool ReadPixelf(const float x,const float y,const float W,const float H,BYTE MODE,float *out);

	//IM
	DEF_VIRTUA bool SetIMColor(const bVector4f &Color);

	DEF_VIRTUA bool EnableVSShader(LPCSTR SName);
	DEF_VIRTUA bool DisableVSShader(bool Forsed=false);
    DEF_VIRTUA Draw::cgShader *GetCurrentVertexShader();
    DEF_VIRTUA Draw::cgShader *GetUsedVertexShader();

    static bool Iscore_IN_GL_READY();
};

#define glCheckError inglCheckError(__FUNCTION__,__LINE__)

void DLLTYPE inglCheckError(LPCSTR f,long l);

