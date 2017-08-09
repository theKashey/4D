#include "stdafx.h"
#include "SoundShader.h"

CSoundShader::_MYBSPDIVIDOR  CSoundShader::BSPFor;
CSoundShader::_MYBSPDIVIDOR  CSoundShader::BSPStorage;
CSoundShader::_SourcePack    CSoundShader::SourcePack;
bool                         CSoundShader::BSPFuncInited=false;


#define SB_BUFFER                         1
#define SB_STREAM                         2

#define SB_GAIN                           3
#define SB_CLASS                          4

#define SB_DISTANCEMODEL                  5
#define SB_GAIN_PACK                      6
#define SB_VALID_DIST                     7 
#define SB_LOOPED                         8 
#define SB_AFTERLOOP                      9
#define SB_DISTFACTOR                     10

#define SB_DISTANCE_MODEL_DEFAULT          100 //hardare
#define SB_DISTANCE_MODEL_USERFALLOFF      101 // 1-dist
#define SB_DISTANCE_MODEL_USERFALLOFFQ     102 // 1-dist^2
#define SB_DISTANCE_MODEL_USERFALLOFFBQ    103 //(1-dist)^2
#define SB_DISTANCE_MODEL_USERFALLOFFBSQR  104 //1-sqrt(dist)


#define SB_SND_GAIN_OVERALL         200
#define SB_SND_GAIN_ALL             201
#define SB_SND_GAIN_MUSIC           202
#define SB_SND_GAIN_FX              203
#define SB_SND_GAIN_SPEECH          204
#define SB_SBD_GAIN_AMBIENT         205


void DLLTYPE LoadSoundShaders()
{
	CSoundShader Shader;
	Shader.StartParseSounds();
}

void    CSoundShader::ConstructBSP()
{
  GStringBase Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();BSPFor.PushAs(b,Base.v_str(),(DWORD)strlen(a))

 ADDBSP( SB_BUFFER                        ,"buffer");
 ADDBSP( SB_STREAM                        ,"stream");

 ADDBSP( SB_GAIN                          ,"gain");
 ADDBSP( SB_CLASS                         ,"class");

 ADDBSP( SB_DISTANCEMODEL                 ,"model");
 ADDBSP( SB_GAIN_PACK                     ,"pack");
 ADDBSP( SB_VALID_DIST                    ,"dist");
 ADDBSP( SB_LOOPED                        ,"looped");
 ADDBSP( SB_AFTERLOOP                     ,"afterloop");
 ADDBSP( SB_DISTFACTOR                    ,"dfactor");
 

 ADDBSP( SB_DISTANCE_MODEL_DEFAULT        ,"default");
 ADDBSP( SB_DISTANCE_MODEL_USERFALLOFF    ,"ufalloff");
 ADDBSP( SB_DISTANCE_MODEL_USERFALLOFFQ   ,"ufalloffq");
 ADDBSP( SB_DISTANCE_MODEL_USERFALLOFFBQ  ,"ufalloffq2");
 ADDBSP( SB_DISTANCE_MODEL_USERFALLOFFBSQR,"ufalloffsq2");

 
 ADDBSP( SB_SND_GAIN_OVERALL              ,"overall");
 ADDBSP( SB_SND_GAIN_ALL                  ,"all");
 ADDBSP( SB_SND_GAIN_MUSIC                ,"music");
 ADDBSP( SB_SND_GAIN_FX                   ,"fx");
 ADDBSP( SB_SND_GAIN_SPEECH               ,"speech");
 ADDBSP( SB_SBD_GAIN_AMBIENT              ,"ambient");



 BSPFuncInited=true;
}

PSoundShader ActiveSoundSM=NULL;

PSoundShader GetSoundShaderManager()
{
    return ActiveSoundSM;
}

CSoundShader::CSoundShader(void)
{
    ActiveSoundSM=this;
	if(!BSPFuncInited)   ConstructBSP();

}

CSoundShader::~CSoundShader(void)
{
}

void CSoundShader::StartParseSounds()
{
  StartParseFile("Sounds/common.sound");
  GString SName=GetFirstShader();
  //Print2Console(SYS_SHADERS,"FoundShader:%s",SName.c_str());
  while(1)
  {
	  ParseShader(SName);
	  SName=GetNextShader();
	  if(!SName.length())break;
	  //Print2Console(SYS_SHADERS,"FoundShader:%s",SName.c_str());
  }
 // Print2Console(SYS_SHADERS,"END");
}

void CSoundShader::StopParseSounds()
{
}

char *CSoundShader::GetFirstShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else 		    return NULL;
}
//
// Get the next shader of the file
//
char *CSoundShader::GetNextShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else         	return NULL;
}

