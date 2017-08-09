/*===========================================================================
	Заголовок: cgShader.h
    Автор: Kulib
	Информация: framework для работы с сг

===========================================================================*/

#ifndef cgShader_h
#define cgShader_h


#include "../Data/RenderVertex.h"

struct cgParam
{
	GString name;
	DWORD   body;
};

#define GETBODY(a) ((CGparameter)a)


#define MAX_SHADER_NAME 256
#define CG_UNDEF_TYPE  0
#define CG_VERTEX_TYPE 1
#define CG_PIXEL_TYPE  2


#define NUM_CGP_TMU 16

class CShaderExtention;

class CCurrentTextureSet
{
public:
    //normal cube map
	DWORD NormalCubeMap;
	void SetNormalMap(const DWORD Src){	NormalCubeMap=Src;}
	//filter
	DWORD FilterCubeMap;
	void SetFilterMap(const DWORD Src){	FilterCubeMap=Src;}
	//shadow
	DWORD Shadow2D;
	DWORD ShadowCube;
	void SetShadow2D  (const DWORD Src,const bool Del=false){	Shadow2D  =Src;if(Del)ShadowCube=0;}
	void SetShadowCube(const DWORD Src,const bool Del=false){	ShadowCube=Src;if(Del)Shadow2D  =0;}
	//attention
	DWORD Atten1D;
	DWORD Atten2D;
	DWORD Atten3D;
	DWORD Atten1DQ;
	void SetAtten1D (const DWORD Src){	Atten1D =Src;}
	void SetAtten2D (const DWORD Src){	Atten2D =Src;}
    void SetAtten3D (const DWORD Src){	Atten3D =Src;}
	void SetAtten1DQ(const DWORD Src){	Atten1DQ=Src;}
	//reflection
	DWORD ReflectionCube;
	void SetReflectionCube(const DWORD Src){	ReflectionCube=Src;}
	CCurrentTextureSet()
	{
		ZeroMemory(this,sizeof(CCurrentTextureSet));
	}
	
};

typedef CCurrentTextureSet* PCurrentTextureSet;

PCurrentTextureSet GetCurrentTextureSet();

struct CShaderLuaInVar
{
	GString shaderparam;
	int     paramID;
	GString Command;
	CShaderLuaInVar()
	{
		paramID=0;
	}
	CShaderLuaInVar(GString CMD,GString SP)
	{
	  shaderparam=SP;
	  Command    =CMD;
	  paramID    =0;
	}
};

typedef SmartVector<CShaderLuaInVar> CShaderLuaInVarVector;

class cgShader;

class cgBaseInstrument
{
protected:
    cgShader *Driver;
    bool      Instrumented;
public:
    cgBaseInstrument();
    void SetDriver(cgShader *D);
    virtual DWORD GetCode()=0;
    virtual bool Instrument()=0;
};

class CModelShaderInstrument;

class DLLTYPE cgShader
{
private:
	DWORD   profile;
	DWORD   context;
	DWORD   program;

	typedef SmartVector<cgParam> cgParamPack;
	cgParamPack params;

	DWORD    Type;
	bool     Disabled;
	Draw::CRenderStream RenderTarget;
	DWORD    ShaderFlag;
	DWORD    TexBind[NUM_CGP_TMU];
	DWORD    Techs;
	GString  UniqueTech;
	cgShader *BaseShader;

	BYTE      fIsFPL,fIsARBProfile;

	CShaderLuaInVarVector InVars;
	GString               inFunc;
    typedef SmartVector<cgBaseInstrument*> InstrumentsVector;
    InstrumentsVector Instruments;
    DWORD   LetBeMyID;
protected:
	int cgp_modelViewProj;

	GString   filename;
	GString   CapName;
	DWORD     ExecuteInVar();
    bool AddInstrument(cgBaseInstrument *Instr);
public:
	cgShader();
	~cgShader();

	cgParam *GetParam(const DWORD ID);

    cgBaseInstrument *Instrument(DWORD ID);

