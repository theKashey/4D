#include "stdafx.h"
#include "parserplugin.h"
#include "Particles/ParticleShader.h"

CParserPlugIn::CParserPlugIn(void):BaseObject(REFC_CORE,REFB_UTIL,"Parser")
{
}

CParserPlugIn::~CParserPlugIn(void)
{
}

extern void	ShadersInit();
extern void	ShadersPass();
extern void	LoadSoundShaders();

bool CParserPlugIn::Start()
{
	ShadersInit();
	ShadersPass();
	LoadSoundShaders();
	InitParticleSybsetShaders();
	return true;
}

/*PLoadObjectList DLLTYPE GetStoredClasses()
{
    ONCE_PLUGIN 
    MAKE_PLUGIN (new CParserPlugIn,"PARSER",0,true);
    END_PLUGIN
}*/

PLoadObjectList GetParserStoredClasses()
{
    ONCE_PLUGIN 
    MAKE_PLUGIN (new CParserPlugIn,"PARSER",0,true);
    END_PLUGIN
}
