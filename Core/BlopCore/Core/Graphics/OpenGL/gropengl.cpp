/*===========================================================================
    Заголовок: grOpenGL.cpp
    Автор: Kashey
    Информация: OpenGLRender

===========================================================================*/

#include "ProjectSpace.h"
#include "ProjectCounter.h"
#include "..\GenericGraphic.h"
#include "ProjectGraphic.h"
#include "grOpenGl.h"
#include "coregraphics.h"
//#include "../Shaders/cgShader.h"


bool COpenGL::core_IN_GL_READY=false;
bool COpenGL::Iscore_IN_GL_READY(){return core_IN_GL_READY;}



//#pragma comment (lib,"opengl32.lib")

#include "..\..\FX\Particles.h"

DWORD deftex_noscene;

COpenGLModes::COpenGLModes()
{
	ZeroMemory(MatrixDepth,sizeof(MatrixDepth));;
	CurMatrix=0;
}

//void COpenGLModes::PushMatrix(){++MatrixDepth[CurMatrix];}
//void COpenGLModes::PopMatrix (){++MatrixDepth[CurMatrix];}

BINLINE GLenum decodeCullingMode(BYTE Mode)
{
	switch(Mode)
	{
	case CULL_FRONT: return GL_FRONT;
	case CULL_BACK : return GL_BACK;
	case CULL_NONE :
	default        : return GL_FRONT_AND_BACK;
	}
}

BINLINE GLenum decodeDepthMode(BYTE Mode)
{
	switch(Mode)
	{
     case DSF_DEPTH_LESS      :	return GL_LESS;
     case DSF_DEPTH_LEQUAL    :	return GL_LEQUAL;
     case DSF_DEPTH_EQUAL     :	return GL_EQUAL;
     case DSF_DEPTH_GEQUAL    :	return GL_GEQUAL;
     case DSF_DEPTH_GREATER   :	return GL_GREATER;
	 default:_assume(0);
	}
	return GL_LEQUAL;
}


COpenGL::COpenGL(BlopBaseObject *_Driver):CGenericGraphic(_Driver)
{
//	SetName("GLGraphic");
	CurrentTick=0;
	Print2Console(SYS_GRAPHIC,"Using OpenGL");
	Driver=this;
}

COpenGL::~COpenGL(void)
{
}

bool  COpenGL::Start()
{
	 if(!Init()) return false;
	 return true;
}

void  COpenGL::Stop ()
{
}

DWORD sys_glCycle=0;

void inglCheckError(LPCSTR f,long l)
{
    static MakeSectionProfile(pr1,"glCheckError");
    Profile(pr1);
	int err=glGetError();
	if(err)
	{
		GString Err="";
		switch(err)
		{
			case GL_NO_ERROR         : Err="GL_NO_ERROR";         break;
			case GL_INVALID_ENUM     : Err="GL_INVALID_ENUM";     break;
			case GL_INVALID_VALUE    : Err="GL_INVALID_VALUE";    break;
			case GL_INVALID_OPERATION: Err="GL_INVALID_OPERATION";break;
			case GL_STACK_OVERFLOW   : Err="GL_STACK_OVERFLOW";   break;
			case GL_STACK_UNDERFLOW  : Err="GL_STACK_UNDERFLOW";  break;
			case GL_OUT_OF_MEMORY    : Err="GL_OUT_OF_MEMORY";    break;
		};
		Error2Console(SYS_GRAPHIC,"glerror %s(%X) in %s(%d) in %Xc",Err.c_str(),err,f,l,sys_glCycle);
	}
}

DWORD  COpenGL::Tick (float Time)
{
	static MakeProfile(OpenGLPro,"OpenGL");

	Profile(OpenGLPro);
	DetectCycle(DC_FPS);
	Swarp();
	Clear();
	Do   ();
	glFlush ();
	{
	 static MakeProfile(OpenGLParticlesCalc,"Move particles");
	 Profile(OpenGLParticlesCalc);
	 PARTICLES::Prepare();
	}
//	PrintProfiler();
	return 1;
}

