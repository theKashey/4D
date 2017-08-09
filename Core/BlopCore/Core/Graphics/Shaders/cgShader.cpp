/*===========================================================================
	Заголовок: cgShader.cpp
    Автор: Kashey
	Информация: framework для работы с сг

	// CG_PROFILE_ARBFP1 - GL_ARB_fragment_program
	// CG_PROFILE_FP30   - GL_NV_fragment_program
	// CG_PROFILE_FP20   - NV_texture_shader & NV_register_combiners
	//

 	// CG_PROFILE_ARBVP1 - GL_ARB_vertex_program
	// CG_PROFILE_VP30   - GL_NV_vertex_program2
	// CG_PROFILE_VP20   - GL_NV_vertex_program


===========================================================================*/
#include "projectspace.h"
#include "projectgraphic.h"
#include "cgShader.h"
#include "../Parser/Parser.h"
#include "coregraphics.h"
#include "../Cg/include/Cg/cgGL.h"
//#include "Cg/include/Cg/cgGL.h"

#define CGEXCAP "/*CG EX"


#ifndef USECG
int DUMPCG(...){return 0;}
 #define cgGetErrorString DUMPCG
 #define cgGLIsProfileSupported DUMPCG
 #define cgGLSetOptimalOptions DUMPCG
 #define cgDestroyProgram DUMPCG
 #define cgDestroyContext DUMPCG
 #define cgSetErrorCallback DUMPCG
 #define cgCreateContext DUMPCG
 #define cgCreateProgram DUMPCG
 #define cgGLLoadProgram DUMPCG
 #define cgGetNamedParameter DUMPCG
 #define cgGLSetParameter4f DUMPCG
 #define cgGLSetParameterArray1f DUMPCG
 #define cgGLSetParameterPointer DUMPCG
 #define cgGLSetParameter2f DUMPCG
 #define cgGLSetParameter3f DUMPCG
 #define cgGLSetParameter1f DUMPCG
 #define cgGLSetParameterArray4f DUMPCG
 #define cgGLSetTextureParameter DUMPCG
 #define cgGLEnableTextureParameter DUMPCG
 #define cgGLSetParameter3fv DUMPCG
 #define cgGLSetParameter4fv DUMPCG
 #define cgGLDisableTextureParameter DUMPCG
 #define cgGLEnableProfile DUMPCG
 #define cgGLBindProgram DUMPCG
 #define cgGLDisableProfile DUMPCG
 #define cgGetArraySize DUMPCG
 #define cgGetError (CGerror)0
#endif


using namespace Draw;


DWORD WasCGError=0;

CCurrentTextureSet CurrentTextureSet;
PCurrentTextureSet GetCurrentTextureSet()
{
	return &CurrentTextureSet;
}




cgShader *ActiveVertexShader=NULL;
cgShader *FPLVertexShader   =NULL;
cgShader *ActivePixelShader =NULL;

static void cgErrorCallback()
{
    CGerror err = cgGetError();
    if (err != CG_NO_ERROR) 
	{
		GString SName;
		if(ActiveVertexShader!=NULL){ SName=ActiveVertexShader->GetName();}
		if(ActivePixelShader!=NULL){ SName+=" ";SName+=ActivePixelShader->GetName();}
        Error2Console(SYS_GRAPHIC,"*** cgErrorCallback(%s): %s",SName.c_str(),cgGetErrorString(err));
		WasCGError=1;
//		if(TSystem::tvars.s_fatalcgerror->ivalue)
//        FatalExit(cgGetErrorString(err));
    }
}



cgShader *Draw::GetCurrentVertexShader(bool AllowFPL)
{
	if(!ActiveVertexShader && AllowFPL)
		return FPLVertexShader;
	else
	return ActiveVertexShader;
}
cgShader *Draw::GetCurrentPixelShader (){ return ActivePixelShader;}


void cgBaseInstrument::SetDriver(cgShader *D)
{
  Driver=D;
}

cgBaseInstrument::cgBaseInstrument()
{
  Driver=NULL;
  Instrumented=false;
}

#define cgb_NONE               0
#define cgb_BIND_VERTEX        1
#define cgb_BIND_COLOR         2
#define cgb_BIND_COLOR2        3
#define cgb_BIND_NORMAL        4
//#define 
#define cgb_BIND_TEXTURE0      10
#define cgb_BIND_TEXTURE1      11
#define cgb_BIND_TEXTURE2      12
#define cgb_BIND_TEXTURE3      13
#define cgb_BIND_TEXTURE4      14
#define cgb_BIND_TEXTURE5      15
#define cgb_BIND_TEXTURE6      16
#define cgb_BIND_TEXTURE7      17
#define cgb_BIND_TEXTURE8      18
#define cgb_BIND_TEXTURE9      19
#define cgb_BIND_TEXTURE10     20
#define cgb_BIND_TEXTURE11     21
#define cgb_BIND_TEXTURE12     22
#define cgb_BIND_TEXTURE13     23
#define cgb_BIND_TEXTURE14     24
#define cgb_BIND_TEXTURE15     25
#define cgb_BIND_TEXTURE16     26


#define inbind(a,b,c) _inbind(a,c,b,S)


#define cgb_NEED_DIFFUSE           1	// должен быть дифуз
#define cgb_NEED_NORMALMAP		   2	// нормал бамп
#define cgb_NEED_HEIGHTMAP		   4	// карта высот
#define cgb_NEED_GLOSSTMAP		   8	//   глосс
#define cgb_NEED_SPECULARMAP	   16	// спекуляр
#define cgb_NEED_FILTERMAP		   32	//   филтр каб мап
#define cgb_NEED_SHADOW2DMAP	   64	// 2Д тень
#define cgb_NEED_SHADOWCUBEMAP	   128	//   КУБ тень
#define cgb_NEED_NONFLATMAP		   256	//   бамп не есть плоский(для скорости)
#define cgb_NEED_HORIZONT		   512	//   карта самозатения


#define cgb_NULL	               0
#define cgb_BUMP				   1
#define cgb_HEIGHT				   2
#define cgb_HORIZONT			   3
#define cgb_GLOSS				   4
#define cgb_SPECULAR			   5
#define cgb_SHADOW2D			   6
#define cgb_SHADOWCUBE			   7
#define cgb_ATT1				   8
#define cgb_ATT2				   9
#define cgb_ATT3				   10
#define cgb_ATT1Q				   11
#define cgb_NORMALMAP			   12
#define cgb_FILTERMAPCUBE		   13
#define cgb_REFLECTIONMAPCUBE	   14
#define cgb_DIFFUSE1			   15
#define cgb_DIFFUSE2			   16
#define cgb_DIFFUSE3			   17
#define cgb_DIFFUSE4			   18
#define cgb_DIFFUSE5			   19
#define cgb_DIFFUSE6			   20
#define cgb_DIFFUSE7			   21
#define cgb_DIFFUSE8			   22
#define cgb_DIFFUSE9			   23
#define cgb_UNKNOWN 			   24

bool ShaderBSPInit=false;
//BSP синтаксическое дерево
typedef CBSPDividorLookUpArray<char,int> _SHADERIDBSP;
_SHADERIDBSP  ShaderBSP,ShaderBSP2;

#define SDR_NAME         1
#define SDR_TYPE		 2
#define SDR_AUTHOR		 3
#define SDR_VER			 4
#define SDR_SPEED		 5
#define SDR_GPU			 6
#define SDR_QUALITY		 7
#define SDR_VERTEXBIND	 8
#define SDR_TEXBIND		 9
#define SDR_NOPOSINV	 10
#define SDR_CLASS  		 100
#define SDR_INVAR        101
#define SDR_INFUNC       102

#define SDR_END  		 11

#define SDR_TYPE_PIXEL   20  
#define SDR_TYPE_VERTEX	 21

