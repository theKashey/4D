/*===========================================================================
    Заголовок: Camera.cpp
    Автор: Kashey
    Информация: Camera operations

===========================================================================*/
#include "ProjectSpace.h"
#include "camera.h"
#include "coregraphics.h"


//#include "Core/Phisics/BasePhisic.h"

//using namespace Phisic;

CObjectID CamID;
const CObjectID& GetCameraID()
{ 
    return CamID;
}

void LuaExport()
{
};

CCamera::CCamera(void):BaseObject(REFC_CORE,REFB_WORLD,"CAMERA")
{
//    SetName("Camera");
	//CamID=thisID;
	syncObject=0;
    Width=Height=1;
	LuaExport();
	EnableChanges(true);
}

CCamera::~CCamera(void)
{
}

const bVector3f& CCamera::GetPosition()
{
// if(syncObject)((CBasePhisic*)syncObject)->GetPos(Position);
 return Position;
}

const bVector3f& CCamera::GetLookAngles()
{
// if(syncObject)((CBasePhisic*)syncObject)->GetPos(Position);
 return LookAt;
}

void CCamera::SetLookAtAdd  (float X,float Y,float Z){if(ChangeIsOn)LookAt.Plus(X,Y,Z);LookAt.NormAngle();};
void CCamera::SetLookAngles (bVector3f &Pos){if(ChangeIsOn)LookAt=Pos;LookAt.NormAngle();};
void CCamera::SetPosAdd     (bVector3f &Pos){if(ChangeIsOn)Position+=Pos;}
void CCamera::SetPosition   (bVector3f &Pos){if(ChangeIsOn)Position =Pos;}

void CCamera::ClearPos()    { Position.SetZero();LookAt.SetZero();}
bool CCamera::EnableChanges (bool OnOff){ChangeIsOn=OnOff;return OnOff;}

void CCamera::TransformAll()
{
    Sync();
	TransformGraphic();
	TransformSound();
}

void CCamera::Sync()
{
	GetPosition();
}

void CCamera::Translate(const bVector3f &TR)
{
  glTranslatef(TR[0],TR[1],TR[2]);
}

void CCamera::Rotate   (const bVector3f &RT)
{
	glRotatef(RT[1],1,0,0);       
    glRotatef(RT[0],0,1,0);
	glRotatef(RT[2],0,0,1);
}

void CCamera::Scale    (const bVector3f &SC)
{
	glScalef(SC[0],SC[1],SC[2]);
}

void CCamera::StoreMatrix()
{
}

void CCamera::SetViewPort()
{
	StartUpInit();
	glViewport(Viewport[0],
			   Viewport[1],
			   Viewport[2],
			   Viewport[3]);
	glScissor(0, 0, GetWidth(), GetHeight());
}

void  CCamera::StartUpInit()
{
	if(lua::getdword("win_updateviewport"))	
	{
	   Width  =lua::getdword("win_width");
	   Height =lua::getdword("win_height");
	   Fov    =lua::getfloat("win_fov");
	   Znear  =lua::getfloat("win_znear");
       Zfar   =lua::getfloat("win_zfar");

	  Viewport[0]=Viewport[1]=0;
	  Viewport[2]=GetWidth ();
	  Viewport[3]=GetHeight();
	  lua::setdword("win_updateviewport",0);
	  lua::setdword("r_winrestart",1);
	}
	else
	{
		//lua::setdword("r_winrestart",0);
	}
}

void CCamera::Init()
{
	
    SetViewPort();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(Fov, WHRatio(), Znear, Zfar);
	glGetFloatv (GL_PROJECTION_MATRIX, Projection);
    glGetDoublev(GL_PROJECTION_MATRIX, dProjection);
	glMatrixMode(GL_MODELVIEW);
	TransformGraphic();
	glGetFloatv (GL_MODELVIEW_MATRIX  , ModelView);
    glGetDoublev(GL_MODELVIEW_MATRIX  , dModelView);
	{
	const float *a=ModelView;
	const float *b=Projection;
	int i, j;
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    MPV[i*4+j] = 	a[i*4+0]*b[0*4+j] +
					    a[i*4+1]*b[1*4+j] +
					    a[i*4+2]*b[2*4+j] +
					    a[i*4+3]*b[3*4+j];
	}
    }
	}
}

const float* CCamera::GetModelView (){return ModelView;}
const float* CCamera::GetProjection(){return Projection;}
const float* CCamera::GetMPV       (){return MPV;}

void CCamera::Select3D()
{
	Init();
};

void CCamera::Select2D()
{
    SetViewPort();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glOrtho( 0, 640, 480, 0, 0, 1.0 );
	glMatrixMode(GL_MODELVIEW);
//	glOrtho( 0, cam->GetWidth(), cam->GetHeight(), 0, 0.0f, 1.0f );


}

