#include "stdafx.h"
#include "ParticleShader.h"
#include "ProjectGraphic.h"
#include "ProjectParticles.h"

SmartVector<DWORD> OffDataArray;
DWORD OffCount=0;

LPVOID OffData(DWORD Data)
{
	static float fvNULL[4];
	 if(Data==(DWORD)-1) return (LPVOID)fvNULL;
	return (LPVOID)((DWORD)&OffDataArray[0]+Data);
}

CParticleParams::PPLE::PPLE()
{
	ID=Type=0;
	Data=-1;
}


float CParticleParams::PPLE::GetFloat(int ID){return *((float*)OffData(Data+ID*4));}
const float* CParticleParams::PPLE::GetVector(int ID){return ((float*)OffData(Data+ID*12));}

void  CParticleParams::PPLE::PushFloat(float F)
{
  OffDataArray.push_back(1);
  if(Data==-1)Data=OffCount;
  *((float*)OffData(OffCount))=F;
  OffCount+=4;
  Type=0;
}
void  CParticleParams::PPLE::PushVector(const float* F)
{
 OffDataArray.push_back(1);
 OffDataArray.push_back(1);
 OffDataArray.push_back(1);
 if(Data==-1)
  Data=OffCount;
 *((float*)OffData(OffCount ))=F[0];
 *((float*)OffData(OffCount+4))=F[1];
 *((float*)OffData(OffCount+8))=F[2];
 OffCount+=12;
 Type++;
}


DWORD CParticleParams::GetNumParams()
{
	return List.size();
}

CParticleParams::PPLE  CParticleParams::GetParam(int ID)
{
	return List[ID];
}


	CParticleShader::CParticleShader()
	{
		    create_DEFCOUNT    =0;
		    create_CHARGETYPE  =0;
			create_CHARGECOUNT =0;
			create_USECENTER   =0;
			create_BLEND	   =BLEND_ADD;
			willRotate         =0;

	}
	LPCSTR CParticleShader::GetCallBack     ()
	{
		return CallBackName;
	}

	bVector3f CParticleShader::GetSystemSize()
	{
		return SystemSize;
	}
	LPCSTR CParticleShader::GetCallBackParam()
	{
		return CallBackParam;
	}
	LPCSTR CParticleShader::GetSybset       ()
	{
		return "";
	}
	LPCSTR CParticleShader::GetMainTextureName   ()
	{
		return TextureName;
	}
	LPCSTR CParticleShader::GetCentralTextureName()
	{
		if(CentralTexture.IsEmpty())
			return GetMainTextureName();
		else return CentralTexture;
	}
	DWORD  CParticleShader::GetDefCreteCount()
	{
		return create_DEFCOUNT;
	}
	DWORD  CParticleShader::GetChargeType   ()
	{
		return create_CHARGETYPE;
	}
	DWORD  CParticleShader::GetChargeCount  ()
	{
		return create_CHARGECOUNT;
	}
	DWORD  CParticleShader::GetCenterUsage  ()
	{
		return create_USECENTER;
	}
	DWORD  CParticleShader::GetRotatableFlag()
	{
		return willRotate;
	}
	DWORD  CParticleShader::GetColorFlag()
	{
		return willColor;
	}
	DWORD  CParticleShader::GetBlendingType ()
	{
		return create_BLEND;
	}
	CParticleParams *CParticleShader::GetEmiterParams   (int Type)
	{
		if(listEmiter.GetNumParams() || Type)return &listEmiter;else return NULL;
	}
	CParticleParams *CParticleShader::GetRegisterParams (int Type)
	{
        if(listRegister.GetNumParams() || Type)return &listRegister;else return NULL;
	}
	CParticleParams *CParticleShader::GetControllerParams(int Type)
	{
		if(listController.GetNumParams() || Type)return &listController;else return NULL;
	}
	CParticleParams *CParticleShader::GetKillerParams   (int Type)
	{
		if(listKiller.GetNumParams() || Type)return &listKiller;else return NULL;
	}

//	void CParticleShader::SetSybSetName(LPCSTR Name){Sybsetname=Name;}

	typedef SmartVector<CParticleShader*> CParShaderList;
CParShaderList ParticlesShaderSybset;