#define SDRb_VERTEX		 40
#define SDRb_VERTEX2	 41
#define SDRb_COLOR		 42
#define SDRb_COLOR2		 43
#define SDRb_NORMAL		 44
#define SDRb_TAR1		 45
#define SDRb_TAR2		 46
#define SDRb_TAR3		 47
#define SDRb_SBONES      48
#define SDRb_SWEIGHT     49
#define SDRb_SBWEIGHT	 50
#define SDRb_TEXTURE0    60

#define SDRsl_LIGHT  		 10001
#define SDRsl_MODEL  		 10002
#define SDRsl_SUN    		 10003

void InitShaderBSP()
{
  GStringBase Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();ShaderBSP.PushAs(b,Base.v_str(),(DWORD)strlen(a))
  ADDBSP(SDR_NAME              ,"name"			   );
  ADDBSP(SDR_TYPE              ,"type"			   );
  ADDBSP(SDR_AUTHOR            ,"author"		   );
  ADDBSP(SDR_VER               ,"ver"			   );
  ADDBSP(SDR_SPEED             ,"speed"			   );
  ADDBSP(SDR_GPU               ,"gpu"			   );
  ADDBSP(SDR_QUALITY           ,"quality"		   );
  ADDBSP(SDR_VERTEXBIND        ,"vertexbind"	   );
  ADDBSP(SDR_TEXBIND           ,"texbind"	       );
  ADDBSP(SDR_CLASS             ,"class"	           );
  ADDBSP(SDR_NOPOSINV          ,"noposinv"	       );  
  ADDBSP(SDR_INVAR             ,"invar"	           );  
  ADDBSP(SDR_INFUNC            ,"infunc"	       );  
  ADDBSP(SDR_END               ,"end*/"			   );

  ADDBSP(SDR_TYPE_PIXEL        ,"PS"	           );
  ADDBSP(SDR_TYPE_VERTEX       ,"VS"		       );

  ADDBSP(SDRsl_LIGHT           ,"cLIGHT"	       );
  ADDBSP(SDRsl_MODEL           ,"cMODEL"		   );
  ADDBSP(SDRsl_SUN             ,"cSUN"	           );


  ADDBSP(SDRb_VERTEX           ,"vertex"		   );
  ADDBSP(SDRb_VERTEX2          ,"vertex2"		   );
  ADDBSP(SDRb_COLOR            ,"color"			   );
  ADDBSP(SDRb_COLOR2           ,"color2"		   );
  ADDBSP(SDRb_NORMAL           ,"normal"	       );
  ADDBSP(SDRb_TAR1             ,"tar1"	           );
  ADDBSP(SDRb_TAR2             ,"tar2"			   );
  ADDBSP(SDRb_TAR3             ,"tar3"			   );
  ADDBSP(SDRb_SBONES           ,"bones"            );
  ADDBSP(SDRb_SWEIGHT          ,"weights"           );
  ADDBSP(SDRb_SBWEIGHT         ,"boneweights"       );
  GString Tex;                               
  int i=0;
  for(i=0;i<16;++i)
  {
    Tex.Format("texture%d",i);
	LPCSTR cTex=Tex;
    ADDBSP(SDRb_TEXTURE0+i           ,cTex	   );
  }


#undef ADDBSP
#define ADDBSP(b,a) Base=a;Base.MakeLower();ShaderBSP2.PushAs(b+100,Base.v_str(),(DWORD)strlen(a))
  ADDBSP(cgb_NONE-100          ,"none"			   );
  ADDBSP(cgb_BIND_VERTEX       ,"vertex"		   );
  ADDBSP(cgb_BIND_COLOR        ,"color"		       );
  ADDBSP(cgb_BIND_COLOR2       ,"color2"		   );
  ADDBSP(cgb_BIND_NORMAL       ,"normal"		   );

  /*ADDBSP(SDRb_SBONES           ,"bones"            );
  ADDBSP(SDRb_SWEIGHT          ,"weight"           );
  ADDBSP(SDRb_SBWEIGHT         ,"boneweight"       );
  */

  for(i=0;i<16;++i)
  {
    Tex.Format("tex%d",i);
	LPCSTR cTex=Tex;
    ADDBSP(cgb_BIND_TEXTURE0+i           ,cTex	   );
  }
#undef ADDBSP

#define ADDBSP(b,a) Base=a;Base.MakeLower();ShaderBSP2.PushAs(b+200,Base.v_str(),(DWORD)strlen(a))
  ADDBSP(cgb_BUMP          ,"bump"		   );
  ADDBSP(cgb_HEIGHT        ,"height"	   );
  ADDBSP(cgb_HORIZONT      ,"horizont"	   );
  ADDBSP(cgb_GLOSS         ,"gloss"		   );
  ADDBSP(cgb_SPECULAR      ,"specular"	   );
  ADDBSP(cgb_SHADOW2D      ,"shadow2d"	   );
  ADDBSP(cgb_SHADOWCUBE    ,"shadowCUBE"   );
  ADDBSP(cgb_ATT1          ,"att1"		   );
  ADDBSP(cgb_ATT2          ,"att2"		   );
  ADDBSP(cgb_ATT3          ,"att3"		   );
  ADDBSP(cgb_ATT1Q         ,"att1q"		   );
  ADDBSP(cgb_NORMALMAP         ,"normalC"  );
  ADDBSP(cgb_FILTERMAPCUBE     ,"filter"   );
  ADDBSP(cgb_REFLECTIONMAPCUBE ,"reflection"  );
  for(i=0;i<16;++i)
  {
    Tex.Format("diff%d",i);
	LPCSTR cTex=Tex;
    ADDBSP(cgb_DIFFUSE1+i           ,cTex	   );
  }
#undef ADDBSP
}


static DWORD st_EnabledStates=0;
static DWORD st_IncomingStates=0;

BINLINE bool NeedEnabledState(const BYTE ST,const BYTE On=1)
{
	const DWORD OFF=1<<ST;
#define MAKEOFF //{}
	switch(On)
	{
		//start new session
	case 0:st_IncomingStates=0;return true;
		//need to enable new state
	case 1:MAKEOFF
		   st_IncomingStates|=OFF;
		   if(st_EnabledStates & OFF)return false;
		   else { st_EnabledStates|=OFF;return true;}
	   //need disable state
	case 2:MAKEOFF
		   if(st_EnabledStates & OFF && !(st_IncomingStates & OFF))
		   { DWORD NOFF=~OFF;st_EnabledStates&=NOFF;return true;}
		   else return false;
	   //need clean up state
	case 3:MAKEOFF
		  if(st_IncomingStates & OFF)return false;
		  else if(st_EnabledStates & OFF){st_EnabledStates&=~OFF;return true;}
		  else return false;
       //states ret
	case 4:MAKEOFF;return (st_EnabledStates & OFF)!=0;
	case 5:MAKEOFF;return (st_IncomingStates & OFF)!=0;
     //need to clean?
    case 10:return st_EnabledStates==st_IncomingStates; 
	default:_assume(0);
	};
#undef MAKEOFF
}

#define enableState NeedEnabledState

BINLINE void inbindclear()
{
	NeedEnabledState(0,0);
}

void  streamCheck(DWORD Type,bool SIsOn)
{
	DWORD _TON=0;
#define TON(a) _TON=a;break;
	switch(Type)
	{
	  case cgb_NONE          :   return; 
	  case cgb_BIND_VERTEX   :   TON(GL_VERTEX_ARRAY);
	  case cgb_BIND_COLOR    :   TON(GL_COLOR_ARRAY);
	  case cgb_BIND_COLOR2   :   TON(GL_SECONDARY_COLOR_ARRAY_EXT);
	  case cgb_BIND_NORMAL   :   TON(GL_NORMAL_ARRAY);
	};
	if(Type>=cgb_BIND_TEXTURE0 )
	{
	  if(Type-cgb_BIND_TEXTURE0<4)
	  {
		  glClientActiveTextureARB(GL_TEXTURE0_ARB+(Type-cgb_BIND_TEXTURE0));
          _TON=GL_TEXTURE_COORD_ARRAY;
	  }
	}
	if(_TON==0)return;
  bool VIsOn=glIsEnabled(_TON)>0;
  if(!SIsOn &&  VIsOn){ ASSERT("stream is still on "&& FALSE);}
  if( SIsOn && !VIsOn){ ASSERT("stream is still off"&& FALSE);}
  glCheckError;
}

