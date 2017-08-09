#include "ProjectSpace.h"
#include "ProjectGraphic.h"
#include "ProjectParticles.h"
#include "coregraphics.h"

bVector3f qUp;
bVector3f qLeft;
//bVector3f pLM;
//bVector3f pLP;
float     qTMAX;
float     qTMIN;
DWORD     qTAX;
DWORD     qTAY;
float     qTASpeed;

bVector4f defColor;

#define PId2 PI_2

#define DCH //if(IsDisabled((*it)))continue;

void CparticleCPUDrawer::SetTA(const DWORD _TID,const DWORD TX,const DWORD TY,const float TS)
{
		TID=_TID;
		TAX=TX;
		TAY=TY;
		TASpeed=TS;
}
void CparticleCPUDrawer::SetParams(bool Rot,DWORD BT,bool Colored,const float *_DefColor)
{
      Rotate   =Rot;
      BlendType=(BlendingTypes)BT;
      fColored =Colored;
	  if(_DefColor)
       DefColor=_DefColor;
	  else 
	   DefColor.Set(1,1,1,1);
}
void CparticleCPUDrawer::SetTRROT(const float *TR,const float *_Rot)
{
		UseCenter=0;
		if(TR){Center=TR;UseCenter=1;}
		UseRot=0;
		if(_Rot){Rot=_Rot;UseRot=1;}
}


#define _CalcTATS float t0=0,t1=0,t2=0,t3=0;CalcTATS(Particle,t0,t1,t2,t3);
#define _CalcTEX  float t0=0,t1=1,t2=0,t3=1;
#define _CalcTexROT(a) float t[8];CalcRot2(t,Particle.Rotation) 

inline void CalcRot2(float *t,const float R)
{
	const float A=R+PI_4;
    static float s2=0;
	if(!s2) s2=sqrt(2.0f);
    const float cos0 =cos(A)*s2;
	const float sin0 =sin(A)*s2;
    const float cos1 =cos(A+PId2)*s2;
	const float sin1 =sin(A+PId2)*s2;

	t[0]=cos0*0.5+0.5;
	t[1]=sin0*0.5+0.5;
	t[2]=cos1*0.5+0.5;
	t[3]=sin1*0.5+0.5;
	t[4]=1-t[0];
	t[5]=1-t[1];
	t[6]=1-t[2];
	t[7]=1-t[3];

}

#define _CalcTATSROT _CalcTexROT(Particle);//CalcTATS(Particle,t0,t1,t2,t3);                           

inline void CalcTATS(const CParticle &Particle,float &t0,float &t1,float &t2,float &t3)
{
		DWORD tID=Particle.LifeTime*qTASpeed;
		DWORD tMAX=qTAX*qTAY-1;
		while(tID>tMAX)tID-=tMAX;

		DWORD tX=tID % qTAX;
		DWORD tY=tID / qTAY;
	
		t0=qTMIN+qTMAX*tX;
		t1=qTMIN+qTMAX*(tX+1);
		t3=1-(qTMIN+qTMAX*tY);
		t2=1-(qTMIN+qTMAX*(tY+1));
}

//bVector3f pLeft,pUp,pLM,pLP;

/*inline void CalcPos(const CParticle &Particle)
{
		pLeft=qUp*Particle.Size;
		pUp  =qLeft  *Particle.Size;
		pLM  = Particle.Position-pLeft;
        pLP  = Particle.Position+pLeft;
}
*/
#define CalcPos(a)	bVector3f pLeft=qLeft*Particle.Size;bVector3f pUp  =qUp  *Particle.Size;bVector3f pLM = Particle.Position-pLeft;bVector3f pLP = Particle.Position+pLeft;

#define SetPos1 VectorSet3(tp->Position,pLM-pUp);
#define SetPos2 VectorSet3(tp->Position,pLM+pUp);
#define SetPos3 VectorSet3(tp->Position,pLP+pUp);
#define SetPos4 VectorSet3(tp->Position,pLP-pUp);

#define SetTNN1 tp->Tex[0]=t0;tp->Tex[1]=t2;
#define SetTNN2 tp->Tex[0]=t0;tp->Tex[1]=t3;
#define SetTNN3 tp->Tex[0]=t1;tp->Tex[1]=t3;
#define SetTNN4 tp->Tex[0]=t1;tp->Tex[1]=t2;

#define SetRTNN1 tp->Tex[0]=t[0];tp->Tex[1]=t[1];
#define SetRTNN2 tp->Tex[0]=t[2];tp->Tex[1]=t[3];
#define SetRTNN3 tp->Tex[0]=t[4];tp->Tex[1]=t[5];
#define SetRTNN4 tp->Tex[0]=t[6];tp->Tex[1]=t[7];