void CCamera::TransformGraphic()
{
	glMatrixMode(GL_MODELVIEW);     // activate the transformation matrix
    glLoadIdentity(); 
	glRotatef(LookAt[1],1,0,0);       
    glRotatef(LookAt[0],0,1,0);
	glTranslatef(-Position[0],-Position[1],-Position[2]);
}
void CCamera::TransformSound  ()
{
	bVector3f Pos;
	Pos.Set(-Position[0],-Position[1],-Position[2]);
	Pos*=-1;
	Sound::SetListenerPosition(Pos);
	bVector3f At  =GetMoveShift(1,0,1);
	bVector3f Left=GetMoveShift(1,1,1);
	bVector3f Up  =At.Cross(Left);
	Up.Normalize();
	Sound::SetListenerOrientation(At,Up);
}

bVector3f CCamera::GetMoveShift(BYTE Forward,BYTE SLeft,BYTE AddY)
{
  bVector3f Ret(0,0,0);
  if(Forward==0 && SLeft==0) return Ret;
  bVector3f Dirrection(0,0,0);
  float RK=DegToRad(LookAt[0]);
  float x=cos(RK+PI_2);
  float y=sin(RK+PI_2);
  Dirrection.Set(-x,0,-y);
  if(AddY)
  {
   RK=DegToRad(LookAt[1]);
   x=cos(RK);
   y=sin(RK);
   Dirrection*=fabs(x);
   Dirrection-=bVector3f(0,y,0);
  }
  if(Forward==1)   Ret+=Dirrection;else
  if(Forward==2)   Ret-=Dirrection;
  if(SLeft)
  {
  float RK=DegToRad(LookAt[0]);
  float x=cos(RK);
  float y=sin(RK);
  Dirrection.Set(x,0,y);
  if(SLeft==1)     Ret-=Dirrection;else
                   Ret+=Dirrection;
  }

  Ret.Normalize();
  return Ret; 
}



void CCamera::SetLookAt        (bVector3f &Pos)
{
  bVector3f OldLook=LookAt;
  bVector3f Dir=(Position-Pos); // dirrection to point
  bVector3f OD=Dir;
  float DLen=Dir.Length();
  if(DLen<0.001) return;
  if(Dir.SqrLen()>1)
  {
   Dir.Normalize();
   bVector3f NormalDir=GetLookVector();
   //double	change_model_yaw =(atan2(Dir[0], Dir[2]) * 180.0f / PI);
   //while(change_model_yaw<=0) change_model_yaw +=360.0;
   //while(change_model_yaw>360) change_model_yaw-=360.0;
   double ang[2];
   ang[0]=((atan2(Dir[2], Dir[0])) * 180.0) / PI -90.0;
   ang[1]=(asin(Dir[1]) * 180.0) / PI ;
   if(ang[0]<0)ang[0]=360.0+ang[0];
   //if(abs(ang[0]-LookAt[0])<70.0)
   {
    if(fabs(OD[0])>10 || fabs(OD[2])>10)
    {
     //ang[0]-90.0
     //if(fabs(Math::AngleDif(ang[0],LookAt[0]))>10.0f)ang[0]=LookAt[0]+Math::AngleDif(LookAt[0],ang[0])/10.0f;
     LookAt[0]=0;
     LookAt[0]=ang[0];
    }
   }
   {
    if(fabs(OD[1])>10)
    {
     //if(fabs(Math::AngleDif(ang[1],LookAt[1]))>10.0f)ang[1]=LookAt[1]+Math::AngleDif(LookAt[1],ang[1])/10.0f;
     LookAt[1]=0;
     LookAt[1]=ang[1];//-90.0;
    }
   }
  }
  LookAt.NormAngle();
}

bVector3f CCamera::GetLookVector()
{
	return GetMoveShift(1,0,1);
}
bVector3f CCamera::GetFrustumVector()
{
	return bVector3f(0.1f,0.1f,0.1f);
}

void CCamera::ScreenToClient(const bVector3f &In,bVector3f &Out)
{
	  double tout[3];
      const bool ret=
          gluUnProject(In[0], In[1], In[2],
                       dModelView,
                       dProjection,
                       Viewport,
                       &tout[0], &tout[1], &tout[2])!=0;
      if(ret)
       { V3CPY(Out,tout);}
      else Out.SetZero();
	  //return ret;
}

void CCamera::ClientToScreen(const bVector3f &In,bVector3f &Out)
{
	  double tout[3];
      const bool ret=
          gluProject(In[0], In[1], In[2],
                       dModelView,
                       dProjection,
                       Viewport,
                       &tout[0], &tout[1], &tout[2])!=0;
	  if(ret)
       {V3CPY(Out,tout);}
      else Out.SetZero();
	  //return ret;
}


DWORD CCamera::OnMessage(const CBlopMessage &Message)
{
	bVector3f D;
	switch(Message.GetMsg())
	{
 	 case bCMD_MOVE:
		 D=*((const bVector3f*)Message.GetData());
         float FPS=GetCounter(DC_MAIN_CYCLE);
         FPS*=100;
         SendUpSignal(CObjectIDTwin(Message.GetAddress().From,GetID()),bCMD_SYNC_POS,4*3,(LPVOID)&GetPosition());
		 break;
	}
	return 1;
}

