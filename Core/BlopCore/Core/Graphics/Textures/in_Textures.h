#ifndef _TEXTURE_IN_H
#define	_TEXTURE_IN_H

namespace Textures
{

enum TMU_STATE
{
 TMU_INACTIVE	 =0,
 TMU_OFF		 =1,
 TMU_TEXTURE1D   =2,
 TMU_TEXTURE2D	 =3,
 TMU_TEXTURE3D	 =4,
 TMU_TEXTURECUBE =6,
 TMU_TEXTUREDEPTH=7
};

enum TEX_CUBE_TYPE
{
 TEX_CUBEMAP1   = 10,
 TEX_CUBEMAP2	= 11,
 TEX_CUBEMAP3	= 12,
 TEX_CUBEMAP4	= 13,
 TEX_CUBEMAP5	= 14,
 TEX_CUBEMAP6	= 15
};

	// Texture OFF method
enum TMU_OFF_METHOD
{
 TEX_DISABLE_ALL  =0,
 TEX_ENABLE_ALL   =1,
 TEX_22FORCE      =2,
 TEX_44FORCE      =3,
 TEX_16FORCE      =4,
 TEX_SINGLEFORCE  =5
};

enum TMU_FORMAT
{
	TEX_RGB       =1,
	TEX_RGBA      =2,
	TEX_ALPHA     =3,
	TEX_LUM       =4,
	TEX_BGR       =5,
	TEX_BGRA      =6
};

enum TEX_FILTER
{
 TEX_FILTER_NEAR	 =1,
 TEX_FILTER_LINEAR	 =2,
 TEX_FILTER_BILINEAR =3,
 TEX_FILTER_TRILINEAR=4,
 TEX_FILTER_BICUBIC	 =5
};

enum TEX_ENV
{
 TEX_ENV_MODULATE    =1
};

enum TEX_WRAP
{
 TEX_WRAP_NONE        = 0,
 TEX_WRAP_REPEAT      = 1,
 TEX_WRAP_CLAMP       = 2,
 TEX_WRAP_CLAMP2EDGE  = 3,
 TEX_WRAP_CLAMP2BORDER= 4,
 TEX_WRAP_MIRROR      = 5,
 TEX_WRAP_MIRRORONCE  = 6
};

enum TEX_TYPE
{				      
 TEX_TYPE_1D	      = 1,
 TEX_TYPE_2D		  = 2,
 TEX_TYPE_3D		  = 3,
 TEX_TYPE_CUBE	      = 4
};

enum ATM_TYPE
{
  ATM_REPLAY          =1,
  ATM_STOP            =2,
  ATM_PINGPONG        =3
};

class DLLTYPE CTextureParameters
{
  char UseMipMapGeneration;
  char UseAlpha;
  char UseMipmaps;
  int  Filter;
  int  Anisotropic;
  int  EnvMode;
  int  WRAP_S,WRAP_T,WRAP_R,WRAP_Q;
public:
  static CTextureParameters GET();
  CTextureParameters *ref();
  bool operator == (CTextureParameters &Sourse) const;
  CTextureParameters(TEX_TYPE TType=TEX_TYPE_2D);
  int IsUseMipMapGeneration()const;
  int IsUseAlpha           ()const;
  int IsUseMipmaps         ()const;
  int GetFilter            ()const;
  int GetAniso             ()const;
  int GetEnvMode           ()const;
  int GetWRAP_S            ()const;
  int GetWRAP_T            ()const;
  int GetWRAP_R            ()const;
  int GetWRAP_Q            ()const;
  int GetWRAP         (int I)const;

  CTextureParameters& SetUseMipMapGeneration(const DWORD Value);
  CTextureParameters& SetUseAlpha           (const DWORD Value);
  CTextureParameters& SetUseMipmaps         (const DWORD Value);
  CTextureParameters& SetFilter             (const DWORD Value);
  CTextureParameters& SetAniso              (const DWORD Value);
  CTextureParameters& SetEnvMode            (const DWORD Value);
  CTextureParameters& SetWRAP_S             (const DWORD Value);
  CTextureParameters& SetWRAP_T             (const DWORD Value);
  CTextureParameters& SetWRAP_R             (const DWORD Value);
  CTextureParameters& SetWRAP_Q             (const DWORD Value);
  CTextureParameters& SetWRAP         (int I,const DWORD Value);
};

#define tDLLTYPE DLLTYPE




	DWORD     tDLLTYPE LoadTexture     (LPCSTR Name,CTextureParameters *Params=NULL);
	DWORD     tDLLTYPE ReplaceTexture  (CDWORD ID,LPCSTR Name);
	bool      tDLLTYPE UnloadTexture   (CDWORD ID);
	bool      tDLLTYPE BindTexture     (CDWORD ID,int TMU=-1);
	void      tDLLTYPE DisableTexture  (int TMU=-1,int Safe=1);
	bool      tDLLTYPE SetActiveTMU    (CDWORD TMU);
	TMU_STATE tDLLTYPE GetTMUStatus    (CDWORD TMU=-1);
	DWORD     tDLLTYPE GetTextureAtTMU (CDWORD TMU);
	bool      tDLLTYPE DisableTexturing(TMU_OFF_METHOD MODE=TEX_DISABLE_ALL);
	DWORD     tDLLTYPE GetInerTexture  (CDWORD ID);
	bool      tDLLTYPE IsValidTexture  (CDWORD ID);

	DWORD     tDLLTYPE GenerateTexture (CDWORD TType,DWORD FORMAT,CTextureParameters *Params,cint Width,cint Height=-1,cint Depth=-1);
		    
	DWORD     tDLLTYPE SetTextureData  (CDWORD ID,LPCVOID Data,CDWORD TYPE,int MipMap=-1);

	GString   tDLLTYPE GetTextureStat  (DWORD MODE);
    bool      tDLLTYPE HaveAlpha       (CDWORD ID);
};
#endif