typedef CBSPDividorLookUpArray<char,int> _PARTICLENAMEBSP;
_PARTICLENAMEBSP  ParticleNameBSP;

DWORD RegisterParticleShader(LPCSTR Name,CParticleShader *Shad)
{
 	ParticlesShaderSybset.push_back(Shad);
	GString N=Name;
	N.MakeLower();
	ParticleNameBSP.PushAs(ParticlesShaderSybset.size(),N.c_str(),N.length());
	return ParticlesShaderSybset.size();
}

PParticleShader GetParticleShader(LPCSTR Name)
{
	GString N=Name;
	N.MakeLower();
	DWORD ID=ParticleNameBSP.GetIDFor(N.c_str(),N.length());
	if(ID)
	{
		return ParticlesShaderSybset[ID-1];
	}
	return NULL;
}

void DLLTYPE LoadParticleShaders()
{
	CParticleShaderManager Shader;
	Shader.StartParseParticles();
}			  

_PARTICLENAMEBSP             BSPFor;
bool                         BSPFuncInited=false;

#define PSH_EMITER							1
#define PSH_REGISTER						2
#define PSH_CONTROLER						3
#define PSH_KILLER							4
#define PSH_SYBSET							5
#define PSH_TEXTURE							6
#define PSH_CENTERTEXTURE					7
#define PSH_DEFCOUNT						8
#define PSH_CHARGETYPE						9
#define PSH_CHARGECOUNT						10
#define PSH_USECENTER						11
#define PSH_BLEND							12
#define PSH_WROTATE							13
#define PSH_CENTERPARTICLEDEF				14
#define PSH_DRAWER                          1001
#define PSH_COLORED                         1002
#define PSH_COLOR                           1003
#define PSH_PCOLOR                          1004
#define PSH_ADVANCEEMITE                    1005
#define PSH_CALLBACK                        1100
#define PSH_CALLBACKPARAM                   1101
#define PSH_SYSTEMSIZE                      1102

#define PSHa_BLEND_ADD						15
#define PSHa_BLEND_ALPHA					16
#define PSHa_BLEND_MUL2						17
#define PSHa_BLEND_MUL						18
											
#define PSHa_CENTERUSE_NONE					20
#define PSHa_CENTERUSE_TRANSLATE			21
#define PSHa_CENTERUSE_ROTATE				22
#define PSHa_CENTERUSE_BOTH					23
#define PSHa_CENTERUSE_SCALE				24
#define PSHa_CENTERUSE_TRSC					25
#define PSHa_CENTERUSE_RTSC					26
#define PSHa_CENTERUSE_ALL					27

#define PSHa_CHARGE_NONE					30
#define PSHa_CHARGE_ONCE					31
#define PSHa_CHARGE_TIMER					32
#define PSHa_CHARGE_CONST					33
#define PSHa_CHARGE_TIMERFLOAT				34

#define PSHa_CHARGE_FLOAT_FALLOF			35

#define PSH_v_PAREM_CENTER                 100
#define PSH_v_PAREM_SPEED                  102
#define PSH_v_PAREM_OFFSET                 103
#define PSH_v_PAREM_MASS                   104
#define PSH_v_PAREM_2RAD                   105
#define PSH_v_PAREM_RAD                    106
#define PSH_v_PAREM_RAD1                   107
#define PSH_v_PAREM_RAD2                   108
#define PSH_v_PAREM_SIZE                   109
#define PSH_v_PAREM_DELTASIZE              110
#define PSH_v_PAREM_DELTAMASS              111
#define PSH_v_PAREM_DELTALIFE              112
#define PSH_v_PAREM_LIFE                   113
#define PSH_v_PAREM_DELTASPEED             114 
#define PSH_v_PAREM_DELTACENTER            115 
#define PSH_v_PAREM_ROTATION               116
#define PSH_v_PAREM_DELTAROTATION          117
#define PSH_v_PAREM_SIZECHANGE             118

#define PSH_v_PAREM_DELTASF                120 
#define PSH_v_PAREM_SF                     121
#define PSH_v_COLOR_CHANGE                 122
#define PSH_v_COLOR_STAGE                  123
#define PSH_v_ROT_SPEED                    124 
#define PSH_v_DROT_SPEED                   125