static void checkRVSteamsState()
{
  for(int i=0;i<cgb_BIND_TEXTURE0+16;++i)
	  streamCheck(i,enableState(i,5));
}

BINLINE void _fastcall  DCS(DWORD DS)
{
  static MakeSectionProfile(pr1,"Shader::DisableClientState");
  Profile(pr1);
  glDisableClientState(DS);
}

BINLINE void _fastcall  _offbind(const DWORD Type,const int t=0)
{
#define glDisableClientState DCS
  switch(Type)
	{
	  case cgb_NONE          :  return; 
	  case cgb_BIND_VERTEX   :  //if(enableState(Type,t))
                                 glDisableClientState(GL_VERTEX_ARRAY); 		 
                                return;  
	  case cgb_BIND_COLOR    :  //if(enableState(Type,t))
                                 glDisableClientState(GL_COLOR_ARRAY);  		 
                                return;  
	  case cgb_BIND_COLOR2   :  //if(enableState(Type,t))
                                 glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT); 		 
                                 return;  
	  case cgb_BIND_NORMAL   :  //if(enableState(Type,t))
                                 glDisableClientState(GL_NORMAL_ARRAY); 	
                                return;  
	};
	if(Type>=cgb_BIND_TEXTURE0 )
	{
	  //if(enableState(Type,t))
	  {
	   glClientActiveTextureARB(GL_TEXTURE0_ARB+(Type-cgb_BIND_TEXTURE0));
       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	  }
	}
	//glCheckError;
#undef glDisableClientState
}

BINLINE void inbindoff()
{
  static MakeSectionProfile(pr1,"Shader::inbindoff");
  if(enableState(0,10))return;
  Profile(pr1);
	for(int i=0;i<cgb_BIND_TEXTURE0+16;++i)
    {
     if(enableState(i,2))
	   _offbind(i,2);
    }
}


static DWORD AfterBindCount=0;
static DWORD AfterBindStride=0;
static DWORD AfterBindEnter=0;

void PreAfterBind()
{
 AfterBindCount =
 AfterBindStride=
 AfterBindEnter =0;
}

void AfterBind()
{
 if(AfterBindCount)
  {
   glVertexPointer(AfterBindCount,GL_FLOAT,AfterBindStride,(GLvoid*)AfterBindEnter);
  }
}

void _fastcall  _inbind(DWORD Type,DWORD Enter,DWORD Count,const Draw::CRenderStream &S)
{
	if(!Count)return;
	switch(Type)
	{
	  case cgb_NONE          :   return; 
	  case cgb_BIND_VERTEX   :   
		  if(enableState(Type))glEnableClientState(GL_VERTEX_ARRAY); 
		  AfterBindCount =Count;
		  AfterBindStride=S.STRIDE;
		  AfterBindEnter =Enter;
		  return;  
	  case cgb_BIND_COLOR    : if(!Enter)return;   
		  if(enableState(Type))glEnableClientState(GL_COLOR_ARRAY); 
		  glColorPointer(Count,GL_FLOAT,S.STRIDE,(GLvoid*)Enter);
		  return;  
	  case cgb_BIND_COLOR2   : if(!Enter)return;   
		  if(enableState(Type))glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT); 
		  glSecondaryColorPointerEXT(Count,GL_FLOAT,S.STRIDE,(GLvoid*)Enter);
		  return;  
	  case cgb_BIND_NORMAL   : if(!Enter)return;   
		  if(enableState(Type))glEnableClientState(GL_NORMAL_ARRAY); 
		  glNormalPointer(GL_FLOAT,S.STRIDE,(GLvoid*)Enter);
		  return;  
	  //default:ASSERT(FALSE);_assume(0);

	};
	//#define 
	if(Type>=cgb_BIND_TEXTURE0 )
	{
	  if(!Enter)return; 
	  glClientActiveTextureARB(GL_TEXTURE0_ARB+(Type-cgb_BIND_TEXTURE0));
      if(enableState(Type))glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  glTexCoordPointer(Count,GL_FLOAT,S.STRIDE,(GLvoid*)Enter);
	}
	glCheckError;
}

CGprofile  BestVertexProfile,BestPixelProfile;
bool   BestVertexSet=0;
bool   BestPixelSet =0;

CGprofile FindBestProfile(DWORD Type)
{
	if(Type==CG_VERTEX_TYPE)
	{
	  if(BestVertexSet)
		  return BestVertexProfile;

	  for(int i=0;i<4;++i)
	  {
		  switch(i)
		  {
		    case 0:BestVertexProfile = CG_PROFILE_VP40;  break;	  
            case 1:BestVertexProfile = CG_PROFILE_ARBVP1;break;
		    case 2:BestVertexProfile = CG_PROFILE_VP30;  break;
		    case 3:BestVertexProfile = CG_PROFILE_VP20;  break;
		  }
		 if(cgGLIsProfileSupported(BestVertexProfile))
		 {
			 const char *PName;
			 switch(i)
			 {
			  case 0:PName="VP40";break;
			  case 1:PName="ARBVP1";break;
			  case 2:PName="VP30";break;
			  case 3:PName="VP20";break;
			 }
			 Print2Console(SYS_GRAPHIC,"cgEX::FindBest<Vertex>Profile - %s\n",PName);
  	 	     BestVertexSet=1;
			 cgGLSetOptimalOptions(BestVertexProfile);
             return BestVertexProfile;
		 }
	  }
	  //FinalExit("Your hardware dost not support vertex shaders\nThis is fatal\nApplication will exit"); 
	}
	else
	{
	  if(BestPixelSet)
		  return BestPixelProfile;

	  for(int i=0;i<4;++i)
	  {
		  switch(i)
		  {
		    case 0:BestPixelProfile = CG_PROFILE_FP40;  break;
            case 1:BestPixelProfile = CG_PROFILE_FP30;break;
		    case 2:BestPixelProfile = CG_PROFILE_ARBFP1;  break;
		    case 3:BestPixelProfile = CG_PROFILE_FP20;  break;
		  }
		 if(cgGLIsProfileSupported(BestPixelProfile))
		 {
			 const char *PName;
			 switch(i)
			 {
			  case 0:PName="FP40";break;
			  case 1:PName="FP30";break;
			  case 2:PName="ARBFP1";break;
			  case 3:PName="FP20";break;
			 }
			 Print2Console(SYS_GRAPHIC,"cgEX::FindBest<Pixel>Profile - %s\n",PName);
  	 	     BestPixelSet=1;
			 cgGLSetOptimalOptions(BestPixelProfile);
             return BestPixelProfile;
		 }
	  }
	  Error2Console(SYS_GRAPHIC,"Your hardware dost not support pixel shaders\n");
	}
	return  CG_PROFILE_UNKNOWN;
}



cgShader::cgShader()
{
	Disabled=true;
	RenderTarget.Vertex=cgb_BIND_VERTEX;
	RenderTarget.Tex[0]=cgb_BIND_TEXTURE0;
	RenderTarget.basisx=cgb_BIND_COLOR;
	RenderTarget.basisy=cgb_BIND_NORMAL;
	RenderTarget.basisz=cgb_BIND_COLOR2;

	ShaderFlag=0;
	ZeroMemory(TexBind,sizeof(TexBind));
	Type=CG_UNDEF_TYPE;
	fIsARBProfile=0;
	fIsFPL       =0;
    LetBeMyID=0;
}

