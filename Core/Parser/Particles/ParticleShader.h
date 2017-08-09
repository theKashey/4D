#ifndef _PARTICLE_SHADER_H_
#define _PARTICLE_SHADER_H_

class CParticleShaderManager;

class DLLTYPE CParticleParams
{
public:
	struct DLLTYPE PPLE
	{
		WORD ID;
		WORD Type;
	    DWORD Data;
		PPLE();
		void  SetID(WORD T){ID=T;}
		float GetFloat(int ID=0);
		const float* GetVector(int ID=0);
		void  PushFloat(float F);
		void  PushVector(const float* F);
	};
	typedef SmartVector<PPLE>  CPParamList;
private:
	CPParamList List;
	void AddParam(PPLE PP){List.push_back(PP);}
	int CGET;
public:
	void ResetGet(){CGET=0;}
	PPLE* GetNextParam(){if(CGET<List.size())return &List[CGET++];else return NULL;}
	DWORD GetNumParams();
	PPLE  GetParam(int ID);
	friend class CParticleShaderManager;
};



class DLLTYPE CParticleShader
{
	CParticleParams listEmiter,
		            listRegister,
					listController,
					listKiller;
	GString EmiterName;
	GString ControllerName;
	GString RegisterName;
	GString KillerName;
	GString TextureName;
	GString CallBackName;
	GString CallBackParam;
	GString CentralTexture;
	DWORD   create_DEFCOUNT,
		    create_CHARGETYPE,
			create_CHARGECOUNT,
			create_USECENTER,
			create_BLEND;
	DWORD   willRotate,willColor;
	void SetSybSetName(LPCSTR Name);
	bVector3f SystemSize;
public:
	bVector3f Color;
	bVector3f PColor;
	bool      AdvanceEmite;
	CParticleShader();


	LPCSTR GetCallBack     ();
	LPCSTR GetCallBackParam();
	bVector3f GetSystemSize();

	LPCSTR GetSybset       ();
	LPCSTR GetMainTextureName   ();
	LPCSTR GetCentralTextureName();
	DWORD  GetDefCreteCount();
	DWORD  GetChargeType   ();
	DWORD  GetChargeCount  ();
	DWORD  GetCenterUsage  ();
	DWORD  GetRotatableFlag();
	DWORD  GetColorFlag    ();
	DWORD  GetBlendingType ();
	CParticleParams *GetEmiterParams    (int Type=0);
	CParticleParams *GetRegisterParams  (int Type=0);
	CParticleParams *GetControllerParams(int Type=0);
	CParticleParams *GetKillerParams    (int Type=0);

	LPCSTR GetEmiterName    (){return EmiterName    ;}
    LPCSTR GetControllerName(){return ControllerName;}
    LPCSTR GetRegisterName  (){return RegisterName  ;}
    LPCSTR GetKillerName    (){return KillerName    ;}
	friend class CParticleShaderManager;
};

typedef CParticleShader* PParticleShader;

PParticleShader DLLTYPE GetParticleShader(LPCSTR Name);
DWORD DLLTYPE RegisterParticleShader(LPCSTR Name,CParticleShader *Shad);

class DLLTYPE CParticleShaderManager: public CParser
{
	   	    int     errors;
			int     Depth;

	void    ShaderError(LPCSTR Error,...);
	void    ParseError (LPCSTR Error,...);
	void    ShaderNotImp();

	//void    ConstructBSP();
	void	InvalidToken(void);

	char *	GetFirstShader(void);
	char *	GetNextShader (void);
	void	ParseShader   (LPCSTR ShaderName);

	float   GetFloat();
	LPCSTR  GetParameter(bool Won=true);
	bool    GetName(void);

	void    PerformEmiterSet(CParticleParams *Params,int &Depth);


public:
	CParticleShaderManager(void);
	~CParticleShaderManager(void);
	void StartParseParticles();
	void StopParseParticles();
};

void DLLTYPE InitParticleSybsetShaders();


#endif