#define PSH_s_OPEN                         200

void ConstructBSP()
{
  GStringBase Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();BSPFor.PushAs(b,Base.v_str(),(DWORD)strlen(a))
     ADDBSP( PSH_EMITER				,"emiter");
     ADDBSP( PSH_REGISTER			,"register");
     ADDBSP( PSH_CONTROLER			,"controller");
     ADDBSP( PSH_KILLER				,"killer");
     ADDBSP( PSH_SYBSET				,"sybset");
     ADDBSP( PSH_TEXTURE			,"texture");
     ADDBSP( PSH_CENTERTEXTURE		,"centertexture");
     ADDBSP( PSH_DEFCOUNT			,"defcount");
     ADDBSP( PSH_CHARGETYPE			,"chargetype");
     ADDBSP( PSH_CHARGECOUNT		,"chargecount");
     ADDBSP( PSH_USECENTER			,"usecenter");
     ADDBSP( PSH_BLEND				,"blend");
     ADDBSP( PSH_WROTATE			,"rotatable");
     ADDBSP( PSH_CENTERPARTICLEDEF	,"centerparticle");
	 ADDBSP( PSH_COLORED            ,"colored");
	 ADDBSP( PSH_COLOR              ,"color");
	 ADDBSP( PSH_PCOLOR             ,"pcolor");
	 ADDBSP( PSH_ADVANCEEMITE       ,"advanceemite");

	 ADDBSP( PSH_CALLBACK           ,"callback");
	 ADDBSP( PSH_CALLBACKPARAM      ,"callbackparam");
	 ADDBSP( PSH_SYSTEMSIZE         ,"syssize");
		 

     ADDBSP( PSHa_BLEND_ADD			,"add");
     ADDBSP( PSHa_BLEND_ALPHA		,"alpha");
     ADDBSP( PSHa_BLEND_MUL2		,"mul2");
     ADDBSP( PSHa_BLEND_MUL			,"mul");
											
     ADDBSP( PSHa_CENTERUSE_NONE	,"unuse");
     ADDBSP( PSHa_CENTERUSE_TRANSLATE,"TR");
     ADDBSP( PSHa_CENTERUSE_ROTATE	,"RT");
     ADDBSP( PSHa_CENTERUSE_BOTH	,"TRRT");
     ADDBSP( PSHa_CENTERUSE_SCALE	,"SC");
     ADDBSP( PSHa_CENTERUSE_TRSC	,"TRSC");
     ADDBSP( PSHa_CENTERUSE_RTSC	,"RTSC");
     ADDBSP( PSHa_CENTERUSE_ALL		,"ALL");
									 
	 ADDBSP( PSHa_CHARGE_NONE		,"none");
     ADDBSP( PSHa_CHARGE_ONCE		,"once");
     ADDBSP( PSHa_CHARGE_TIMER		,"timer");
     ADDBSP( PSHa_CHARGE_CONST		,"const");
     ADDBSP( PSHa_CHARGE_TIMERFLOAT	,"float");
									 
     ADDBSP( PSHa_CHARGE_FLOAT_FALLOF,	"floatfactor");

     ADDBSP( PSH_v_PAREM_CENTER     ,"center");
     ADDBSP( PSH_v_PAREM_DELTACENTER,"deltacenter"); 
     ADDBSP( PSH_v_PAREM_SPEED      ,"speed");
     ADDBSP( PSH_v_PAREM_DELTASPEED ,"deltaspeed");
     ADDBSP( PSH_v_PAREM_OFFSET     ,"offset");
     ADDBSP( PSH_v_PAREM_MASS       ,"mass");
     ADDBSP( PSH_v_PAREM_2RAD       ,"2rad");
     ADDBSP( PSH_v_PAREM_RAD        ,"rad");
     ADDBSP( PSH_v_PAREM_RAD1       ,"rad1");
     ADDBSP( PSH_v_PAREM_RAD2       ,"rad2");
     ADDBSP( PSH_v_PAREM_SIZE       ,"size");
     ADDBSP( PSH_v_PAREM_DELTASIZE  ,"deltasize");
     ADDBSP( PSH_v_PAREM_DELTAMASS  ,"deltamass");
     ADDBSP( PSH_v_PAREM_DELTALIFE  ,"deltalife");
     ADDBSP( PSH_v_PAREM_LIFE       ,"lifetime");
     ADDBSP( PSH_v_PAREM_DELTASF    ,"deltaspeedfactor");
     ADDBSP( PSH_v_PAREM_SF         ,"speedfactor");
     ADDBSP( PSH_v_PAREM_ROTATION   ,"rotation");
     ADDBSP( PSH_v_PAREM_DELTAROTATION,"deltarotation");
     ADDBSP( PSH_v_PAREM_SIZECHANGE   ,"sizechange");

     ADDBSP( PSH_v_COLOR_CHANGE      ,"colorchange");
     ADDBSP( PSH_v_COLOR_STAGE       ,"colorstage");
     ADDBSP( PSH_v_ROT_SPEED         ,"rotspeed");
     ADDBSP( PSH_v_DROT_SPEED        ,"deltarotspeed");
	 ADDBSP( PSH_s_OPEN       ,"{");
}