cgShader::~cgShader()
{
	if(RENDER->GRCollapsing)return;
	if(!IsValid())return;
	cgDestroyProgram((CGprogram)program);
    cgDestroyContext((CGcontext)context);

}

bool RenderTargetEmpty=true;
bool TexTargetEmpty=true;


bool AllowPosInv=true;

cgShader* cgShader::LoadBasePath(LPCSTR name,bool printMessages)
{
	AllowPosInv=true;
	BaseShader=NULL;
	Type=CG_UNDEF_TYPE;
    GString v=name;//=BASE_PATH;
	if(printMessages)Print2Console(SYS_GRAPHIC,"--- Loading shader: %s\n",name);

	DWORD F=bOpenFile("Shaders/"+v,"r",true);
	if(!F) return false;
	LPCSTR cData=(LPCSTR)bGetFileData(F);
	LPCSTR fData=cData;
	DWORD  cLen =bFileSize   (F);
	ZeroMemory(&RenderTarget,sizeof(RenderTarget));
	if(strncmp(cData,CGEXCAP,strlen(CGEXCAP))==0)
	{
		//this is ex shader
		fData=ParseShader(cData+strlen(CGEXCAP),cLen);
	}
	else
	{
		Error2Console(SYS_GRAPHIC,"shader %s is not CG EX shader",v.c_str());
		return false;
	}
	if(!fData)
	{
		Error2Console(SYS_GRAPHIC,"shader %s have error in CG EX section(may be not closed?)",v.c_str());
		return false;
	}

	cgSetErrorCallback(cgErrorCallback);
    if(!BaseShader) BaseShader=new cgShader;
	if(!BaseShader)
	{
		Error2Console(SYS_GRAPHIC,"Base shader for %s if undef",v.c_str());
		return NULL;
	}
	BaseShader->CapName=CapName;
	BaseShader->Type   =Type;
	memcpy(BaseShader->TexBind,TexBind,sizeof(TexBind));
	BaseShader->RenderTarget=RenderTarget;
	BaseShader->profile = FindBestProfile(Type);

	// "-fastprecision"
	// "-posinv"

	char *args[2];
	char arg1[128];
	arg1[1]=0;
	char arg2[2];
	arg2[0]=0;
	if(BaseShader->profile== CG_PROFILE_VP40  || BaseShader->profile==CG_PROFILE_ARBVP1)BaseShader->fIsARBProfile=1;
	if(BaseShader->profile== CG_PROFILE_FP40  || BaseShader->profile==CG_PROFILE_ARBFP1)BaseShader->fIsARBProfile=2;

	ZeroMemory(arg1,sizeof(arg1));
	if(BaseShader->IsARBProfile() && AllowPosInv)strcpy_s(arg1,128,"-posinv");
	else
	if(BaseShader->IsARBProfile())strcat_s(arg1,128," -profileopts MaxLocalParams=240");


	
	args[0]=arg1;
	args[1]=NULL;
	BaseShader->Disabled=true;
	if((RenderTargetEmpty && BaseShader->Type==CG_VERTEX_TYPE)||
	   (   TexTargetEmpty && BaseShader->Type==CG_PIXEL_TYPE))

	{ Print2Console(SYS_GRAPHIC,"%s threaded as FPL shader",v.c_str());
	  if(BaseShader->Type==CG_VERTEX_TYPE )
	  {
		  BaseShader->RenderTarget.Vertex =cgb_BIND_VERTEX;
		  BaseShader->RenderTarget.Color  =cgb_BIND_COLOR;
		  BaseShader->RenderTarget.Color2 =cgb_BIND_COLOR2;
		  BaseShader->RenderTarget.Normal =cgb_BIND_NORMAL;
		  for(int i=0;i<16;++i)BaseShader->RenderTarget.Tex[i]=cgb_BIND_TEXTURE0+i;
		  FPLVertexShader=BaseShader;
	  }
	  BaseShader->fIsFPL=true;  
	}
	else
	{
	BaseShader->context = (DWORD)cgCreateContext();
	BaseShader->program = (DWORD)cgCreateProgram( (CGcontext)BaseShader->context,
					                        CG_SOURCE,
					                        fData,
					                        (CGprofile)BaseShader->profile,
					                        NULL,
					                        (const char**)args );
	if(!BaseShader->program) 
	{
		Error2Console(SYS_GRAPHIC,"*** cgShaders: unable to create %s",name);
		bCloseFile(F);
		return 0;
	}
//	cgSetAutoCompile( (CGcontext)BaseShader->context, CG_COMPILE_MANUAL);
	cgGLLoadProgram( (CGprogram)BaseShader->program );
//	cgGLSetOptimalOptions((CGprofile)BaseShader->profile);
	//cgCompileProgram((CGprogram)BaseShader->program);
	/**/
	BaseShader->Disabled=false;
    }
    BaseShader->filename=v;

	bCloseFile(F);
    cgp_modelViewProj=0;
    if(!BaseShader->Disabled)
        BaseShader->InstrumentInit();
//	if(!BaseShader->Disabled)
//	  BaseShader->Instrument();
	return BaseShader;
}

cgShader* cgShader::Retouch()
{
	return this->LoadBasePath(filename,true);
}

bool cgShader::AddInstrument(cgBaseInstrument *Instr)
{
 if(!Instr)return false;
 DWORD Code=Instr->GetCode();
 for(InstrumentsVector::iterator it=Instruments.begin();it!=Instruments.end();++it)
     if((*it)->GetCode()==Code){ delete Instr;return false;}
 Instr->SetDriver(this);
 Instruments.push_back(Instr);
 return true;
}

cgBaseInstrument *cgShader::Instrument(DWORD ID)
{
 for(InstrumentsVector::iterator it=Instruments.begin();it!=Instruments.end();++it)
     if((*it)->GetCode()==ID)return *it;
 return false;
}

void cgShader::InstrumentInit()
{
    if(cgp_modelViewProj==0)
        cgp_modelViewProj=SetNamedParameter ("modelViewProj");

    for(InstrumentsVector::iterator it=Instruments.begin();it!=Instruments.end();++it) 
        if(*it)
            if(!(*it)->Instrument())
            {
             delete (*it);
             *it=NULL;
            }
}


cgParam *cgShader::GetParam(const DWORD ID)
{
	if(ID<(DWORD)(params.size()+1))return params.addr(ID-1);
	else return NULL;
}

