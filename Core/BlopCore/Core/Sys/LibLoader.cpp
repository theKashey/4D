#include "ProjectSpace.h"

//#define bLINK


#pragma comment (lib,"OpenGL32.Lib")
#pragma comment (lib,"GLu32.Lib")

#ifndef bLINK
#	pragma comment (lib,"liblua.lib")
#	pragma comment (lib,"liblualib.lib") 

//#pragma comment (lib,"cg.lib")
//#pragma comment (lib,"cggl.lib")

#pragma comment (lib,"Winmm.lib")


#ifdef _DEBUG
#pragma comment (lib,"StaticModules/Textures.lib")
#pragma comment (lib,"StaticModules/Parser.lib")
//#pragma comment (lib,"StaticModules/NvTriStrip.lib")
#pragma comment (lib,"StaticModules/Loaders.lib")
#pragma comment (lib,"StaticModules/luamodule.lib")
#else
#pragma comment (lib,"RelStaticModules/Textures.lib")
#pragma comment (lib,"RelStaticModules/Parser.lib")
//#pragma comment (lib,"StaticModules/NvTriStrip.lib")
#pragma comment (lib,"RelStaticModules/Loaders.lib")
#pragma comment (lib,"RelStaticModules/luamodule.lib")
#endif
//#pragma comment (lib,"gui.lib")

#pragma comment (lib,"Core/Graphics/Opengl/glew/glew32s.lib")


#pragma comment (lib,"Ws2_32.lib")



#endif
			   