CParticleShaderManager::CParticleShaderManager(void)
{
    //ActiveSoundSM=this;
	if(!BSPFuncInited)   ConstructBSP();

}

CParticleShaderManager::~CParticleShaderManager()
{
}

void InitParticleSybsetShaders()
{
	CParticleShaderManager Man;
	Man.StartParseParticles();
}

void CParticleShaderManager::StartParseParticles()
{
  StartParseFile("Particles/common.shd");
  GString SName=GetFirstShader();
  //Print2Console(SYS_SHADERS,"FoundShader:%s",SName.c_str());
  while(1)
  {
	  ParseShader(SName);
	  SName=GetNextShader();
	  if(!SName.length())break;
	  //Print2Console(SYS_SHADERS,"FoundShader:%s",SName.c_str());
  }
  //Print2Console(SYS_SHADERS,"END");
}

char *CParticleShaderManager::GetFirstShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else 		    return NULL;
}

void CParticleShaderManager::StopParseParticles()
{
}

/*char *CParticleShaderManager::StopParseParticles(void){
	errors=0;
	if(GetName())	return CParser::token;
	else 		    return NULL;
}  */
//
// Get the next shader of the file
//
char *CParticleShaderManager::GetNextShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else         	return NULL;
}

float   CParticleShaderManager::GetFloat()
{
	if(!CParser::GetToken(false))
	{ShaderError("Warning: Paramenter missed");return false;}
	return atof(CParser::token);
}

LPCSTR  CParticleShaderManager::GetParameter(bool Won)
{
	static char cNULL[2];
	if(!CParser::GetToken(false))
	{if(Won)ShaderError("Warning: Paramenter missed");cNULL[0]=0;return cNULL;}
	return CParser::token;
}


void CParticleShaderManager::InvalidToken(void){
    ShaderError( "Warining, invalid token: %s on line %d\n", CParser::token, CParser::scriptline);
}

extern char sSPszError[1024*10];

void    CParticleShaderManager::ShaderError(LPCSTR Error,...)
{
    /*va_list ap;
    va_start(ap, Error);
    vsprintf(sSPszError, Error, ap);
    va_end(ap);
	*/
    DOFORMAT(Err,Error);
	LPCSTR sSPszError=Err;
	Error2Console(SYS_SHADERS,"SE%d|%d:%s",CParser::scriptline,Depth,sSPszError);
}
void    CParticleShaderManager::ParseError (LPCSTR Error,...)
{	
    /*va_list ap;
    va_start(ap, Error);
    vsprintf(sSPszError, Error, ap);
    va_end(ap);
	*/
	DOFORMAT(Err,Error);
	LPCSTR sSPszError=Err;
	Error2Console(SYS_SHADERS,"PE%d|%d:%s",CParser::scriptline,Depth,sSPszError);
}
void    CParticleShaderManager::ShaderNotImp()
{
	Error2Console(SYS_SHADERS,"%d|%d:This (%s) shader not implimented",CParser::scriptline,Depth,CParser::token);
	//SkipLine();
}

bool CParticleShaderManager::GetName(void){
	return CParser::GetToken(true);
}