LPCSTR cgShader::ParseShader(LPCSTR In,int Len)
{
	if(!ShaderBSPInit)
		InitShaderBSP();
	CParser Parser;
    Parser.StartParseBuffer(In,Len);
	GString Token,Token2;
	RenderTargetEmpty=TexTargetEmpty=true;

	BaseShader=new	cgShader();

	while( Parser.GetToken(true) )
	{
		Token=Parser.token;
		Token.MakeLower();
		DWORD ShaderToken=ShaderBSP.GetIDFor(Token.c_str(),(DWORD)Token.length());
		switch(ShaderToken)
		{
          case SDR_NAME          :  CapName=Parser.GetTokenToLineEnd(); break;
		  case SDR_NOPOSINV      :  AllowPosInv=false;break;
          case SDR_TYPE		     :	
			  Token2=Parser.GetParameter();Token2.MakeLower();
			  {
				  Type=ShaderBSP.GetIDFor(Token2.c_str(),(DWORD)Token2.length());
				  switch(Type)
				  {
				  case   SDR_TYPE_PIXEL :Type=CG_PIXEL_TYPE;break;
				  case   SDR_TYPE_VERTEX:Type=CG_VERTEX_TYPE;break;
				  }
			  }
			  break;
          case SDR_AUTHOR		 :	 break;
          case SDR_VER			 :	 break;
          case SDR_SPEED		 :	 break;
          case SDR_GPU			 :	 break;
          case SDR_QUALITY		 :	 break;
          case SDR_VERTEXBIND	 :
			  if(Parser.GetParameter()!="{"){ ShaderError(" need { at vertex bind"); return 0;}
			  while(Parser.GetToken(true) )
			  {
				  if(strcmp("}",Parser.token)==0)
					  break;
				  Token2=Parser.token;
				  Token2.MakeLower();
				  DWORD bID=ShaderBSP.GetIDFor(Token2.c_str(),(DWORD)Token2.length());
				  Token2=Parser.GetParameter();
				  Token2.MakeLower();
				  DWORD vID=ShaderBSP2.GetIDFor(Token2.c_str(),(DWORD)Token2.length());
				  if(vID) vID-=100;
				  switch(bID)
				  {
				  case SDRb_VERTEX  : RenderTarget.Vertex =vID;break;
				  case SDRb_VERTEX2 : RenderTarget.Vertex2=vID;break;
				  case SDRb_COLOR   : RenderTarget.Color  =vID;break;
				  case SDRb_COLOR2  : RenderTarget.Color2 =vID;break;   
				  case SDRb_NORMAL  : RenderTarget.Normal =vID;break;
				  case SDRb_TAR1    : RenderTarget.basisx =vID;break;
				  case SDRb_TAR2    : RenderTarget.basisy =vID;break;
				  case SDRb_TAR3    : RenderTarget.basisz =vID;break;

				  case SDRb_SBONES  : RenderTarget.bones   =vID;break;
				  case SDRb_SWEIGHT : RenderTarget.weights =vID;break;
				  case SDRb_SBWEIGHT: RenderTarget.bweigth =vID;break;

				  default:
					  if(bID>=SDRb_TEXTURE0)
						  RenderTarget.Tex[bID-SDRb_TEXTURE0]=vID;
				  }
				  RenderTargetEmpty=false;
			  }
			  break;

          case SDR_TEXBIND		 :	 
			  if(Parser.GetParameter()!="{"){ ShaderError(" need { at vertex bind"); return 0;}
			   while(Parser.GetToken(true) )
			  {
				  if(strcmp("}",Parser.token)==0) break;
				  Token2=Parser.token;
				  Token2.MakeLower();
				  DWORD bID=atoi(Token2);
				  Token2=Parser.GetParameter();
				  Token2.MakeLower();
				  DWORD vID=ShaderBSP2.GetIDFor(Token2.c_str(),(DWORD)Token2.length());
				  if(vID) vID-=200;
				  TexBind[bID]=vID;
				  TexTargetEmpty=false;
			  }
			  break;
		  case SDR_CLASS         :
			  Token2=Parser.GetParameter();Token2.MakeLower();
			  {
				  DWORD tType=ShaderBSP.GetIDFor(Token2.c_str(),(DWORD)Token2.length());
				 // if(BaseShader)delete BaseShader;
                  //BaseShader=NULL;
				  switch(tType)
				  {
//				  case   SDRsl_LIGHT :AddInstrument(); break;
				  case   SDRsl_MODEL :BaseShader->AddInstrument(new CModelShaderInstrument());break;
//				  case   SDRsl_SUN   :BaseShader=NULL;break;
				  }
                
			  }
			  break;
		  case SDR_INVAR        :
			  if(!BaseShader){ASSERT("NEED FIRST SET BASESHADER");break;}
			  BaseShader->InVars.push_back(CShaderLuaInVar(Parser.GetParameter(),Parser.GetParameter()));
			  break;
		  case SDR_INFUNC       :
			  if(!BaseShader){ASSERT("NEED FIRST SET BASESHADER");break;}
			  BaseShader->inFunc=Parser.GetParameter();
			  break;
          case SDR_END  		 :	 return Parser.EndOffset();
		}
	}
	if(!BaseShader)
		BaseShader=new	cgShader();
	return Parser.EndOffset();
}


void cgShader::IdentityTex(const int TMU)
{
	if(Disabled)return;
	if(TMU<16)
//	if(cgp_TMU[TMU])
	{
	//	SetStateMatrixParameter(cgp_TMU[TMU],CG_GL_TEXTURE_MATRIX,CG_GL_MATRIX_IDENTITY );
	}

}

void cgShader::LoadTex    (const int TMU)
{
	if(Disabled)return;
    if(TMU<16)
//	if(cgp_TMU[TMU])
	{
	//	SetStateMatrixParameter(cgp_TMU[TMU],CG_GL_TEXTURE_MATRIX,CG_GL_MATRIX_IDENTITY );
	}
}

int cgShader::GetParameter(LPCSTR n)
{
	GString name=n;
	cgParamPack::iterator it=params.begin();
	const 	cgParamPack::iterator endit=params.end(); 
	for(int i=0;it!=endit;++it,++i)
		if(name==it->name) return i;
	return -1;
}

int cgShader::SetNamedParameter(LPCSTR n)
{
    GString name=n;
	cgParamPack::iterator it=params.begin();
	const 	cgParamPack::iterator endit=params.end(); 
	for(int i=0;it!=endit;++it,++i)
		if(name==it->name) return i+1;

    cgParam param;
	param.name=name;
	param.body=(DWORD)cgGetNamedParameter((CGprogram)program, n);
	if(!param.body)return 0;
    params.push_back(param);
	return params.size();
}

/*void cgShader::SetStateMatrixParameter(LPCSTR param,CGGLenum matrix,
                                             CGGLenum transform)
{
	if(Disabled)return;
	int id=GetParameter(param);if(id<0)return;	
	cgGLSetStateMatrixParameter(GETBODY(params.ref(id).body),matrix, transform );
}	*/



cgShader& cgShader::SetParameter4f(LPCSTR param,float x,float y,float z,float w)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameter4f( GETBODY(params.ref(id).body),x,y,z,w );
	return *this;
}

// 1dimension float array
cgShader& cgShader::SetParameterArray1f(LPCSTR param,const long offset,
                                         const long nelements,
                                         const float *v)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameterArray1f(GETBODY(params.ref(id).body),offset,nelements,v);
	return *this;
}

cgShader& cgShader::SetParameterPointer(LPCSTR param,
								         const int fsize,
                                         const DWORD type,
                                         const int stride,
                                         void *pointer)
{
	if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameterPointer(GETBODY(params.ref(id).body),fsize, type, stride, pointer );
	return *this;
}

cgShader& cgShader::SetParameter2f(LPCSTR param,float x,float y)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameter2f( GETBODY(params.ref(id).body),x,y );
	return *this;
}

cgShader& cgShader::SetParameter3fv(LPCSTR param,const float *v)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameter3fv( GETBODY(params.ref(id).body),v);
	return *this;
}

cgShader& cgShader::SetParameter4fv(LPCSTR param,const float *v)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameter4fv( GETBODY(params.ref(id).body),v);
	return *this;
}

cgShader& cgShader::SetParameterf(LPCSTR param,float v)
{
    if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetParameter1f( GETBODY(params.ref(id).body),v);
	return *this;
}

/**/
//----------------------

/*void cgShader::SetStateMatrixParameter(const int param,CGGLenum matrix,
                                                       CGGLenum transform)
{
	if(Disabled)return;
	cgGLSetStateMatrixParameter(GETBODY(params.ref(param-1).body),matrix, transform );
}
*/

cgShader& cgShader::SetMatrixArrayf    (const int param,const DWORD Off,const DWORD Num,const float *matrix)
{
	if(Disabled)return *this;
	//cgGLSetMatrixParameterArrayfr(GETBODY(params.ref(param-1).body),Off,0,matrix);
	cgGLSetParameterArray4f(GETBODY(params.ref(param-1).body),Off,Num,matrix);
	return *this;
}


cgShader& cgShader::SetParameter4f(const int param,float x,const float y,const float z,const float w)
{
	if(Disabled)return *this;
	cgGLSetParameter4f( GETBODY(params.ref(param-1).body),x,y,z,w );
	return *this;
}

