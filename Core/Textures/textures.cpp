#include "StdAfx.h"
#include "Core\Graphics\Textures\Textures.h"
#include "Core\Graphics\Textures\in_Textures.h"
#include "coregraphics.h"
//#include "Core\Graphics\OpenGL\grOpenGL.h"
#include "ProjectGraphic.h"
//#include "Textures.h"

#include "Video/VideoTexture.h"

/**********
 ANIMATED TEXTURE
**/////////


CAnimatedTexture::CAnimatedTexture()
{
 Reset();
 framespersecond=1;
 maxframe=0;
 EndMode=ATM_REPLAY;
}
float CAnimatedTexture::GetFrame()
{
  float T=Time::Time();
  currentframe+=(T-currenttime)*framespersecond;
  currenttime=T;
  if(currentframe>maxframe)
  switch(EndMode)
  {
      case ATM_REPLAY  : while(currentframe>maxframe)currentframe-=maxframe;break;
      case ATM_STOP    : currentframe=maxframe;                             break;
      case ATM_PINGPONG: framespersecond*=-1;                               break;
  }
  if(currentframe<0)
  switch(EndMode)
  {
      case ATM_REPLAY  : currentframe=0;                                    break;
      case ATM_STOP    : currentframe=0;                                    break;
      case ATM_PINGPONG: framespersecond*=-1;                               break;
  }
  return currentframe;
}
void CAnimatedTexture::Reset()
{
 currenttime=Time::Time();
 currentframe=0;
}
void CAnimatedTexture::SetEndMode(BYTE Mode)
{
 EndMode=Mode;
}

void CAnimatedTexture::SetMaxFrame(float MF)
{
  maxframe=MF;
}
void CAnimatedTexture::SetFreq    (float FREQ)
{
  framespersecond=1.0f/FREQ;
}
void CAnimatedTexture::SetFrame   (float FR)
{
  currentframe=FR;
}


/*************
 TEXTURE PARAMS
**///////////

 CTextureParameters CTextureParameters::GET(){CTextureParameters T;return T;}
 CTextureParameters *CTextureParameters::ref(){return this;}

 bool CTextureContainer::Load(LPCSTR FileName){return false;}