CParticleShader *DefaultSourceParams=NULL;


void CParticleShaderManager::ParseShader(LPCSTR ShaderName){
	if(ParticleNameBSP.GetIDFor(ShaderName,(DWORD)Istrlen(ShaderName)))
	{
		ShaderError("Warning: Duplicate shader found(%s)",ShaderName);
		return;
    }
	int   CurrentMode=0;
	Depth=1;
	int CurrentLine;
	GString Token;
	if(CParser::GetToken(true))
 	 if(!IS_OPENBRACKET(CParser::token)) {ShaderError("Warning, '{' expected");return;}
	CurrentLine=CParser::scriptline;


	//GString EmiterName,RegisterName,ControllerName,KillerName,SysSetName,
	GString Texture,CenterTexture;
	GString DrawerName="CparticleCPUDrawer";
	CParticleShader *Sys=new CParticleShader();
	Sys->create_CHARGETYPE =PD_EMITE_UNDEF;
	Sys->willRotate        =0;
	Sys->create_BLEND      =BLEND_ADD;
	Sys->create_CHARGECOUNT=0;
	Sys->create_DEFCOUNT   =0;
	Sys->create_USECENTER  =0;
	Sys->willColor         =0;
	Sys->AdvanceEmite      =1;
	Sys->Color.Set(1,1,1);
	Sys->PColor.Set(1,1,1);

    //GString EmiterName,RegisterName,ControllerName,KillerName,SysSetName,Texture,CenterTexture;
    float DefCount=10,ChangeCount=10;
	bool Rotatable=false;
	DWORD LastTokenID=0;
	while( CParser::GetToken(true) )
	{
           if(IS_OPENBRACKET (CParser::token)) {
			   Depth++;
			   switch(LastTokenID)
			   {
			       case PSH_EMITER:
					   PerformEmiterSet(Sys->GetEmiterParams(1),Depth);
					   break;
				   case PSH_REGISTER:
					   PerformEmiterSet(Sys->GetRegisterParams(1),Depth);
					   break;
				   case PSH_CONTROLER:
					   PerformEmiterSet(Sys->GetControllerParams(1),Depth);
					   break;
				   case PSH_KILLER:
					   PerformEmiterSet(Sys->GetKillerParams(1),Depth);
					   break;
				   case PSH_CENTERPARTICLEDEF:break;
			   }
			   continue;
		   }
  	  else if(IS_CLOSEBRACKET(CParser::token)) 
	  {
		  Depth--;
		  if(!Depth)break;continue;
		  CurrentMode=0;
	  }
      else
      if(CurrentMode==-1)continue;
	  else
	  if(CurrentMode){ShaderError("{ must follow after path token");}
		Token=CParser::token;
		Token.MakeLower();
		DWORD ShaderToken=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
		GString T;
		switch(ShaderToken)
		{	 
          case PSH_EMITER			 :Sys->EmiterName     =GetParameter();break;
          case PSH_REGISTER		     :Sys->RegisterName   =GetParameter();break;
          case PSH_CONTROLER		 :Sys->ControllerName =GetParameter();break;
          case PSH_KILLER			 :Sys->KillerName     =GetParameter();break;
		  case PSH_DRAWER            :DrawerName     =GetParameter();break;
          //case PSH_SYBSET			 :SysSetName     =GetParameter();break;
          case PSH_TEXTURE			 :Sys->TextureName   =(GetParameter());break;
          case PSH_CENTERTEXTURE	 :Sys->CentralTexture=(GetParameter());break;
          case PSH_DEFCOUNT		     :Sys->create_DEFCOUNT=(GetFloat());break;
          case PSH_CHARGETYPE		 :
			  {
				  Token=GetParameter();
		          Token.MakeLower();
		          DWORD ST=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
				  switch(ST)
				  {
				     case PSHa_CHARGE_NONE      :Sys->create_CHARGETYPE=PD_EMITE_USER;break;
				     case PSHa_CHARGE_ONCE	    :Sys->create_CHARGETYPE=PD_EMITE_ONCE;break;
				     case PSHa_CHARGE_TIMER	    :Sys->create_CHARGETYPE=PD_EMITE_TIMER;break;
				     case PSHa_CHARGE_CONST	    :Sys->create_CHARGETYPE=PD_EMITE_FRAME;break;
				     case PSHa_CHARGE_TIMERFLOAT:Sys->create_CHARGETYPE=PD_EMITE_TIMERFLOAT;break;
				  }
			  }
			  break;
          case PSH_CHARGECOUNT		 :Sys->create_CHARGECOUNT=GetFloat();break;
          case PSH_USECENTER		 :
			   {
				  Token=GetParameter();
		          Token.MakeLower();
		          DWORD ST=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
				  switch(ST)
				  {
                    case PSHa_CENTERUSE_NONE     : Sys->create_USECENTER=0;break;
                    case PSHa_CENTERUSE_TRANSLATE: Sys->create_USECENTER=PD_CENTER_TR;break;
                    case PSHa_CENTERUSE_ROTATE	 : Sys->create_USECENTER=PD_CENTER_RT;break;
                    case PSHa_CENTERUSE_BOTH	 : Sys->create_USECENTER=PD_CENTER_TR|PD_CENTER_RT;break;
                    case PSHa_CENTERUSE_SCALE	 : Sys->create_USECENTER=PD_CENTER_SC;break;
                    case PSHa_CENTERUSE_TRSC	 : Sys->create_USECENTER=PD_CENTER_TR|PD_CENTER_SC;break;
                    case PSHa_CENTERUSE_RTSC	 : Sys->create_USECENTER=PD_CENTER_RT|PD_CENTER_SC;;break;
                    case PSHa_CENTERUSE_ALL		 : Sys->create_USECENTER=PD_CENTER_TR|PD_CENTER_RT|PD_CENTER_SC;;break;
				  }
			   }
			  break;
          case PSH_BLEND			 :
			  {
				  Token=GetParameter();
		          Token.MakeLower();
		          DWORD ST=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
				  switch(ST)
				  {
				  case PSHa_BLEND_ADD  	: Sys->create_BLEND=BLEND_ADD;break;	
				  case PSHa_BLEND_ALPHA : Sys->create_BLEND=BLEND_ALPHA;break;	
  				  case PSHa_BLEND_MUL2	: Sys->create_BLEND=BLEND_MUL2;break;	
				  case PSHa_BLEND_MUL	: Sys->create_BLEND=BLEND_MUL;break;	
				  }
			  }
			  break;
          case PSH_WROTATE			 :
			  T=GetParameter();
			  T.MakeLower();
			  Sys->willRotate=(T=="1") || (T=="on") || (T=="true");break;
          case PSH_COLORED:
              T=GetParameter();
			  T.MakeLower();
			  Sys->willColor=(T=="1") || (T=="on") || (T=="true");break;

          case PSH_ADVANCEEMITE:
              T=GetParameter();
			  T.MakeLower();
			  Sys->AdvanceEmite=(T=="1") || (T=="on") || (T=="true");break;
		  case PSH_COLOR:
              Sys->Color=Math::VectorFromString(GetParameter(false));
              break;
		  case PSH_PCOLOR:
			  Sys->PColor=Math::VectorFromString(GetParameter(false));
              break;
          case PSH_CENTERPARTICLEDEF :
			  break;

		  case PSH_CALLBACK:Sys->CallBackName=GetParameter();break;
		  case PSH_CALLBACKPARAM:Sys->CallBackParam=GetParameter();break;
		  case PSH_SYSTEMSIZE: Sys->SystemSize=Math::VectorFromString(GetParameter());break;
	     default:		InvalidToken();
		}
		LastTokenID=ShaderToken;
  	 //SkipLine();
	}
	bool EN=false,RN=false,CN=false,KN=false;
	if(Sys->EmiterName    .IsEmpty() || Sys->EmiterName    =="PARD"){EN=true;Sys->EmiterName="PARD";}
	if(Sys->RegisterName  .IsEmpty() || Sys->RegisterName  =="PARD"){RN=true;Sys->RegisterName="PARD";}
	if(Sys->ControllerName.IsEmpty() || Sys->ControllerName=="PARD"){CN=true;Sys->ControllerName="PARD";}
	if(Sys->KillerName    .IsEmpty() || Sys->KillerName    =="PARD"){KN=true;Sys->KillerName="PARD";}

	if(strcmp(ShaderName,"!DEFAULT")==0)
	{
		DefaultSourceParams=Sys;
		return;
	}
	RegisterParticleShader(ShaderName,Sys);

}