CSoundSourceShader DefaultSourceParams;

void CSoundShader::ParseShader(LPCSTR ShaderName){
	if(!ShaderName)return;
	if(BSPStorage.GetIDFor(ShaderName,(DWORD)Istrlen(ShaderName)))
	{
		ShaderError("Warning: Duplicate shader found(%s)",ShaderName);
		return;
    }
	int   CurrentMode=0;
    CSoundSourceShader Source;//=new CSoundSourceShader();
    //if(DefaultSourceParams)
    Source=DefaultSourceParams;
	Depth=1;
	int CurrentLine;
	GString Token;
	if(CParser::GetToken(true))
 	 if(!IS_OPENBRACKET(CParser::token)) {ShaderError("Warning, '{' expected");return;}
	CurrentLine=CParser::scriptline;

	while( CParser::GetToken(true) )
	{
           if(IS_OPENBRACKET (CParser::token)) {
			   Depth++;
			   /*ParseLayer();*/
//               if(!PL)CurrentMode=-1;
//			   Buffer.Clear();
//			   Effect.Clear();
			   continue;
		   }
  	  else if(IS_CLOSEBRACKET(CParser::token)) 
	  {
		  Depth--;
		  /*CloseLayer();*/
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
        LPCSTR tmpName=NULL;
		switch(ShaderToken)
		{
          case  SB_BUFFER         : tmpName=GetParameter();if(tmpName && tmpName[0]!=0)Source.AddBuffer(tmpName,0); break;
          case  SB_STREAM         : tmpName=GetParameter();if(tmpName && tmpName[0]!=0)Source.AddBuffer(tmpName,1); break; 
 
          case  SB_GAIN           : Source.Gain=GetFloat(); break;  
          case  SB_CLASS          : Source.Class=GetFloat(); break;   

          case  SB_DISTANCEMODEL  : 
                                     GetParameter();
			                         Token=CParser::token;
		                             Token.MakeLower();
			                         Source.DistModel=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
                                     if(Source.DistModel)Source.DistModel-=100;
              
              break;       
          case  SB_GAIN_PACK      : GetParameter();
			                        Token=CParser::token;
		                            Token.MakeLower();
			                        Source.GainPack=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
                                    if(Source.GainPack)Source.GainPack-=200;
              
              break;       
          case  SB_VALID_DIST     : Source.DistMax =GetFloat(); break;         
          case  SB_LOOPED         : Source.Looped  =GetFloat()>0; break;         
          case  SB_AFTERLOOP      : Source.AlterLoopBuffer(); break; 
          case  SB_DISTFACTOR     : Source.DistFactor=GetFloat(); break;   
			 
			 /////////////////////
	     default:		InvalidToken();
		}
  	 //SkipLine();
	}
	if(strcmp(ShaderName,"!DEFAULT")==0)
	{
		DefaultSourceParams=Source;
		return;
	}
	SourcePack.push_back(Source);
	BSPStorage.PushAs(SourcePack.size(),ShaderName,(DWORD)Istrlen(ShaderName));
}

float   CSoundShader::GetFloat()
{
	if(!CParser::GetToken(false))
	{ShaderError("Warning: Paramenter missed");return false;}
	return atof(CParser::token);
}

char cNULL[2];

LPCSTR  CSoundShader::GetParameter()
{
	if(!CParser::GetToken(false))
	{ShaderError("Warning: Paramenter missed");cNULL[0]=0;return cNULL;}
	return CParser::token;
}

bool CSoundShader::GetShader(LPCSTR Name,CSoundSourceShader &Source)
{
	DWORD ID=BSPStorage.GetIDFor(Name,(DWORD)Istrlen(Name));
	if(!ID)return false;
	Source=SourcePack[ID-1];
	return true;
}

bool CSoundShader::GetShader(LPCSTR Name,CSoundSourceShader *&Source)
{
	DWORD ID=BSPStorage.GetIDFor(Name,(DWORD)Istrlen(Name));
	if(!ID)return false;
	Source=&SourcePack[ID-1];
	return true;
}

void CSoundShader::InvalidToken(void){
    ShaderError( "Предупреждение, invalid token: %s на линии %d\n", CParser::token, CParser::scriptline);
}

char sSPszError[1024*10];

void    CSoundShader::ShaderError(LPCSTR Error,...)
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
void    CSoundShader::ParseError (LPCSTR Error,...)
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
void    CSoundShader::ShaderNotImp()
{
	Error2Console(SYS_SHADERS,"%d|%d:This (%s) shader not implimented",CParser::scriptline,Depth,CParser::token);
	//SkipLine();
}

bool CSoundShader::GetName(void){
	return CParser::GetToken(true);
}







