//---------------------------------------------------------------------------


#include "ProjectSpace.h"
#include "VectorBase.h"


bVector3f Math::VectorFromString(LPCSTR String)
{
	GString X[3];
	int ii=0;
	int SL=Istrlen(String);
	for(int i=0;i<SL;)
	{
		char c=String[i];
		if(c==',' || c==' ')
		{ 
			//i++;ii++;
			while(String[i]==' ' || String[i]==','){i++;ii++;}
		}
		else 
		{
			X[ii]+=String[i++];
		}
	}
	return bVector3f(Str2Float(X[0]),Str2Float(X[1]),Str2Float(X[2]));
}

float Math::Str2Float(LPCSTR Str)
{
	return atof(Str);
}
int   Math::Str2Int  (LPCSTR Str)
{
	return  atoi(Str);
}



float Math::AngleDif(const float _A,const float _B)
{
 float A=_A;
 float B=_B;
 //A+ret=B; ret=A-B;
 while(A<=0.0f ) A+=360.0f;while(B<=0.0f ) B+=360.0f;
 while(A>360.0f) A-=360.0f;while(B>360.0f) B-=360.0f;
 float R1=B-A;
 //float R2=R1-360.0f;
 while(R1<-360.0f) R1+=360.0f;//while(R2<-360.0f) R2+=360.0f;
 while(R1> 360.0f) R1-=360.0f;//while(R2> 360.0f) R2-=360.0f;
 if(R1>180.0f)R1=360.0f-R1;if(R1<-180.0f)R1=360.0f+R1;
 //if(R2>180.0f)R2=360.0f-R2;if(R2<-180.0f)R2=360.0f+R2;
 return R1;//MIN(fabs(R1),fabs(R2));
}

bVector3f Math::GetAngleDirrection(bVector3f &src,bVector3f &Target)
{
return v3ONE;
}


//---------------------------------------------------------------------------