void    CParticleShaderManager::PerformEmiterSet(CParticleParams *Params,int &Depth)
{
	int StartDepth=Depth;
	GString Token;
	while( CParser::GetToken(true) )
	{
           if(IS_OPENBRACKET (CParser::token))  Depth++;
		   else if(IS_CLOSEBRACKET(CParser::token)) 
	       {
   		    Depth--;
		    if(Depth<=StartDepth)break;continue;
	       }
		   Token=CParser::token;
		   Token.MakeLower();
		   DWORD ShaderToken=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
		   GString T;
		   CParticleParams::PPLE PP;
		   bool NoError=true;
		   switch(ShaderToken)
		   {
			   case PSH_v_PAREM_CENTER       :PP.SetID(PAREM_CENTER);     break;
			   case PSH_v_PAREM_DELTACENTER  :PP.SetID(PAREM_DELTACENTER);break; 
			   case PSH_v_PAREM_SPEED        :PP.SetID(PAREM_SPEED);      break;
			   case PSH_v_PAREM_DELTASPEED   :PP.SetID(PAREM_DELTASPEED); break;
     		   case PSH_v_PAREM_OFFSET       :PP.SetID(PAREM_OFFSET);     break;
     		   case PSH_v_PAREM_MASS         :PP.SetID(PAREM_MASS);       break;
    		   case PSH_v_PAREM_DELTAMASS    :PP.SetID(PAREM_DELTAMASS);  break;
     		   case PSH_v_PAREM_2RAD         :PP.SetID(PAREM_2RAD);       break;
     		   case PSH_v_PAREM_RAD          :PP.SetID(PAREM_RAD);        break;
     		   case PSH_v_PAREM_RAD1         :PP.SetID(PAREM_RAD1);       break;
     		   case PSH_v_PAREM_RAD2         :PP.SetID(PAREM_RAD2);       break;
     		   case PSH_v_PAREM_SIZE         :PP.SetID(PAREM_SIZE);       break;
     		   case PSH_v_PAREM_DELTASIZE    :PP.SetID(PAREM_DELTASIZE);  break;
     		   case PSH_v_PAREM_LIFE         :PP.SetID(PAREM_LIFE);       break;
      		   case PSH_v_PAREM_DELTALIFE    :PP.SetID(PAREM_DELTALIFE);  break;
     		   case PSH_v_PAREM_SF           :PP.SetID(PAREM_SPEEDFACTOR);       break;
      		   case PSH_v_PAREM_DELTASF      :PP.SetID(PAREM_DELTASPEEDFACTOR);  break;
			   case PSH_v_PAREM_SIZECHANGE   :PP.SetID(PAREM_SIZECHANGE);  break;
			   case PSH_v_PAREM_ROTATION     :PP.SetID(PAREM_ROTATION);       break;
      		   case PSH_v_PAREM_DELTAROTATION:PP.SetID(PAREM_DELTAROTATION);  break;

			   case PSH_v_COLOR_CHANGE       :PP.SetID(PAREM_COLORCHANGE);       break;
      		   case PSH_v_COLOR_STAGE        :PP.SetID(PAREM_COLORSTAGE);  break;

			   case PSH_v_ROT_SPEED          :PP.SetID(PAREM_ROTATIONSP);break;
               case PSH_v_DROT_SPEED          :PP.SetID(PAREM_DELTAROTATIONSP);break;

			   default:NoError=false;
		   }
		   if(NoError)
		   {
			GString P =GetParameter();
			if(P.Find(",")==-1)
              PP.PushFloat(atof(P));
			else
			{
			  GString P2=GetParameter(false);
			  if(P2.IsEmpty())
				  PP.PushVector(Math::VectorFromString(P));
			  else
			  {
				PP.PushVector(Math::VectorFromString(P));
				PP.PushVector(Math::VectorFromString(P2));
			  }
			}
		    Params->AddParam(PP);
		   }
	}
	Depth=StartDepth-1;
}