// 1dimension float array
cgShader& cgShader::SetParameterArray1f(int param,const long offset,
                                         const long nelements,
                                         const float *v)
{
	if(Disabled)return *this;
	cgGLSetParameterArray1f(GETBODY(params.ref(param-1).body),offset,nelements,v);
	return *this;
}

cgShader& cgShader::SetParameterPointer(int param,
								         const int fsize,
                                         const DWORD type,
                                         const int stride,
                                         void *pointer)
{
	if(Disabled)return *this;
	cgGLSetParameterPointer(GETBODY(params.ref(param-1).body),fsize, type, stride, pointer );
	return *this;
}

cgShader& cgShader::SetParameter2f(const int param,const float x,const float y)
{
	if(Disabled)return *this;
	cgGLSetParameter2f( GETBODY(params.ref(param-1).body),x,y );
	return *this;
}

cgShader& cgShader::SetParameter3fv(const int param,const float *v)
{
	if(Disabled)return *this;
	cgGLSetParameter3fv( GETBODY(params.ref(param-1).body),v);
	return *this;
}

cgShader& cgShader::SetParameter4fv(const int param,const float *v)
{
	if(Disabled)return *this;
	cgGLSetParameter4fv( GETBODY(params.ref(param-1).body),v);
	return *this;
}




cgShader& cgShader::SetParameterf(int param,const float v)
{
	if(Disabled)return *this;
	cgGLSetParameter1f( GETBODY(params.ref(param-1).body),v);
	return *this;
}

cgShader& cgShader::SetTextureParameter(LPCSTR param,unsigned int tex)
{
	if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLSetTextureParameter( GETBODY(params.ref(id).body), tex );
	return *this;
}

cgShader& cgShader::EnableTextureParameter(LPCSTR param)
{
	if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLEnableTextureParameter( GETBODY(params.ref(id).body) );
	return *this;
}

cgShader& cgShader::DisableTextureParameter(LPCSTR param)
{
	if(Disabled)return *this;
	int id=GetParameter(param);if(id<0)return *this;
	cgGLDisableTextureParameter( GETBODY(params.ref(id).body) );
	return *this;
}
/**/

cgShader& cgShader::SetTextureParameter(const int id,const unsigned int tex)
{
	if(Disabled)return *this;
	cgGLSetTextureParameter( GETBODY(params.ref(id).body), tex );
	return *this;
}

cgShader& cgShader::EnableTextureParameter(const int id)
{
	if(Disabled)return *this;
	cgGLEnableTextureParameter( GETBODY(params.ref(id).body) );
	return *this;
}

cgShader& cgShader::DisableTextureParameter(const int id)
{
	if(Disabled)return *this;
	cgGLDisableTextureParameter( GETBODY(params.ref(id).body) );
	return *this;
}

cgShader& cgShader::LoadMatrix (const int MID,const neM4 &Matrix)
{
	GLint numMat;
	glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&numMat);
	if(MID>numMat)return *this;
	if(IsARBProfile())
	{
		glMatrixMode(GL_MATRIX0_ARB+MID);
		glLoadMatrixf(&Matrix.M[0][0]);
		glMatrixMode(GL_MODELVIEW);
	}
	return *this;
}

cgShader& cgShader::LoadMatrix (const int MID,const float *Matrix)
{
	GLint numMat;
	glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&numMat);
	if(MID>numMat)return *this;
	if(IsARBProfile())
	{
		glMatrixMode(GL_MATRIX0_ARB+MID);
		glLoadMatrixf(Matrix);
		glMatrixMode(GL_MODELVIEW);
	}
	return *this;
}


//----------------------

bool cgShader::IsVS()
{
	return Type==CG_VERTEX_TYPE;
}

bool cgShader::IsValid()
{
	return !Disabled;
}

float sm_currentparametervector[4];

static int sm_decodeparamvector(lua_State *L) 
{
	  for(int i=0;i<4;++i)
	    sm_currentparametervector[i]= lua_tonumber(L, i+1);    
      return 0; 
}

DWORD    cgShader::ExecuteInVar()
{
    static MakeSectionProfile(pr1,"cgShader::ExecuteInVar");
    Profile(pr1);
    static DWORD _LetBeMyID=1;
    bool NeedInit=0;
    if(LetBeMyID==0)
    {
     LetBeMyID=_LetBeMyID++;
     NeedInit=1;
    }

	static bool sv_Ssm_decodeparamvector=false;
	if(!sv_Ssm_decodeparamvector)
      lua::luafunction("Ssm_decodeparamvector",sm_decodeparamvector);
	sv_Ssm_decodeparamvector=true;
    int i=0;
    static char ivBuf[32];
	for(CShaderLuaInVarVector::iterator it=InVars.begin();it!=InVars.end();++it,++i)
	{
		if(it->paramID==-1) continue;
		else
		if(it->paramID==0)
		{
			it->paramID=SetNamedParameter (it->shaderparam);
			if(!it->paramID)
			{
				it->paramID=-1;
				continue;
			}
		}
    	{
        if(NeedInit)
        {  // make little wrapper
          lua::doVstring("function Ssm_decodeparamvector_for%d_%d()\nSsm_decodeparamvector%s;\nend;\n",
          LetBeMyID,i,(LPCSTR)it->Command);
        }
        sprintf_s(ivBuf,sizeof(ivBuf),"%d_%d",LetBeMyID,i);
        static GString SStart="Ssm_decodeparamvector_for";
        lua::callfunction(SStart+ivBuf);
		//	lua::dostring("Ssm_decodeparamvector"+it->Command);
			SetParameter4fv(it->paramID,sm_currentparametervector);
		}
	}
	if(inFunc.length()>0)
	  lua::dostring(inFunc);
	return 0;
}

bool cgShader::Enable()
{
	if(Disabled)
	{
	   if(IsVS())
	   {
           
		   if(ActiveVertexShader)
		     ActiveVertexShader->Disable();
   		   if(IsFPL())
		     ActiveVertexShader=this;
		   else
		    ActiveVertexShader=NULL;
		   return ActiveVertexShader!=NULL;
	   }
	   else
	   {
           
		   if(ActivePixelShader)
             ActivePixelShader->Disable();
		   if(IsFPL())
		      ActivePixelShader=this;
		   else
			  ActivePixelShader=NULL;
		   return ActivePixelShader!=NULL;
	   }
	}
	if(IsVS())
	 {
	  if(ActiveVertexShader==this)return true;
	  ActiveVertexShader=this;
	 }
	else
	 {
	  if(ActivePixelShader==this)return true;
	  ActivePixelShader=this;
	 }
	WasCGError=0;
	cgGLEnableProfile( (CGprofile)profile );
	cgGLBindProgram  ( (CGprogram)program );
	if(WasCGError)
	{
		DoDisable();
		return false;
	}
	else
	{
		ExecuteInVar();
	}
	return true;
}

void cgShader::Disable()
{
	if(Disabled) return;
	if(Type==CG_VERTEX_TYPE)
	{
	  ASSERT(ActiveVertexShader==this);
	  ActiveVertexShader=NULL;
	}
	else
	{
      ASSERT(ActivePixelShader==this);
      ActivePixelShader=NULL;
	}
	cgGLDisableProfile( (CGprofile)profile );
}

void cgShader::DoDisable()
{
	Disabled=true;
	GString _Type;
	if(Type==CG_VERTEX_TYPE)_Type="Vertex";
	else                    _Type="Pixel";
	Print2Console(SYS_GRAPHIC,"%s shader %s was disabled\n",_Type.c_str(),GetName());
}