bool gl_firsttick=true;

bool COpenGL::Init()
{
    Print2Console(SYS_GRAPHIC,"Starting GL graphics...");
	DWORD Width =lua::getdword("win_width");//ThisWindow->GetParamAsDWORD("width");
	DWORD Height=lua::getdword("win_height");;//=ThisWindow->GetParamAsDWORD("height");
	DC     =(HDC)lua::getdword("win_dc");//ThisWindow->GetParamAsDWORD("DC");
    ASSERT("NO DC SET" && DC);
	DWORD BPP   =lua::getdword("win_BPP");
	DWORD DPP   =lua::getdword("win_DPP");

	PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize      = sizeof( pfd );
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW |  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = (BYTE)BPP;
	pfd.cAlphaBits = 8;
    pfd.cDepthBits = (BYTE)DPP;
    pfd.iLayerType = PFD_MAIN_PLANE;
    bool ret=true;
	Print2Console(SYS_GRAPHIC,"SetUpMode:\nW  :%4d;H    :%4d;\nBPP:%4d;Depth:%4d;",Width,Height,BPP,DPP);
    SleepEx(0,true);
    int i=0;
    for(i=0;i<2;++i)
	 if((iFormat = ChoosePixelFormat( DC, &pfd )))break;
    if(!iFormat)
    {
	  Print2Console(SYS_GRAPHIC,"Unable to ChoosePixelFormat(%d),reason %d\n",(DWORD)DC,GetLastError());
      return false;
    }
	if(!SetPixelFormat( DC, iFormat, &pfd ))
    {
      Print2Console(SYS_GRAPHIC,"Unable to SetPixelFormat,reason %d\n",GetLastError());
      return false;
    }
    GLRC =(DWORD) ::wglCreateContext(DC);
    ::wglMakeCurrent( DC, (HGLRC)GLRC );
	//extgl_Initialize();
    glewInit();


    glClearColor(0,0,0,0);
	glViewport(0,0,Width,Height);


	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1, 0.8, 1000);
    glDepthRange(0,1);
    glTranslatef(0,0,0);
    glMatrixMode (GL_MODELVIEW);
    glTranslatef(0,0,-20);

	
 
   Print2Console(SYS_GRAPHIC,"VENDOR  :%s",glGetString(GL_VENDOR));
   Print2Console(SYS_GRAPHIC,"RENDERER:%s",glGetString(GL_RENDERER));
   Print2Console(SYS_GRAPHIC,"VERSION :%s",glGetString(GL_VERSION));
   Print2Console(SYS_GRAPHIC,"EXT:%s\n",glGetString(GL_EXTENSIONS));
	int SafeSwapInterval = 0;
    core_IN_GL_READY=true;
//	SafeSwapInterval = wglGetSwapIntervalEXT();
	//wglSwapIntervalEXT(0);
    CGenericGraphic::Init(); 
	glCheckError;
	shaderManager.LoadShaders();
	glCheckError;
    deftex_noscene=Textures::LoadTexture("noscene.tga");
    //Textures::LoadTexture("avi/d3.avi:256");///
	glCheckError;
	SetZMode   (DSF_DEPTH_LEQUAL);
	ForseClear(0xFF);
	glCheckError;
	gl_firsttick=true;
	lua::setdword("render.SceneClear",1);

	//init
	EnableWrite(LAYER_DEPTH);
	EnableWrite(LAYER_COLOR);



	return true;
}

bool COpenGL::IsSupported(DWORD ID)
{
	return true;
}