	LPCSTR GetName(){return filename;}
    const GString &GName()const{return filename;}
    void InstrumentInit();

	cgShader* Retouch();
	cgShader* LoadBasePath(LPCSTR name,bool printMessages);

	LPCSTR    ParseShader(LPCSTR In,int Len);

	int  GetParameter(LPCSTR n);
	int SetNamedParameter(LPCSTR n);
	cgShader& SetParameter4f     (LPCSTR param,const float x,const float y,const float z,const float w);
	cgShader& SetParameter3fv    (LPCSTR param,const float*v);
	cgShader& SetParameter4fv    (LPCSTR param,const float*v);
	cgShader& SetParameter2f     (LPCSTR param,float x,float y);
	cgShader& SetParameterf      (LPCSTR param,const float v);
	cgShader& SetParameterArray1f(LPCSTR param,const long offset,const long nelements,const float *v);
	cgShader& SetParameterPointer(LPCSTR param,const int fsize,const DWORD type,const int stride,void *pointer);
  /**/
	cgShader& SetMatrixArrayf    (const int param,const DWORD Off,const DWORD Num,const float *matrix);
	cgShader& SetParameter4f     (const int param,const float x,const float y,const float z,const float w);
	cgShader& SetParameter3fv    (const int param,const float*v);
	cgShader& SetParameter4fv    (const int param,const float*v);
    cgShader& SetParameter2f     (const int param,const float x,const float y);
	cgShader& SetParameterf      (const int param,const float v);
	cgShader& SetParameterArray1f(const int param,const long offset,const long nelements,const float *v);
	cgShader& SetParameterPointer(const int param,const int fsize,const DWORD type,const int stride,void *pointer);

	//pix
	cgShader& SetTextureParameter    (LPCSTR param,unsigned int tex);
	cgShader& EnableTextureParameter (LPCSTR param);
	cgShader& DisableTextureParameter(LPCSTR param);
/**/	
	cgShader& SetTextureParameter    (const int param,unsigned int tex);
	cgShader& EnableTextureParameter (const int param);
	cgShader& DisableTextureParameter(const int param);

	bool Enable();
	void Disable();

	void StoreMPV();
	void IdentityTex(const int TMU);
	void LoadTex    (const int TMU);

	cgShader& LoadMatrix (const int MID,const neM4 &Matrix);
	cgShader& LoadMatrix (const int MID,const float *Matrix);

	inline bool IsARBProfile  ();
	inline bool IsARBFPProfile();
	inline bool IsFPL(){return fIsFPL==1;}
	       bool IsVS ();

	bool IsValid();

	virtual bool BindRD(const Draw::CRenderVertex &RV);
	virtual bool BindShader(BShader *Shader);
	virtual bool BindNDraw(CRenderVertex &RV,BShader *Shader,DWORD NumIndex,LPCVOID Index,DWORD IndexType=1);//DWORD
	virtual bool ClearShader();
	void DoDisable();
	void ShaderError(LPCSTR Format,...);

	friend class cgShader;
};
typedef cgShader *PcgShader;

#define CGI_SKIN 1

class CModelShaderInstrument:public  cgBaseInstrument
{
	int cgp_frame;
	int cgp_matrixstack;
	DWORD MaxMatrix,MinMatrix;
	float *MATRIXSTACK;
	DWORD   MATRIXMAX;
public:

    struct GPUBone
    {
      bVector3f Translate;
      neQ       Q;
      neM4      Matrix;
    };

	void SetFrame         (const float frame);
	void StartTransform();
	void AddMatrix(const DWORD ID,const float *mat);
    void AddBone(const DWORD ID,const GPUBone&Bone);
	void EndTransform();
	bool Instrument();

    virtual DWORD GetCode(){return CGI_SKIN;}
};

void cgStoreMPV();

void cgPushCustomTranform();
void cgStartCustomTranform();
void cgEndCustomTranform();
void cgPopCustomTransform();

PcgShader DLLTYPE GetCurrentVertexShader(bool AllowFPL=false);
PcgShader DLLTYPE GetCurrentPixelShader();

#endif