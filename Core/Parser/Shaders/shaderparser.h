#pragma once

#include "../Parser.h"
#include "ProjectDividors.h"

class DLLTYPE CShaderParser : public CParser
{
	typedef CBSPDividorLookUpArray<char,int> _MYBSPDIVIDOR;
	void    ShaderError(LPCSTR Error,...);
	void    ParseError (LPCSTR Error,...);
	void    ShaderNotImp();
	void    ConstructBSP();
public:
	char *	GetFirstShader(void);
	char *	GetNextShader (void);
	void	SkipShader    (void);

	CShaderParser(int N=0);
   ~CShaderParser();
	///

//private:
    // Methods
	int     Depth;
	static  _MYBSPDIVIDOR  BSPFor;
	static  bool BSPFuncInited;

	void	ParseShader (void);
	void	ParseLayer  (void);
	void    CloseLayer  (void);
	float	ParseFunc   (void);

	bool	GetName     (void);
	bool	GetCommand  (void);
	bool	GetParameter(void);

	void	InvalidToken(void);
	void	SkipLine    (void);
	void	IgnoreLine  (void);

	// general shader commands
	void ShaderCommand_skyParms             (void);
	void ShaderCommand_cull                 (void);
	void ShaderCommand_terrainfog           (void);
	void ShaderCommand_deformVertexes       (void);
	void ShaderCommand_fogParams            (void);
	void ShaderCommand_terrainfogparams     (void);
	void ShaderCommand_nopicmip             (void);
	void ShaderCommand_nomipmaps            (void);
	void ShaderCommand_polygonOffset        (void);
	void ShaderCommand_portal               (void);
	void ShaderCommand_sort                 (void);

	// map processing tools shader commands
	void ShaderCommand_tessSize             (void);
	void ShaderCommand_q3map_backshader     (void);
	void ShaderCommand_q3map_globaltexture  (void);
	void ShaderCommand_q3map_sun            (void);
	void ShaderCommand_q3map_surfaceLight   (void);
	void ShaderCommand_q3map_lightImage     (void);
	void ShaderCommand_q3map_lightSubdivide (void);
	void ShaderCommand_surfaceParm          (void);

	// qeradiant shadercommands
	void ShaderCommand_qer_editorimage      (void);
	void ShaderCommand_qer_nocarve          (void);
	void ShaderCommand_qer_trans            (void);

	// undocumented shader commands
	void ShaderCommand_backsided            (void);
	void ShaderCommand_cloudParms           (void);
	void ShaderCommand_entityMergable       (void);
	void ShaderCommand_fogGen               (void);
	void ShaderCommand_fogOnly              (void);
	void ShaderCommand_light                (void);
	void ShaderCommand_lightning            (void);
	void ShaderCommand_q3map_backsplash     (void);
	void ShaderCommand_q3map_flare          (void);
	///////////////// rfd ///////////////////
	void ShaderCommand_sky                  (void);
	void ShaderCommand_terrain              (void);
	void ShaderCommand_stenciled_flat_mirror(void);
	void ShaderCommand_surfaceColor         (void);
	void ShaderCommand_nobright             (void);
	void ShaderCommand_spriteParam          (void);
	void ShaderCommand_include_effect       (void);
	////////////////////

	// layer commands
	void LayerCommand_map                   (void);
	void LayerCommand_clampMap              (void);
	void LayerCommand_animMap               (void);
	void LayerCommand_blendFunc             (void);
	void LayerCommand_rgbGen                (void);
	void LayerCommand_alphaGen              (void);
	void LayerCommand_tcGen                 (void);
	void LayerCommand_tcMod                 (void);
	void LayerCommand_depthFunc             (void);
	void LayerCommand_depthMask             (void);
	void LayerCommand_detail                (void);
	void LayerCommand_alphaFunc             (void);

	// rfd ////////////
	/////////////////////

	// undocumented layer commands
	void LayerCommand_alphaMap              (void);

	// members:
	DWORD	shaderFile;
	int		errors;
};


// Transform functions
enum FUNC{
	FUNC_CONST,
	FUNC_SIN,
    FUNC_TRIANGLE,
    FUNC_SQUARE,
    FUNC_SAWTOOTH,
    FUNC_INVERSESAWTOOTH,
	FUNC_NOISE
};