void  COpenGL::Swarp()
{
	static MakeProfile(OpenGLProSwap,"...Swap...");
	Profile(OpenGLProSwap);
	glCheckError;
	//if(!gl_firsttick)glPopMatrix();
	gl_firsttick=false;
	{
		static MakeProfile(pr1,"Lua update");
		Profile(pr1);
        static bool CompiledStatsFunc=false;
        if(!CompiledStatsFunc)
         {
           lua::dostring("function __inRenCompiledStatsFunc(a,b,c)\n"
                       "r_depthcleared=0;\n"
                       "render.trisdrawn=a;\n"
                       "render.indsdrawn=b;\n"
                       "render.deltacashe=c;\n"
                       "end\n"
                       //"render.CompiledStatsFunc=CompiledStatsFunc;\n"
                       //"CompiledStatsFunc=nil;"
                       );
           CompiledStatsFunc=true;
         }


        lua::setdword("r_depthcleared",0);
        lua::setdword("r_trisdrawn"   ,NumTriDrawn);
        lua::setdword("r_indsdrawn"   ,NumIndsDrawn);
        lua::setdword("r_deltacashe"  ,VDeltaCashe);
        lua::setdword("r_batches"     ,NumBatches);
        //lua::doVstring("__inRenCompiledStatsFunc(%d,%d,%d)",
        //               NumTriDrawn,NumIndsDrawn,VDeltaCashe);
        NumTriDrawn=NumIndsDrawn=VDeltaCashe=0;
        NumBatches=0;
    }
	{
		static MakeProfile(pr1,"Before Callbacks");
		Profile(pr1);
        for(VgrRightBeforeSwapIntrStruct::iterator it=RightBeforeSwapV.begin();it!=RightBeforeSwapV.end();++it)
         it->Call();
	}
    glCheckError;
	{
		static MakeProfile(pr1,"Dirrect swap");
		Profile(pr1);
	    SwapBuffers((HDC)DC);
    }

	{
		static MakeProfile(pr1,"After Callbacks");
		Profile(pr1);
        for(VgrRightBeforeSwapIntrStruct::iterator it=RightAfterSwapV.begin();it!=RightAfterSwapV.end();++it)
         it->Call();
	}

	
	glCheckError;
	sys_glCycle++;

}
void  COpenGL::Clear()
{
	DWORD Mode=lua::getdword("render.clearstyle");
	ForseClear(Mode);
	glCheckError;
}

void COpenGL::ForseClear(DWORD Mode)
{
    static MakeProfile(pr1,"ForseClear");
    Profile(pr1);
	DWORD Mask=0;
    if((Mode&RENDER_CLEAR_DEPTH  )==RENDER_CLEAR_DEPTH  )
	{
		Mask|=GL_DEPTH_BUFFER_BIT;
		lua::setdword("r_depthcleared",1);
	}
	if((Mode&RENDER_CLEAR_COLOR  )==RENDER_CLEAR_COLOR  ) Mask|=GL_COLOR_BUFFER_BIT;
	if((Mode&RENDER_CLEAR_STENCIL)==RENDER_CLEAR_STENCIL) Mask|=GL_STENCIL_BUFFER_BIT;

    if(Modes.ZOn==0)
    {glEnable(GL_DEPTH_TEST);
	 glDepthFunc(GL_LEQUAL);
    }

    if(Modes.ZWrite==0)
	 glDepthMask(GL_TRUE);

	glClear(Mask);
    if(Modes.ZOn==0)
     {
      glDisable(GL_DEPTH_TEST);
      glDepthFunc(decodeDepthMode(Modes.ZMode));
     }
    if(Modes.ZWrite==0)
	 glDepthMask(GL_FALSE);

}


DWORD LastTime =0;

void  COpenGL::Do   ()
{
	static MakeProfile(OpenGLProDo,"render");
	Profile(OpenGLProDo);
	RENDER->GetOverlay()->CommitChanges();
	lua::setdword("r_winrestart",0);// hack
	glCheckError;
	Camera->Init();
	Blending.Disable();
	//3d
	Camera->Select3D();
	{
	 static MakeProfile(OpenGLVA,"Objects");
	 Profile(OpenGLVA);
	 DoVAObjects();
	}
	
	{
     static MakeProfile(OpenGLParticles,"Particles");
	 Profile(OpenGLParticles);
     DoParticles();
	}

	Camera->Select2D();
	{
	 static MakeProfile(OpenGLOverlay,"Overlay");
	 Profile(OpenGLOverlay);


	 Overlay->Render();
	 Overlay->UpdateSkiping();
	 glCheckError;
	}
}