bool CTextureParameters::operator == (CTextureParameters &Sourse) const
  {
	  return true;
	  return UseMipMapGeneration==Sourse.UseMipMapGeneration &&
		     UseAlpha           ==Sourse.UseAlpha            &&
			 UseMipmaps         ==Sourse.UseMipmaps          &&
			 Filter             ==Sourse.Filter              &&
			 WRAP_S             ==Sourse.WRAP_S              &&
			 WRAP_T             ==Sourse.WRAP_T              &&
			 Anisotropic        ==Sourse.Anisotropic;
  }
  CTextureParameters::CTextureParameters(TEX_TYPE TType)
  {
     UseMipMapGeneration=true;
     UseAlpha   =true;
     UseMipmaps =false;
     Filter     =TEX_FILTER_TRILINEAR;
	 Anisotropic=0;
     EnvMode    =TEX_ENV_MODULATE;
     WRAP_S     =TEX_WRAP_REPEAT;
     WRAP_T     =TEX_WRAP_REPEAT;
	 if(TType==TEX_TYPE_3D)
       WRAP_R   =TEX_WRAP_REPEAT;
	 else
      WRAP_R    =TEX_WRAP_NONE;
     WRAP_Q     =TEX_WRAP_NONE;
  }
  int CTextureParameters::IsUseMipMapGeneration()const{return UseMipMapGeneration;}
  int CTextureParameters::IsUseAlpha()           const{return UseAlpha;}
  int CTextureParameters::IsUseMipmaps()         const{return UseMipmaps;}
  int CTextureParameters::GetFilter()            const{return Filter;}
  int CTextureParameters::GetAniso()             const{return Anisotropic;}
  int CTextureParameters::GetEnvMode()           const{return EnvMode;}
  int CTextureParameters::GetWRAP_S()            const{return WRAP_S;}
  int CTextureParameters::GetWRAP_T()            const{return WRAP_T;}
  int CTextureParameters::GetWRAP_R()            const{return WRAP_R;}
  int CTextureParameters::GetWRAP_Q()            const{return WRAP_Q;}
  int CTextureParameters::GetWRAP(int I)         const
  {
	  switch(I)
	  {
	  case 0:return GetWRAP_T();
      case 1:return GetWRAP_S();
	  case 2:return GetWRAP_R();
	  case 3:return GetWRAP_Q();
	  }
	  return TEX_WRAP_NONE;
  }

  CTextureParameters& CTextureParameters::SetUseMipMapGeneration(const DWORD Value)
  {
	  UseMipMapGeneration=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetUseAlpha           (const DWORD Value)
  {
	  UseAlpha=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetUseMipmaps         (const DWORD Value)
  {
	  UseMipmaps=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetFilter             (const DWORD Value)
  {
	  Filter=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetAniso              (const DWORD Value)
  {
	  Anisotropic=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetEnvMode            (const DWORD Value)
  {
	  EnvMode=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetWRAP_S             (const DWORD Value)
  {
	  WRAP_S=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetWRAP_T             (const DWORD Value)
  {
	  WRAP_T=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetWRAP_R             (const DWORD Value)
  {
	  WRAP_R=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetWRAP_Q             (const DWORD Value)
  {
	  WRAP_Q=Value;
	  return *this;
  }
  CTextureParameters& CTextureParameters::SetWRAP         (int I,const DWORD Value)
  {
	  switch(I)
	  {
	  case 0:return SetWRAP_T(Value);
      case 1:return SetWRAP_S(Value);
	  case 2:return SetWRAP_R(Value);
	  case 3:return SetWRAP_Q(Value);
	  }
	  return *this;
  }


DWORD __GetSysTextures();

//CTextureParameters TEXTURE_DEFAULT_FONT();

_TEXNAMETOID CTexture::LookUp;
//_TEXIDTOADDR CTexture::IDLook; 


CTexture::CTexture()
{
 if(!SYS_TEXTURES)SYS_TEXTURES=__GetSysTextures();
 TextureContainer=NULL;
 Width=0;
 Height=0;
}

CTexture::~CTexture()
{
 delete TextureContainer;
 //Unload();
}

bool CTexture::Load(LPCSTR Source,CTextureParameters *Params)
{
 //Print2Console(SYS_TEXTURES,"Loading %s",Source.c_str());
 if(strstr(Source,".avi"))
 {
  if(TextureContainer)delete TextureContainer;
   TextureContainer = new CVideoTexture();
   if(TextureContainer->Load(Source))
       return true;
   else
   {
     delete TextureContainer;
     TextureContainer=NULL;
     Print2Console(SYS_TEXTURES,"Load %s - no AVI file found",Source);
     return false;
   }
 }
 if(!Params) Params=new CTextureParameters(); 
 CTexture *Duplicate;
 u_Texture_Loader *Loader;
 if((Duplicate=IsTextureLoaded(Source,Params)))
 {
  *this=*Duplicate;
  //Print2Console(SYS_TEXTURES,"Load %s - duplicated",Source.c_str());
  return true;
 }
 if((Loader=GetLoader(Source)))
 {
  bool ret= LoadFrom(Loader,Params);
  if(ret)
   LookUp.PushAs((DWORD)this,Source,strlen(Source));
  //Loader->Clear();
  delete Loader;
  return ret;
 }
 return false;
}

bool CTexture::SubTexture(bVector2w To,u_Texture_Loader *Loader)
{
  return TextureContainer->SubTexture(To,Loader);
}

CTexture *CTexture::IsTextureLoaded(GString Source,CTextureParameters *Params)
{
 DWORD ID=LookUp.GetIDFor(Source.v_str(),Source.length());
 if(ID>0xFF) return (CTexture*)ID;
 return NULL;
}

u_Texture_Loader *CTexture::GetLoader(GString Source)
{
 u_Texture_Loader *Ret;
 //if (LoaderList.Lookup(Ret,Source)) return Ret;
 Ret=new u_Texture_Loader;
 if(!Ret->LoadFromFile(Source))
 {
	 Print2Console(SYS_TEXTURES,"Load %s - no file found",Source.c_str());
	 delete Ret;Width=0;Height=0;return NULL;
 }
// LoaderList.Push(Source,Ret);
 Width=Ret->Info.Width;
 Height=Ret->Info.Height;
 return Ret;
}

bool CTexture::LoadFrom(u_Texture_Loader *Loader,CTextureParameters *Params)
{
 if(TextureContainer) delete TextureContainer;
 TextureContainer=GetTextureContainer();
 bool res=TextureContainer->Load(Loader,Params);
 if(!res) Print2Console(SYS_TEXTURES,"Load %s - unable to upload",Loader->Sourse.c_str());
 else
 	 Print2Console(SYS_TEXTURES,"Loaded %s",Loader->Sourse.c_str());
 return res;
}

CTextureContainer *GetTextureContainer()
{
 return new CGLTextureContainer;
}

void CTexture::Bind()
{
	if(TextureContainer)
	 TextureContainer->Bind();
}

void CTexture::Disable()
{
	if(TextureContainer)
	 TextureContainer->Disable();
}

DWORD CTexture::GetInnerID()
{
	if(TextureContainer)
	 	 return TextureContainer->GetInnerID();
	else return 0;
}

DWORD CTexture::GetTextureType()
{
    if(TextureContainer)
		 return TextureContainer->GetTextureType();
	else return 0;
}

bool CTexture::HaveAlpha()
{
    if(TextureContainer)
		 return TextureContainer->HaveAlpha();
	else return false; 
}

 u_Texture_Loader * CTexture::GetData(){return ThisLoader;};

 WORD CTexture::GetWidth(){return Width;}
 WORD CTexture::GetHeight(){return Height;}


 
 bool CTexture::Create(CDWORD TType,DWORD FORMAT,cint Width,cint Height,cint Depth,CTextureParameters *Params)
 {
	 if(TextureContainer) delete TextureContainer;
     TextureContainer=GetTextureContainer();
     bool res=TextureContainer->Create(TType,FORMAT,Width,Height,Depth,Params);
	 this->Width=Width;
     this->Height=Height;
	 return res;
 }

 bool CTexture::SetData(LPCVOID Data,int MipMap)
 {
	 if(TextureContainer)
	 {
		 return TextureContainer->SetData(Data,MipMap);
	 }
	 else
		 return false;
 }


namespace Textures
{
    #define MAX_TMU 16

	DWORD MAX_NUM_TMU=4;

	SmartVector <CTexture*> TexturePack;
	DWORD LastTextureID[0xFF];
	struct CTMUInfo
	{
		DWORD ActiveTexture;
		DWORD TMUState;
		CTMUInfo()
		{
			ActiveTexture=TMUState=0;  
			TMUState=0xDEADBEAF;
		}

		DWORD GetTMUState()
		{
			DWORD iTS=0;
			if(TMUState==0xDEADBEAF) TMUState=0;//GL_TEXTURE_2D;
			if(glIsEnabled(GL_TEXTURE_1D))iTS=GL_TEXTURE_1D;
			if(glIsEnabled(GL_TEXTURE_2D))iTS=GL_TEXTURE_2D;
			if(glIsEnabled(GL_TEXTURE_3D))iTS=GL_TEXTURE_3D;
			if(glIsEnabled(GL_TEXTURE_CUBE_MAP))iTS=GL_TEXTURE_CUBE_MAP;
			ASSERT(iTS==TMUState);
			return TMUState;
		}

		DWORD DropTexture()
		{
			DWORD TS=TMUState;
			TMUState=TMU_INACTIVE;
			ActiveTexture=0;
			if(TS==0xDEADBEAF) TS=0;//GL_TEXTURE_2D;

			//check
			DWORD iTS=0;
			if(glIsEnabled(GL_TEXTURE_1D))iTS=GL_TEXTURE_1D;
			if(glIsEnabled(GL_TEXTURE_2D))iTS=GL_TEXTURE_2D;
			if(glIsEnabled(GL_TEXTURE_3D))iTS=GL_TEXTURE_3D;
			if(glIsEnabled(GL_TEXTURE_CUBE_MAP))iTS=GL_TEXTURE_CUBE_MAP;
			ASSERT(iTS==TS);

			return TS;
		}
	};

	TMU_STATE TMUS2OGL(CDWORD TMUs)
	{
		switch(TMUs)
		{
			case 0: return TMU_OFF;
			case GL_TEXTURE_1D: return TMU_TEXTURE1D;
			case GL_TEXTURE_2D: return TMU_TEXTURE2D;
			case GL_TEXTURE_3D: return TMU_TEXTURE3D;
			case GL_TEXTURE_CUBE_MAP:return TMU_TEXTURECUBE;
		}
		return TMU_OFF;
	}

	char GetTRUStateNameBuf[4][128];
	BYTE GetTRUStateNameBufCur=0;
	LPCSTR GetTRUStateName(CDWORD TMUs)
	{
		LPSTR Buf=GetTRUStateNameBuf[GetTRUStateNameBufCur++];
		if(GetTRUStateNameBufCur>3)GetTRUStateNameBufCur=0;
   		//switch(TMUs)
		{
/*			case 0                  : strcpy_s(Buf,128,"OFF");break;
			case 0xDEADBEAF         : strcpy_s(Buf,128,"Activate");break;
			case GL_TEXTURE_1D      : strcpy_s(Buf,128,"1D Texture");break;
			case GL_TEXTURE_2D      : strcpy_s(Buf,128,"2D Texture");break;
			case GL_TEXTURE_3D      : strcpy_s(Buf,128,"3D Texture");break;
			case GL_TEXTURE_CUBE_MAP: strcpy_s(Buf,128,"Cubemap Texture");break;
*/
		}
		return Buf;
	}

	CTMUInfo TMUInfo[MAX_TMU];
	DWORD ActiveTMU=0;

	bool _Enable(CDWORD Type)
	{
#ifndef TEXTURES_DISABLE_WARNING
		if(TMUInfo[ActiveTMU].TMUState && TMUInfo[ActiveTMU].TMUState!=Type)
			Warning2Console(SYS_TEXTURES,"%dTMU - unhandled enable from %s to %s",ActiveTMU,GetTRUStateName(TMUInfo[ActiveTMU].TMUState),GetTRUStateName(Type));
#endif
		TMUInfo[ActiveTMU].TMUState=Type;
		glEnable(Type);
		return true;
	}

	TMU_STATE GetTMUStatus    (CDWORD TMU)
	{
		if(TMU==(DWORD)-1) return TMUS2OGL(TMUInfo[ActiveTMU].TMUState);
		else               return TMUS2OGL(TMUInfo[TMU].TMUState);
	}

	bool  DisableTexturing(TMU_OFF_METHOD MODE)
	{
		DWORD AT=ActiveTMU;
		for(DWORD i=0;i<MAX_NUM_TMU;++i)
		 DisableTexture(i,0);
	    SetActiveTMU(AT);
		glCheckError;
		return false;
	}

	DWORD GetInerTexture  (CDWORD ID)
	{
	  if(!ID)return false;
	  if(TexturePack[ID-1])
	   return TexturePack[ID-1]->GetInnerID();
	  return 0;
	}

	bool IsValidTexture  (CDWORD ID)
	{
	  return GetInerTexture(ID)!=0;
	}

	bool  SetActiveTMU (DWORD TMU)
	{		
		if(ActiveTMU!=TMU)
		{
		  ActiveTMU=TMU;
		  glActiveTextureARB(GL_TEXTURE0_ARB+TMU);
		}
		return TMU<MAX_NUM_TMU;
	}
	void  DisableTexture(int TMU,int Safe)
	{
		const DWORD SavedTMU=ActiveTMU;
		if(TMU==-1)
		{
			TMU=ActiveTMU;
			if(!TMUInfo[TMU].GetTMUState())return;
		}
		else 
		{
			SetActiveTMU(TMU);
			if(!TMUInfo[TMU].GetTMUState())
			{
				if(Safe)SetActiveTMU(SavedTMU);
				return;			
			}
		}
		glDisable(TMUInfo[ActiveTMU].DropTexture());
		glCheckError;
		//TMUInfo[ActiveTMU].TMUState=0xDEADBEAF;
		if(Safe)SetActiveTMU(SavedTMU);
		glCheckError;
		//return true;
	}

	DWORD GetTextureType(CDWORD ID)
	{
		if(!ID) return 0;
		if(TexturePack[ID-1])
			 return TexturePack[ID-1]->GetTextureType();
		else return 0;
	}

	bool  BindTexture  (DWORD ID,int TMU)
	{
        static MakeSectionProfile(texbind,"bind texture");
        Profile(texbind); 
		if(TMU!=-1) SetActiveTMU(TMU);
		if(TMUInfo[ActiveTMU].ActiveTexture==ID)
#ifndef TEXTURES_DISABLE_WARNING
			if(!TMUInfo[ActiveTMU].TMUState==GetTextureType(TMUInfo[ActiveTMU].ActiveTexture))
			{ DisableTexture();
		      Warning2Console(SYS_TEXTURES,"%dTMU - unhandled switch from %s to %s",ActiveTMU,GetTRUStateName(TMUInfo[ActiveTMU].TMUState),GetTextureType(TMUInfo[ActiveTMU].ActiveTexture));
			}
			else
#endif
			 return true;
		if(!ID)return false;
		if(TexturePack[ID-1])
		{
			//_Enable(GetTextureType(TMUInfo[ActiveTMU].ActiveTexture));
			DWORD TType=GetTextureType(ID);
			glCheckError;
			if(TMUInfo[ActiveTMU].TMUState && TMUInfo[ActiveTMU].TMUState!=TType)
				glDisable(TMUInfo[ActiveTMU].DropTexture());
			glCheckError;
			TexturePack[ID-1]->Bind();
			glCheckError;
			TMUInfo[ActiveTMU].ActiveTexture=ID;
			TMUInfo[ActiveTMU].TMUState=TType;
			LastTextureID[ActiveTMU]=ID;
			glCheckError;
			return true;
		}
		return false;
	}

	DWORD LoadTexture  (LPCSTR Name,CTextureParameters *Params)
	{
        if(Name[0]=='0')
        {
          if(strcmp(Name,"0SHUTDOWN")==0)
          {
              //delete avi
           CVideoTexture::ShutDown();
           return true;
          }
        }
		CTexture *Tex=new CTexture();
		DisableTexture(-1,false);
		if(!Tex->Load(Name,Params))
		{
			delete Tex;
			return 0;
		}
		Tex->Disable();
	    TMUInfo[ActiveTMU].DropTexture();
		TexturePack.push_back(Tex);
		return TexturePack.size();
	}

	bool  UnloadTexture(DWORD ID)
	{
		if(!ID)return false;
		if(TexturePack[ID-1]){ delete TexturePack[ID-1];TexturePack[ID-1]=NULL;}
		return true;
	}



    GString GetTextureStat(DWORD MODE)
	{
		static char Buf[0xFF];
//		sprintf_s(Buf,sizeof(Buf),"%d textures loaded",TexturePack.size());
		return Buf;
	}

	DWORD  GenerateTexture (CDWORD TType,DWORD FORMAT,CTextureParameters *Params,cint Width,cint Height,cint Depth)
	{
	   DisableTexture(-1,false);
       CTexture *Tex=new CTexture();
	   //CTMUInfo=TMUInfo[ActiveTMU];
	   if(!Tex->Create(TType,FORMAT,Width,Height,Depth,Params))
	   {
		   delete Tex;
		   return 0;
	   }
	   glCheckError;
	   Tex->Disable();
	   glCheckError;
	   TMUInfo[ActiveTMU].DropTexture();
	   TexturePack.push_back(Tex);
	   return TexturePack.size();
	}
		    
	DWORD   SetTextureData  (CDWORD ID,LPCVOID Data,CDWORD TYPE,int MipMap)
	{
		if(!ID)return false;

		if(TexturePack[ID-1])
		{
			DisableTexture(-1,false);
			glCheckError;
			bool ret= (TexturePack[ID-1])->SetData(Data,MipMap);
			glCheckError;
			(TexturePack[ID-1])->Disable();
			glCheckError;
	        TMUInfo[ActiveTMU].DropTexture();
			return ret;
		}
		return 0;
	}

   bool    HaveAlpha       (CDWORD ID)
   {
      if(!ID)return false;
      if(TexturePack[ID-1])
          return TexturePack[ID-1]->HaveAlpha(); 
      else
      return false;
   }

};