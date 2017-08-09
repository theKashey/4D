#pragma once

#define MAX_G_TEXTURES 16

#define SF_NODRAW                           0x01  
#define SF_NOCOLLIDE                        0x01

#define CSF_COLOR_WRITE_R                   0x01<<1
#define CSF_COLOR_WRITE_G                   0x01<<2
#define CSF_COLOR_WRITE_B                   0x01<<3
#define CSF_COLOR_WRITE_A                   0x01<<4
#define CSF_COLOR_WRITE                     CSF_COLOR_WRITE_R|CSF_COLOR_WRITE_G|CSF_COLOR_WRITE_B|CSF_COLOR_WRITE_A


#define DSF_DEPTH_OFF                       0x00
#define DSF_DEPTH_ON                        0x01
#define DSF_DEPTH_LESS                      0x01<<1
#define DSF_DEPTH_LEQUAL                    0x02<<1
#define DSF_DEPTH_EQUAL                     0x03<<1
#define DSF_DEPTH_GEQUAL                    0x04<<1
#define DSF_DEPTH_GREATER                   0x05<<1

#define CULL_FRONT		    				(BYTE)1
#define CULL_BACK							(BYTE)2
#define CULL_NONE							(BYTE)0

#define LAYER_COLOR	                        (BYTE)1
#define LAYER_DEPTH							(BYTE)2
#define LAYER_STENCIL						(BYTE)4
#define LAYER_KEEP   						(BYTE)256

#define grDEPTH_COMPONENT                   (BYTE)1
#define grCOLOR_COMPONENT                   (BYTE)2
#define grRGBA_COMPONENT                   (BYTE)2
                                            
#define bgDEPTH_TEST_ON					    0x250
#define bgDEPTH_TEST_OFF					0x251




struct CMaterial
{
	DWORD   Textures[MAX_G_TEXTURES];
	BYTE	AlphaMode;
	BYTE    BlendMode;
	BYTE    NumTextures;
	bool IsDrawable(){ return NumTextures>0;}
	DWORD Get(const DWORD ID){ ASSERT(ID<NumTextures);return Textures[ID];}
};
typedef CMaterial &rCMaterial; 
typedef CMaterial *pCMaterial; 

DWORD      DLLTYPE GetMaterial        (LPCSTR Name);
rCMaterial DLLTYPE GetMaterialByID    (DWORD ID);
pCMaterial DLLTYPE pGetMaterialByID   (DWORD ID);

struct BShader
{
	DWORD               MaterialParams;
	DWORD               inDiffuseMaterial;
	BINLINE rCMaterial  DiffuseMaterial(){ return GetMaterialByID(inDiffuseMaterial);}
	DWORD               NormalMap;
	DWORD               NormalGlossMap;
	DWORD               SpecularMap;
	DWORD               HorisonMap;
	DWORD               HeightMap;
	float               HeightBias,HeightScale;

	BYTE                CullMode;
	BYTE                ZMode   ;
	BYTE                ColorMode;
	BINLINE bool        IsDrawable    () {	return (MaterialParams & SF_NODRAW)==0;}
	BINLINE BYTE        GetCullingMode() {	return CullMode;}
	BINLINE BYTE        IsColorWrite  () {  return ColorMode &   CSF_COLOR_WRITE ;}
	BINLINE BYTE        IsDepthWrite  () {  return ZMode     &   DSF_DEPTH_ON ;}
	BINLINE BYTE        GetZTestMode  () {  return ZMode     & (!DSF_DEPTH_ON);}
	BINLINE bool        HasNormalMap  () {  return Textures::IsValidTexture(NormalMap);}
	BShader();
   ~BShader();
};
typedef BShader &rBShader; 
typedef BShader *pBShader; 

DWORD      DLLTYPE GetShader          (LPCSTR Name);
rBShader   DLLTYPE GetShaderByID      (DWORD ID);
pBShader   DLLTYPE pGetShaderByID      (DWORD ID);
bool       DLLTYPE CallShader         (DWORD ID);

DWORD      DLLTYPE GetCurrentShader   ();
DWORD      DLLTYPE GetCurrentMaterial ();