void COpenGL::DoVAObjects()
{
   bool SceneIn=false;
   if(RootNode.Draw(NULL)>0)SceneIn=true;
   glCheckError;
   lua::setdword("render.SceneClear",SceneIn?0:1);
   //QuadT(deftex_noscene,0,0,640,480,v4ONE,1,-1);
}

void COpenGL::DoParticles()
{
    static MakeProfile(OpenGLParticlesDraw,"render");   
	Profile(OpenGLParticlesDraw);
	PARTICLES::DO();
	glCheckError;
}

void COpenGL::PushMatrix(){glPushMatrix();Modes.PushMatrix();}
void COpenGL::PopMatrix (){glPopMatrix();Modes.PopMatrix();}

void COpenGL::ApplyTransform(Draw::CTransformer &TR)
{
	switch(TR.WorkMode)
	{
	case 1:glTranslatef(TR.Translate[0],TR.Translate[1],TR.Translate[2]);
		   glRotatef   (TR.Rotation [0],TR.Rotation [1],TR.Rotation [2],TR.Rotation[3]);
		   glScalef    (TR.Scale    [0],TR.Scale    [1],TR.Scale    [2]);
		   break;
	case 2:glMatrixMode( GL_MODELVIEW );
           glMultMatrixf( TR.Translate );
		   glScalef    (TR.ScaleEx  [0],TR.ScaleEx  [1],TR.ScaleEx  [2]);
		   break;
    default:_assume(0);
	}
	glCheckError;
}

bool COpenGL::CallShader(Draw::BShader &Shader)
{
	if(!Shader.IsDrawable())return false;
	CullFace  (Shader.GetCullingMode());
	SetZMode  (Shader.GetZTestMode());
	SetDrawing(Shader.IsColorWrite(),
		       Shader.IsDepthWrite());
	glCheckError;
	//Draw::rCMaterial Mat=Draw::GetMaterialByID(Shader.Material);
	//Textures::BindTexture(Mat.Texture[0]);
	return true;
}	

inline DWORD decodepixelformat(BYTE M)
{
	switch(M)
	{
	case grDEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
    case grRGBA_COMPONENT : return GL_RGBA ;
	default:_assume(0);
	}
}

bool COpenGL::ReadPixel(const float x,const float y,const float W,const float H,BYTE MODE,DWORD &out)
{
	glReadPixels(x,y,W,H,decodepixelformat(MODE),GL_UNSIGNED_BYTE,&out);
	glCheckError;
	return true;
}

bool COpenGL::ReadPixel(const float x,const float y,const float W,const float H,BYTE MODE,DWORD *out)
{
	glReadPixels(x,y,W,H,decodepixelformat(MODE),GL_UNSIGNED_BYTE,out);
	glCheckError;
	return true;
}

bool COpenGL::ReadPixelf(const float x,const float y,const float W,const float H,BYTE MODE,float &out)
{
	glReadPixels(x,y,W,H,decodepixelformat(MODE),GL_FLOAT,&out);
	glCheckError;
	return true;
}

bool COpenGL::ReadPixelf(const float x,const float y,const float W,const float H,BYTE MODE,float *out)
{
	glCheckError;
	glReadPixels(x,y,W,H,decodepixelformat(MODE),GL_FLOAT,out);
	glCheckError;
	return true;
}

void COpenGL::SetBlending(BlendingTypes MODE)
{
	Blending.Apply(MODE);
	glCheckError;
}

Draw::cgShaderMng* COpenGL::Shaders(){return &shaderManager;}