#define SetCol1 memcpy(tp->Color,Particle.Color,sizeof(tp->Color));{for(int ik=0;ik<4;ik++)tp->Color[ik]*=defColor[ik];}

#define DEFP 		CParticleOut starttp[4];CParticleOut *tp=starttp;
#define DRAWVT     	tp=starttp;for(DWORD i=0;i<4;++i,++tp){ glTexCoord2fv(tp->Tex);glVertex3fv(tp->Position);}
#define DRAWVTC    	tp=starttp;glColor4fv(&tp->Color[0]);for(DWORD i=0;i<4;++i,++tp){ glTexCoord2fv(tp->Tex);glVertex3fv(tp->Position);}

inline DWORD CPUQuadParticlesTA(const CParticle &Particle)
{
	    DCH;
        _CalcTATS(Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetTNN1;++tp;
		SetPos2;SetTNN2;++tp;
		SetPos3;SetTNN3;++tp;
		SetPos4;SetTNN4;++tp;
        DRAWVT
		return 4;
}

inline DWORD CPUQuadParticlesTACol(const CParticle &Particle)
{
	    DCH;
        _CalcTATS(Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetTNN1;SetCol1;++tp;
		SetPos2;SetTNN2;SetCol1;++tp;
		SetPos3;SetTNN3;SetCol1;++tp;
		SetPos4;SetTNN4;SetCol1;++tp;
        DRAWVTC
		return 4;
}


inline DWORD CPUQuadParticlesTAColRot(const CParticle &Particle)
{
	    DCH;
        _CalcTATSROT(Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetRTNN1;SetCol1;++tp;
		SetPos2;SetRTNN2;SetCol1;++tp;
		SetPos3;SetRTNN3;SetCol1;++tp;
		SetPos4;SetRTNN4;SetCol1;++tp;
        DRAWVTC
		return 4;
}

inline DWORD CPUQuadParticlesTARot(const CParticle &Particle)
{
	    DCH;
        _CalcTATSROT(Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetRTNN1;++tp;
		SetPos2;SetRTNN2;++tp;
		SetPos3;SetRTNN3;++tp;
		SetPos4;SetRTNN4;++tp;
        DRAWVT
		return 4;
}

inline DWORD CPUQuadParticles(const CParticle &Particle)
{
	    DCH;
        _CalcTEX (Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetTNN1;++tp;
		SetPos2;SetTNN2;++tp;
		SetPos3;SetTNN3;++tp;
		SetPos4;SetTNN4;++tp;
        DRAWVT
		return 4;
}

inline DWORD CPUQuadParticlesCol(const CParticle &Particle)
{
	    DCH;
        _CalcTEX (Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetTNN1;SetCol1;++tp;
		SetPos2;SetTNN2;SetCol1;++tp;
		SetPos3;SetTNN3;SetCol1;++tp;
		SetPos4;SetTNN4;SetCol1;++tp;
        DRAWVTC
		return 4;
}

inline DWORD CPUQuadParticlesRot(const CParticle &Particle)
{
	    DCH;
        _CalcTexROT (Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetRTNN1;++tp;
		SetPos2;SetRTNN2;++tp;
		SetPos3;SetRTNN3;++tp;
		SetPos4;SetRTNN4;++tp;
        DRAWVT
		return 4;
}

inline DWORD CPUQuadParticlesColRot(const CParticle &Particle)
{
	    DCH;
        _CalcTexROT (Particle);
        CalcPos (Particle);
        DEFP
		SetPos1;SetRTNN1;SetCol1;++tp;
		SetPos2;SetRTNN2;SetCol1;++tp;
		SetPos3;SetRTNN3;SetCol1;++tp;
		SetPos4;SetRTNN4;SetCol1;++tp;
        DRAWVTC
		return 4;
}
#define INDRAWP(a) {for(;P!=endit;++P){const CParticle* RednerP=P;const DWORD Count=a(*RednerP);OutCount+=Count;} }

inline DWORD _CPUQuadParticles(const CParticle &Particle,const bVector3f &_Up,const bVector3f &_Left,float TMAX,float TMIN,const DWORD TAX,const DWORD TAY,const float TASpeed)
{
		//CParticleOut* Stp=0;
//#define TMAX 1.0f
//#define TMIN 0.0f
		//if(IsDisabled((*it)))continue;
	    /*DWORD tID=Particle.LifeTime*TASpeed;
		DWORD tMAX=TAX*TAY-1;
		while(tID>tMAX)tID-=tMAX;

		DWORD tX=tID % TAX;
		DWORD tY=tID / TAY;

			
		//TMIN+=TMAX
		float t0,t1,t2,t3;
		t0=TMIN+TMAX*tX;
		t1=TMIN+TMAX*(tX+1);
		t3=1-(TMIN+TMAX*tY);
		t2=1-(TMIN+TMAX*(tY+1));
		
		//Print2Console(0,"tID:%d %d|%d :%f",tID,tX,tY,t3);
*/
	/*	bVector3f pLeft=qLeft*Particle.Size;
		bVector3f pUp  =qUp  *Particle.Size;
		bVector3f pLM = Particle.Position-pLeft;
        bVector3f pLP = Particle.Position+pLeft;
/**/
	    DCH;
        _CalcTEX (Particle);
        CalcPos (Particle);
        DEFP

	//	CParticleOut starttp[4];
//		CParticleOut *tp=starttp;

		VectorSet3(tp->Position,pLM-pUp);tp->Tex[0]=t0;tp->Tex[1]=t2;++tp;
		VectorSet3(tp->Position,pLM+pUp);tp->Tex[0]=t0;tp->Tex[1]=t3;++tp;
		VectorSet3(tp->Position,pLP+pUp);tp->Tex[0]=t1;tp->Tex[1]=t3;++tp;
		VectorSet3(tp->Position,pLP-pUp);tp->Tex[0]=t1;tp->Tex[1]=t2;++tp;
		tp=starttp;
    	for(DWORD i=0;i<4;++i,++tp)
	    { 
		 glTexCoord2fv(tp->Tex);
		 glVertex3fv(tp->Position);
	    }
		
//#undef TMAX
//#undef TMIN
		return 4;
}


bool CparticleCPUDrawer::Draw (CParticle *Particles,const DWORD Num)
{
	glColorMask(1,1,1,1);
	glDepthMask(0);
	grBLENDER->Apply(BlendType);//BLEND_ADD);
	glColor4f(DefColor[0],DefColor[1],DefColor[2],DefColor[3]);
	defColor=DefColor;
	float TMAX=1.0f/TAX;
    float TMIN=0;
	int OutCount = 0;
	//Textures
	glActiveTexture(GL_TEXTURE3_ARB);glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2_ARB);glDisable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE1_ARB);glDisable(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE0_ARB);glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,TID);

	bVector3f _Up(0,0.1f,0);
 	bVector3f _Left(0.1f,0,0);
	float ViewMatrix[16];

	if(UseCenter)
	{
		glPushMatrix();
		glTranslatef(Center[0],Center[1],Center[2]);
		glRotatef(Rot[0],1,0,0);
		glRotatef(Rot[1],0,1,0);
		glRotatef(Rot[2],0,0,1);
	}

	glGetFloatv( GL_MODELVIEW_MATRIX, ViewMatrix );

	for(int j=0;j<3;++j){
		_Left  [j]=ViewMatrix[j*4+0];
		_Up    [j]=ViewMatrix[j*4+1];
	}
	CParticle *P=Particles;
	const CParticle* endit=P+Num;
	float OneTASpeed=1.0f/TASpeed;

    qUp=_Up;
    qLeft=_Left;
    qTMAX=TMAX;;
    qTMIN=TMIN;
    qTAX =TAX;
    qTAY =TAY;
    qTASpeed=OneTASpeed;

	if(0)
	{
	glBegin(GL_QUADS);
	for(;P!=endit;++P)
	{
		const CParticle* RednerP=P;
        DWORD Count=_CPUQuadParticles(*RednerP,_Up,_Left,TMAX,TMIN,TAX,TAY,OneTASpeed);
		OutCount+=Count;
	}
	glEnd();
	}
    else
    //if(0)
	{
	glBegin(GL_QUADS);
	if(TAX>1 || TAY>1)
	{
		if(fColored)
		{
			if(Rotate)
				INDRAWP(CPUQuadParticlesTAColRot)
			else
				INDRAWP(CPUQuadParticlesTACol)
		}
		else
		{
   		    if(Rotate)
				INDRAWP(CPUQuadParticlesTARot)
			else
				INDRAWP(CPUQuadParticlesTA)
		}

	}
	else
	{
		if(fColored)
		{
			if(Rotate)
				INDRAWP(CPUQuadParticlesColRot)
			else
				INDRAWP(CPUQuadParticlesCol)
		}
		else
		{
   		    if(Rotate)
				INDRAWP(CPUQuadParticlesRot)
			else
				INDRAWP(CPUQuadParticles)
		}

	}

	glEnd();
	}
	if(UseCenter)
	{
		glPopMatrix();
	}

	glColorMask(1,1,1,1);
	glDepthMask(1);
   return OutCount>0;
}