bool cgShader::BindRD(const Draw::CRenderVertex &RV)
{
	static MakeSectionProfile(bindrb1,"Shader::BindRD");
    static MakeSectionProfile(bindrb2,"Shader::BindRD:VBO");
    static MakeSectionProfile(bindrb3,"Shader::BindRD:BClear");
    static MakeSectionProfile(bindrb4,"Shader::BindRD:Pointers");
    static MakeSectionProfile(bindrb5,"Shader::BindRD:AClear");
    static MakeSectionProfile(bindrb6,"Shader::BindRD:End");
	Profile(bindrb1);
	PreAfterBind();
	//bind vertex
	for(DWORD i=0;i<RV.GetNumStreams();++i)
	{
		const Draw::CRenderStream &S=RV.GetStream(i);

         {
          Profile(bindrb2);
		if(S.VBOElem) S.VBOElem->Bind();//enable
		else       CVBOElement::ResetVBO(VBO_RESET_VB);        // disable
         }

		//bind vertex
		//unbind unused//
		//_offbind
         {
          Profile(bindrb3);
		inbindclear();
         }
         {
         Profile(bindrb4);
		inbind(RenderTarget.Vertex ,S.VCount,S.Vertex);
		inbind(RenderTarget.Vertex2,S.V2Count,S.Vertex2);
		inbind(RenderTarget.Color  ,S.CCount,S.Color);
		inbind(RenderTarget.basisx ,S.BXCount,S.basisx);
		inbind(RenderTarget.basisy ,S.BYCount,S.basisy);
		inbind(RenderTarget.basisz ,S.BZCount,S.basisz);

		inbind(RenderTarget.bones   ,S.SBCount,S.bones);
		inbind(RenderTarget.weights ,S.SWCount,S.weights);
		inbind(RenderTarget.bweigth ,S.SBWCount,S.bweigth);

		for(int j=S.NumTexs;j>=0;--j)
			inbind(RenderTarget.Tex[j],S.TCount[j],S.Tex[j]);
         }
         {
           Profile(bindrb5);
		   inbindoff();
		   //checkRVSteamsState();
         }
		

		//anyway
          {
           Profile(bindrb6);
		   glClientActiveTextureARB(GL_TEXTURE0_ARB);
          }
     }
    AfterBind();
	return true;
}

#define t1D 1
#define t2D 2
#define t3D	3
#define tCB	4

DWORD texCurType=0;
void texOn(int a)
{
	texCurType=0;
	switch(a)
	{
	 case t1D:texCurType=GL_TEXTURE_1D;break;
	 case t2D:texCurType=GL_TEXTURE_2D;break;
	 case t3D:texCurType=GL_TEXTURE_3D;break;
	 case tCB:texCurType=GL_TEXTURE_CUBE_MAP;break;
	}
	if(	texCurType)
		glEnable(texCurType);
}

void texOff(int a)
{
	texCurType=0;
	switch(a)
	{
	case t1D:texCurType=GL_TEXTURE_1D;break;
	case t2D:texCurType=GL_TEXTURE_2D;break;
	case t3D:texCurType=GL_TEXTURE_3D;break;
	case tCB:texCurType=GL_TEXTURE_CUBE_MAP;break;
	}
	if(	texCurType)
		glDisable(texCurType);
}

int texBind(DWORD a)
{
	if(!a || ((int)a)==-1) return -1;
	glBindTexture(texCurType,a);
	return 0;
}

bool cgShader::BindShader(BShader *Shader)
{
  if(!Shader || Shader->IsDrawable())return false;
 //diffuse map
  if((ShaderFlag&cgb_NEED_DIFFUSE)&& (Shader->DiffuseMaterial().IsDrawable()))return false;
 // bump normal map
  if((ShaderFlag & cgb_NEED_NORMALMAP) && (!Shader->HasNormalMap())) return false;
 // bump height map
  //if((ShaderFlag & cgb_NEED_HEIGHTMAP) && (!(curr_shader->flags&SF_HASNORMALMAP))) return false;
 // specular gloss map
  //if((ShaderFlag & cgb_NEED_GLOSSTMAP) && (!(curr_shader->flags&SF_HASNORMALMAP))) return false;
 // specular map ( alpha normal)
  //if((ShaderFlag & cgb_NEED_SPECULARMAP) && (!(curr_shader->flags&SF_HASNORMALMAP))) return false;
 // need filter cube map
  if((ShaderFlag & cgb_NEED_FILTERMAP) && (!CurrentTextureSet.FilterCubeMap)) return false;
 // shadow must be a 2d texture
  if((ShaderFlag & cgb_NEED_SHADOW2DMAP) && (!CurrentTextureSet.Shadow2D)) return false;
 // shadow must be a cube texture
  if((ShaderFlag & cgb_NEED_SHADOWCUBEMAP) && (!CurrentTextureSet.ShadowCube)) return false;
 // bump must be NON flat
 // just lot of materials have it. so we can do not read from texture
 // just use contant
 // it is a bit faster
 // we need this bit :)
  //if((ShaderFlag & cgb_NEED_NONFLATMAP) && (!(curr_shader->flags&SF_HASNORMALMAP))) return false;

  for(int i=NUM_CGP_TMU;i<=0;--i)
  {
	  if(TexBind[i])
		  glActiveTextureARB(GL_TEXTURE0_ARB+i);
#define e(a) texOn(a)
#define bindS(a) error=texBind(Shader->a);
#define bindD(a) error=texBind(Shader->DiffuseMaterial().Get(a));
#define bindC(a) error=texBind(CurrentTextureSet.a);
  int error=0;
  switch(TexBind[i])
  {
	case cgb_NULL	               :break;
	case cgb_BUMP				   :e(t2D);bindS(NormalMap);break;
	case cgb_HEIGHT				   :e(t2D);bindS(HeightMap);break;
	case cgb_HORIZONT			   :e(t2D);bindS(HorisonMap);break;
	case cgb_GLOSS				   :e(t2D);break;
	case cgb_SPECULAR			   :e(t2D);bindS(SpecularMap);break;
	case cgb_SHADOW2D			   :e(t2D);bindC(Shadow2D);break;
	case cgb_SHADOWCUBE			   :e(tCB);bindC(ShadowCube);break;
	case cgb_ATT1				   :e(t1D);bindC(Atten1D);break;
	case cgb_ATT2				   :e(t2D);bindC(Atten2D);break;
	case cgb_ATT3				   :e(t3D);bindC(Atten3D);break;
	case cgb_ATT1Q				   :e(t1D);bindC(Atten1DQ);break;
	case cgb_NORMALMAP			   :e(t2D);bindC(NormalCubeMap);break;
	case cgb_FILTERMAPCUBE		   :e(tCB);bindC(FilterCubeMap);break;
	case cgb_REFLECTIONMAPCUBE	   :e(tCB);bindC(ReflectionCube);break;
	case cgb_DIFFUSE1			   :e(t2D);bindD(0);break;
	case cgb_DIFFUSE2			   :e(t2D);bindD(1);break;
	case cgb_DIFFUSE3			   :e(t2D);bindD(2);break;
	case cgb_DIFFUSE4			   :e(t2D);bindD(3);break;
	/*case cgb_DIFFUSE5			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE6			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE7			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE8			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE9			   :e(t2D);bindD(layer[1]);break;
	case cgb_UNKNOWN 			   :e(t2D);break;
	/**/
	default:ShaderError("on %d TMU set %d class texture - undefined class",i,TexBind[i]);
		    glActiveTextureARB(GL_TEXTURE0_ARB);
		    return false;
  }
#undef e
#undef bindS
#undef bindC
#undef bindD
  if(error)
  {
	 ShaderError("on %d TMU set %d class texture - no texture in",i,TexBind[i]);
	 glActiveTextureARB(GL_TEXTURE0_ARB);
	 return false;
  }
  }
  glActiveTextureARB(GL_TEXTURE0_ARB);
  return true;
}