void COpenGL::CullFace(BYTE Mode)
{
	if(Mode!=Modes.CullingMode)
	{
	 if(Mode==CULL_NONE)
		  glDisable(GL_CULL_FACE);
	 else
	 {
	  if(Modes.CullingMode==CULL_NONE)
		  glEnable(GL_CULL_FACE);
	  glCullFace(decodeCullingMode(Mode));
	 }
	 Modes.CullingMode=Mode;
	}
	glCheckError;
}
void COpenGL::SetZMode  (BYTE Mode)
{
  	if(Mode!=Modes.ZMode || (Modes.ZOn==0 && Mode!=DSF_DEPTH_OFF))
	{
	 if(Mode==DSF_DEPTH_OFF)
		 {glDisable(GL_DEPTH_TEST);Modes.ZOn=0;return;}
	 if(Mode==DSF_DEPTH_ON)
		 {glEnable(GL_DEPTH_TEST);Modes.ZOn=1;return;}
	 if(!Modes.ZOn){glEnable(GL_DEPTH_TEST);Modes.ZOn=1;}
	 glDepthFunc(decodeDepthMode(Mode));
	 Modes.ZMode=Mode;
	}
	glCheckError;
}
void COpenGL::SetDrawing(BYTE CMode,BYTE DMode)
{
	if(Modes.ColorWrite!=CMode && CMode!=LAYER_KEEP)
	{
	 glColorMask((CMode & CSF_COLOR_WRITE_R)?GL_TRUE:GL_FALSE,
		         (CMode & CSF_COLOR_WRITE_G)?GL_TRUE:GL_FALSE,
				 (CMode & CSF_COLOR_WRITE_B)?GL_TRUE:GL_FALSE,
				 (CMode & CSF_COLOR_WRITE_A)?GL_TRUE:GL_FALSE);
	 Modes.ColorWrite=CMode;
	}
	if(Modes.ZWrite!=DMode && DMode!=LAYER_KEEP)
	{
	 glDepthMask(DMode?GL_TRUE:GL_FALSE);
	 Modes.ZWrite=DMode;
	}
	glCheckError;
}

void COpenGL::EnableWrite(BYTE LAYER)
{
	BYTE CMode=Modes.ColorWrite;
	BYTE DMode=Modes.ZWrite;
	if((LAYER&LAYER_DEPTH)==LAYER_DEPTH)DMode=1;
	if((LAYER&LAYER_COLOR)==LAYER_COLOR)
	{
		CMode=CSF_COLOR_WRITE_R|CSF_COLOR_WRITE_G|CSF_COLOR_WRITE_B|CSF_COLOR_WRITE_A;
	}
    SetDrawing(CMode,DMode);
	glCheckError;
}

void COpenGL::DisableWrite(BYTE LAYER)
{
	BYTE CMode=Modes.ColorWrite;
	BYTE DMode=Modes.ZWrite;
	if((LAYER&LAYER_DEPTH)==LAYER_DEPTH)DMode=0;
	if((LAYER&LAYER_COLOR)==LAYER_COLOR)CMode=0;
    SetDrawing(CMode,DMode);
	glCheckError;
}

void COpenGL::SetMaterial(const DWORD M)
{
}

void COpenGL::SetTexture (const DWORD T)
{
	glCheckError;
	Textures::BindTexture(T);
	glCheckError;
}

inline DWORD DecodeIndexFormat(const DWORD F)
{
	switch(F)
	{
     case Draw::RI_TRIS      :return GL_TRIANGLES;
	 case Draw::RI_TRIStrips :return GL_TRIANGLE_STRIP;
	 case Draw::RI_QUADS     :return GL_QUADS;
	 case Draw::RI_QUADStrips:return GL_QUAD_STRIP;
	 case Draw::RI_POINTS    :return GL_POINTS;
	 case Draw::RI_LINES     :return GL_LINES;
	 case Draw::RI_LINEStrip :return GL_LINE_STRIP;
	 default:_assume(0);
	}
}

inline DWORD DecodeIndexType(const DWORD T)
{
	switch(T)
	{
		case Draw::RI_WORD:return GL_UNSIGNED_SHORT;
		case Draw::RI_DWORD:return GL_UNSIGNED_INT;
		default:_assume(0);
	}
}

