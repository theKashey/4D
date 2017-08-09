#pragma once

#include "../Parser.h"
#include "ProjectDividors.h"

#define MAX_BUFFERS_PER_SOURCE 5

class CSoundShaderBuffer
{
public:
    GString Name;
    bool    OverrideLoop;
    bool    Streamed;
    CSoundShaderBuffer()
    {
        OverrideLoop=0;
        Streamed    =0;
    }
};

class CSoundSourceShader
{
public:
    bool Looped;
    float Gain;
    DWORD GainPack;
    DWORD DistModel;
    DWORD DistMax;
    DWORD DistFactor;
    DWORD NumBuffers;
    DWORD Class;
    bool  AfterLooped;
    CSoundShaderBuffer Buffers[MAX_BUFFERS_PER_SOURCE];
    CSoundSourceShader()
    {
        NumBuffers=0;
        Gain=0;
    }
    void AddBuffer(LPCSTR Name,bool Stream)
    {
      Buffers[NumBuffers].Name=Name;
      Buffers[NumBuffers].Streamed=Stream;
      if(NumBuffers<MAX_BUFFERS_PER_SOURCE)
      NumBuffers++;
    };
    void AlterLoopBuffer()
    {
        if(NumBuffers)
            Buffers[NumBuffers-1].OverrideLoop=true;
        AfterLooped=true;
    }
};

#define DISTANCE_MODEL_DEFAULT          0 //hardare
#define DISTANCE_MODEL_USERFALLOFF      1 // 1-dist
#define DISTANCE_MODEL_USERFALLOFFQ     2 // 1-dist^2
#define DISTANCE_MODEL_USERFALLOFFBQ    3 //(1-dist)^2
#define DISTANCE_MODEL_USERFALLOFFBSQR  4 //1-sqrt(dist)

class DLLTYPE CSoundShader: public CParser
{
	typedef CBSPDividorLookUpArray<char,int> _MYBSPDIVIDOR;
	static _MYBSPDIVIDOR BSPFor;
	static _MYBSPDIVIDOR BSPStorage;	
	typedef SmartVector<CSoundSourceShader      > _SourcePack;
	static _SourcePack SourcePack;
	static  bool BSPFuncInited;
	   	    int     errors;
			int     Depth;

	void    ShaderError(LPCSTR Error,...);
	void    ParseError (LPCSTR Error,...);
	void    ShaderNotImp();

	void    ConstructBSP();
	void	InvalidToken(void);

	char *	GetFirstShader(void);
	char *	GetNextShader (void);
	void	ParseShader   (LPCSTR ShaderName);

	float   GetFloat();
	LPCSTR  GetParameter();
	bool    GetName(void);

public:
	CSoundShader(void);
	~CSoundShader(void);
	void StartParseSounds();
	void StopParseSounds();

	bool GetShader(LPCSTR Name,CSoundSourceShader &Source);
	bool GetShader(LPCSTR Name,CSoundSourceShader *&Source);
};

typedef CSoundShader *PSoundShader;

PSoundShader DLLTYPE GetSoundShaderManager(); 