bool cgShader::ClearShader()
{
  for(int i=NUM_CGP_TMU;i<=0;--i)
  {
	  if(TexBind[i])
		  glActiveTextureARB(GL_TEXTURE0_ARB+i);
#define e(a) texOff(a)
#define bindS(a)
#define bindD(a) 
#define bindC(a)  
  switch(TexBind[i])
  {
	case cgb_NULL	               :break;
	case cgb_BUMP				   :e(t2D);bindS(normalmap);break;
	case cgb_HEIGHT				   :e(t2D);bindS(heightmap);break;
	case cgb_HORIZONT			   :e(t2D);bindS(horizontmap);break;
	case cgb_GLOSS				   :e(t2D);break;
	case cgb_SPECULAR			   :e(t2D);bindS(specularmap);break;
	case cgb_SHADOW2D			   :e(t2D);bindC(Shadow2D);break;
	case cgb_SHADOWCUBE			   :e(tCB);bindC(ShadowCube);break;
	case cgb_ATT1				   :e(t1D);bindC(Atten1D);break;
	case cgb_ATT2				   :e(t2D);bindC(Atten2D);break;
	case cgb_ATT3				   :e(t3D);bindC(Atten3D);break;
    case cgb_ATT1Q				   :e(t1D);bindC(Atten1DQ);break;
	case cgb_NORMALMAP			   :e(t2D);bindC(NormalCubeMap);break;
	case cgb_FILTERMAPCUBE		   :e(tCB);bindC(FilterCubeMap);break;
	case cgb_REFLECTIONMAPCUBE	   :e(tCB);bindC(ReflectionCube);break;
	case cgb_DIFFUSE1			   :e(t2D);bindD(layer[0]);break;
	case cgb_DIFFUSE2			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE3			   :e(t2D);bindD(layer[2]);break;
	case cgb_DIFFUSE4			   :e(t2D);bindD(layer[3]);break;
	/*case cgb_DIFFUSE5			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE6			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE7			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE8			   :e(t2D);bindD(layer[1]);break;
	case cgb_DIFFUSE9			   :e(t2D);bindD(layer[1]);break;
	case cgb_UNKNOWN 			   :e(t2D);break;
	/**/
  }
  }
#undef e
#undef bindS
#undef bindC
#undef bindD
   glActiveTextureARB(GL_TEXTURE0_ARB);
   return true;
}


bool cgShader::BindNDraw(Draw::CRenderVertex &RV,BShader *Shader,DWORD NumIndex,LPCVOID Index,DWORD IndexType)
{
	if(BindRD(RV))
		if(BindShader(Shader))
		{
			if(IndexType)
             glDrawElements( GL_TRIANGLES, NumIndex,GL_UNSIGNED_INT  , Index);
			else
             glDrawElements( GL_TRIANGLES, NumIndex,GL_UNSIGNED_SHORT, Index);
			return true;
		}
    return false;
}
/**/
void cgShader::ShaderError(LPCSTR Format,...)
{
	Error2Console(SYS_GRAPHIC,"%s\n",Format);
}

bool cgShader::IsARBProfile()
{
	return fIsARBProfile==1;
}

bool cgShader::IsARBFPProfile()
{
	return fIsARBProfile==2;
}

void cgShader::StoreMPV()
{
  if(IsARBProfile())
	  return;
  if(Disabled)return;
//  SetStateMatrixParameter(cgp_modelViewProj,CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY );
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

/*void CLightShader::SetLightPos(const float *posrad)
{
  if(IsARBProfile())
  {
	  glLightfv(GL_LIGHT0,GL_AMBIENT,posrad);
	  bVector3f pos=posrad;
	  neM4 M;
//	  memcpy(&M.M,world->current_cam->Rotation4x4GL,sizeof(float)*16);
	  bVector3f pos0=M*pos;
	  float pos1[4];
	  memcpy(pos1,pos0,4*3);
	  pos1[3]=posrad[3];
	  glLightfv(GL_LIGHT1,GL_AMBIENT,pos1);
	  glLightfv(GL_LIGHT0,GL_POSITION,pos1);
      return;
  }
  else
  {
  SetParameter4fv(cgp_lpos_bright,posrad);
  }
}

bool CLightShader::Enable()
{
	camset=false;
	return cgShader::Enable();
}

void CLightShader::SetLightParam(const float *posrad)
{
//  LightPos.Set=posrad;
  SetLightPos(posrad);
  if(cgp_camera && !camset)
//   SetParameter3fv(cgp_camera,world->current_cam->pos );
  {}
  camset=true;
};

void CLightShader::SetLightParam(const float *position,const float Rad)
{
  LightPos.Set(position,Rad);
  SetLightParam(LightPos);
}

void CLightShader::Instrument()
{
	    cgShader::Instrument(); 
		cgp_lpos_bright=SetNamedParameter ("lpos_bright");
		cgp_camera     =SetNamedParameter ("camera");
		cgp_tc3        =SetNamedParameter ("tc3mat");
}

*/

void CModelShaderInstrument::SetFrame         (const float frame)
{
	if(cgp_frame)
		Driver->SetParameterf(cgp_frame,frame);
}


#define NUM_VECTOR_BONES 3
#define SIZE_VECTOR_BONES 4*NUM_VECTOR_BONES

bool CModelShaderInstrument::Instrument()
{
       if(Instrumented)return true;
       cgp_frame      =Driver->SetNamedParameter ("model_frame");
	   cgp_matrixstack=Driver->SetNamedParameter ("matrixs");
       if(!cgp_matrixstack)
	   {
           Error2Console(SYS_GRAPHIC,"ERROR:file %s is not a skin shader\n",Driver->GetName());
           return false;
	   }
	   else
	   {

	    MaxMatrix=0;
	    MinMatrix=0xFF*2;
	    MATRIXMAX= cgGetArraySize( GETBODY(Driver->GetParam(cgp_matrixstack)->body),0) / NUM_VECTOR_BONES;
	    MATRIXSTACK=new float[(MATRIXMAX+2)*SIZE_VECTOR_BONES];
	   }
      Instrumented=true;
      return true;
}

void CModelShaderInstrument::StartTransform()
{
   MaxMatrix=0;
   MinMatrix=0xFF*2;
}

void CModelShaderInstrument::AddMatrix(const DWORD ID,const float *mat)
{
	if(ID<MATRIXMAX)
	{
	  memcpy(MATRIXSTACK+ID*SIZE_VECTOR_BONES,mat,SIZE_VECTOR_BONES*sizeof(float));
	  MaxMatrix=MAX(MaxMatrix,ID);
	  MinMatrix=MIN(MinMatrix,ID);
	}
	else
	{
		Error2Console(SYS_GRAPHIC,"skin:to many bones(%d>%d)\n",ID,MATRIXMAX);
	}
}

void CModelShaderInstrument::AddBone(const DWORD ID,const GPUBone&Bone)
{
  AddMatrix(ID,&Bone.Matrix.M[0][0]);
}

void CModelShaderInstrument::EndTransform()
{
	Driver->SetMatrixArrayf(cgp_matrixstack,MinMatrix,(MaxMatrix-MinMatrix)*NUM_VECTOR_BONES,MATRIXSTACK+MinMatrix*SIZE_VECTOR_BONES);
}

void cgStoreMPV()
{
	if(ActiveVertexShader)
		ActiveVertexShader->StoreMPV();
}

void cgPushCustomTranform()
{
}

void cgStartCustomTranform()
{
	//glPushMatrix();
	//glLoadIdentity();
}
void cgEndCustomTranform()
{
	//float ViewMatrix[16];
	//glGetFloatv( GL_MODELVIEW_MATRIX, ViewMatrix );
	//glPopMatrix();
	//glMultMatrixf(ViewMatrix);
    //if(ActiveVertexShader)
    // ActiveVertexShader->StartCustomTransform(ViewMatrix);
}
void cgPopCustomTransform()
{
  //if(ActiveVertexShader)
   //ActiveVertexShader->EndCustomTransform();
}