void COpenGL::FlushRV(const Draw::CRenderVertex *RV)
{
	static MakeSectionProfile(cfr,"COpenGL::FlushRV");
	Profile(cfr);
	Draw::cgShader *Shader=Draw::GetCurrentVertexShader(true);
	if(!Shader) return;
	if(RV->GetNumStreams()>0)
	 Shader->BindRD(*RV);
	glCheckError;
	if(RV->HaveIndex())
	{
		Draw::CBaseRenderIndex &Ind=RV->GetIndex();
        DWORD GetFormat,GetStart,GetEnd,GetLen,GetType;
		LPCVOID GetOffset;
		Ind.Bind();
        GetFormat=DecodeIndexFormat(Ind.GetFormat()) ;
		GetStart =Ind.GetStart();
		GetEnd   =Ind.GetEnd();
		GetLen   =Ind.GetLen();
		GetType  =DecodeIndexType(Ind.GetType());
		GetOffset=Ind.BufferOffset(RV->GetIndexOffset());
		glDrawRangeElements(GetFormat,
							GetStart,
							GetEnd,
							GetLen,
							GetType,
							GetOffset);
        NumTriDrawn +=Ind.GetLen()/3;
        NumIndsDrawn+=Ind.GetLen();
        ++NumBatches;
		glCheckError;
	}
	if(!SessionDepth)
		Shader->ClearShader();
		                
}

void COpenGL::FlushIV(const Draw::CRenderVertex *RV)
{
	static MakeSectionProfile(cfr,"COpenGL::FlushIV");
	Profile(cfr);
	Draw::cgShader *Shader=Draw::GetCurrentVertexShader(true);
	glCheckError;
	if(!Shader) return;
	glCheckError;
	if(RV->HaveIndex())
	{
//		static MakeSectionProfile(cfi,"COpenGL::FlushRV::index");
//	    Profile(cfi);
//		glCheckError;
		Draw::CBaseRenderIndex &Ind=RV->GetIndex();
		Ind.Bind();
//		glCheckError;
		glDrawRangeElements(DecodeIndexFormat(Ind.GetFormat()),
							Ind.GetStart(),
							Ind.GetEnd(),
							Ind.GetLen(),
							DecodeIndexType(Ind.GetType()),
							Ind.BufferOffset(RV->GetIndexOffset()));
        NumTriDrawn +=Ind.GetLen()/3;
        NumIndsDrawn+=Ind.GetLen();
        ++NumBatches;
		glCheckError;
	}
	if(!SessionDepth)
		Shader->ClearShader();
		                
}


Draw::cgShader *COpenGL::GetCurrentVertexShader(){return Draw::GetCurrentVertexShader(true);}
Draw::cgShader *COpenGL::GetUsedVertexShader(){return Draw::GetCurrentVertexShader(true);}

bool COpenGL::EnableVSShader(LPCSTR SName)
{
    static MakeSectionProfile(pr1,"grShader::Enable");
    Profile(pr1);
	Draw::cgShader *Shader=shaderManager.GetShader(SName);
	if(Shader && Shader->IsVS())return Shader->Enable();
	return false;
}

bool COpenGL::DisableVSShader(bool Forsed)
{
    static MakeSectionProfile(pr1,"grShader::Disable");
    Profile(pr1);
	Draw::cgShader *Shader=Draw::GetCurrentVertexShader(true);
	if(Shader)Shader->Disable();
	return true;
}

void COpenGL::SessionCleanUp()
{
	Draw::cgShader *Shader=Draw::GetCurrentVertexShader(true);
	if(!Shader) return;
	Shader->ClearShader();
}

	//IM
bool COpenGL::SetIMColor(const bVector4f &Color)
{
    static bVector3f OldColor;
    if(memcmp(&Color,&OldColor,4*4))
    {
	 glColor4fv(&Color.x);
     memcpy(OldColor,&Color,4*4);
     return true;
    }
   return false;
}


void COpenGL::RegisterCallBack_RightBeforeSwap(grRightBeforeSwapIntrStruct Str)
{
  RightBeforeSwapV.push_back(Str);
}

void COpenGL::RegisterCallBack_RightAfterSwap(grRightBeforeSwapIntrStruct Str)
{
  RightAfterSwapV.push_back(Str);
}