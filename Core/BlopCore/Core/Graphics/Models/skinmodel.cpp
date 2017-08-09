#include "projectspace.h"
#include "projectgraphic.h"
#include "skinmodel.h"

#define  EPSILON 1e-6f

using namespace Draw;
using namespace Skin;


typedef SmartVector<SkinnedBase*         >   CVBases;
typedef SmartVector<SkinnedMeshAnimation*>   CVAnims;
typedef SmartVector<Surface*             >   CVSurfaces;
typedef SmartVector<SkinnedMesh*         >   CVMeshes;

static CVBases    VBases;
static CVAnims    VAnims;
static CVSurfaces VSurfaces;
static CVMeshes   VMeshes;

static void BasesAdd(SkinnedBase *M,LPCSTR MeshName)
{
 lua::doVstring("models.bases.%s={}",(LPCSTR)MeshName);
 lua::doVstring("models.bases.%s.Addr=%d",MeshName,(DWORD)M);
 VBases.push_back(M);
 lua::doVstring("models.bases.%s.StackID=%d",MeshName,(DWORD)VBases.size());
}

static void AnimationsAdd(SkinnedMeshAnimation *M,LPCSTR MeshName)
{
 lua::doVstring("models.animations.%s={}",(LPCSTR)MeshName);
 lua::doVstring("models.animations.%s.Addr=%d",MeshName,(DWORD)M);
 VAnims.push_back(M);
 lua::doVstring("models.animations.%s.StackID=%d",MeshName,(DWORD)VAnims.size());
}

static void SurfacesAdd(Surface *M,LPCSTR MeshName)
{
 lua::doVstring("models.surfaces.%s={}",(LPCSTR)MeshName);
 lua::doVstring("models.surfaces.%s.Addr=%d",MeshName,(DWORD)M);
 VSurfaces.push_back(M);
 lua::doVstring("models.surfaces.%s.StackID=%d",MeshName,(DWORD)VSurfaces.size());
}

static void AssemblersAdd(SkinnedMesh *M,LPCSTR MeshName)
{
 lua::doVstring("models.assemblers.%s={}",(LPCSTR)MeshName);
 lua::doVstring("models.assemblers.%s.Addr=%d",MeshName,(DWORD)M);
 lua::doVstring("models.assemblers.%s.A={}",MeshName,(DWORD)M);
 lua::doVstring("models.assemblers.%s.S={}",MeshName,(DWORD)M);
 VMeshes.push_back(M);
 lua::doVstring("models.assemblers.%s.StackID=%d",MeshName,(DWORD)VMeshes.size());
}

//SkinnedBase
LPCSTR SkinnedBase::GetName()const{ return BaseName;}
int    SkinnedBase::num_bones()const{return BaseBones.size();}
void   SkinnedBase::SetName(LPCSTR N){ BaseName=N;}

void   SkinnedBase::Read(DWORD file)
{
	    for(CBoneVector::iterator it=BaseBones.begin();it!=BaseBones.end();++it)
			(*it).Name.Empty();
        int numbones;
		bReadFile(&numbones,sizeof(int),1,file);
		BaseBones.SetSize(numbones);
		for(int i = 0; i < numbones; ++i) {
			char bname[128];
			bReadFile(bname,sizeof(bname),1,file);
			BaseBones[i].Name=bname;
			bReadFile(&BaseBones[i].parent,sizeof(int),1,file);
		}

}



//SkinnedMeshAnimation
SkinnedMeshAnimation::SkinnedMeshAnimation()
{
}

SkinnedMeshAnimation::SkinnedMeshAnimation(LPCSTR name)
{
	LoadFromFile(name);
}

SkinnedMeshAnimation::~SkinnedMeshAnimation() 
{
	if(frames.size()!=0) 
		for(int i = 0; i < num_frames(); ++i) delete [] frames[i];	
}

/*
 */
bool SkinnedMeshAnimation::LoadFromFile(LPCSTR name) 
{	
	return LoadFromStream(NULL)>0;
}

int SkinnedMeshAnimation::LoadFromStream(LPCSTR Datain) 
{
	return false;
};

int SkinnedMeshAnimation::num_bones ()const{return bones.size();}
int SkinnedMeshAnimation::num_frames()const{return frames.size();}

bool SkinnedMeshAnimation::AssertBase(SkinnedBase *Base)
{ return true;
}

LPCSTR SkinnedMeshAnimation::GetName()const
{ return Name;
}

void SkinnedMeshAnimation::SetName(LPCSTR S)
{ Name=S;
}

SkinnedMeshAnimation* SkinnedMeshAnimation::ValidateBase(SkinnedBase *Base)
{
  return this;
}
void SkinnedMeshAnimation::ReadyAdd(){}
bool SkinnedMeshAnimation::KillOnce(){return false;}

float SkinnedMeshAnimation::FrameRate()const{return AnimationSpeed;}

int  SkinnedMeshAnimation::Read(DWORD file)
{
    int numframes;
    
    bReadFile(&AnimationSpeed,sizeof(AnimationSpeed),1,file);
	bReadFile(&numframes,sizeof(int),1,file);
    bReadFile(&NumBones,sizeof(int),1,file);
	{
		char base[128];
		bReadFile(&base,128,1,file);
		BaseName=base;
	}
	bones .SetSize(NumBones);
	frames.SetSize(numframes);
	for(int i = 0; i < numframes; ++i) {
		frames[i] = new Frame[NumBones];
		for(int j = 0; j < NumBones; ++j) 
		{
			SkinnedMeshAnimation::Frame *f = &frames[i][j];
			bReadFile(&f->xyz,12,1,file);
			bReadFile(&f->rot,sizeof(neQ),1,file);
			bReadFile(&f->FinalMat,4*4*4,1,file);
		}
	}
	SkinnedMeshAnimation::Frame *frame = frames[0];
	for(int i = 0; i < NumBones; ++i) {

		neM4 translate;
		translate.SetIdentity();
		translate.SetTranslation(frame[i].xyz) ;
		bones[i].transform = translate* (frame[i].rot.BuildMatrix());//translate(frame[i].xyz) * mat3(frame[i].rot);
		bones[i].offset.SetFastInvert(bones[i].transform);
		bones[i].transform_offset = bones[i].transform * bones[i].offset;
	}

 return 1;
}

/*****************************************************************************/
/*                                                                           */
/* Surface tools                                                             */
/*                                                                           */
/*****************************************************************************/

/*
 */

Surface::Surface()
{
	RefCount =0;
	Started  =0;
	Material =0;
	radius   =0xFFFF;
}

int Surface::num_vertex()const
{
	return NumVertex;
}

void Surface::Attach(){ ++RefCount;}
void Surface::Detach(){ --RefCount;ASSERT(RefCount>=0);}
bool Surface::IsFree(){return RefCount==0;}

void Surface::SetName(LPCSTR N){Name=N;}

void Surface::SetVertexType(DWORD T)
{
	Vtype=T;
}

DWORD Surface::GetVertexTypeSize(DWORD T)
{
	switch(T)
	{
	case 0: return sizeof(BaseVertexHW);//vertex only
	case 1: return sizeof(BaseVT);      //vertex+tan
	case 2: return sizeof(BaseVB);      //vertex+bone
	case 3: return sizeof(BaseVTB);     //vertex+tan+bone
	case 4: return sizeof(BaseVB2);      //vertex+bone2
	case 5: return sizeof(BaseVTB2);     //vertex+tan+bone2
	};
	return 0;
}

DWORD Surface::GetBonesOffset   (DWORD T)
{
#define BBEET(a,b) { a B;return B.b();}
	switch(T)
	{
	case 0: return 0;//vertex only
	case 1: return 0;      //vertex+tan
	case 2: BBEET(BaseVB  ,Offbones);      //vertex+bone
	case 3: BBEET(BaseVTB ,Offbones);     //vertex+tan+bone
	case 4: BBEET(BaseVB2 ,Offbones);      //vertex+bone2
	case 5: BBEET(BaseVTB2,Offbones);     //vertex+tan+bone2
	};
	return 0;
}
DWORD Surface::GetWeightOffset  (DWORD T)
{
	switch(T)
	{
	case 0: return 0;//vertex only
	case 1: return 0;      //vertex+tan
	case 2: BBEET(BaseVB  ,Offweight);      //vertex+bone
	case 3: BBEET(BaseVTB ,Offweight);     //vertex+tan+bone
	case 4: BBEET(BaseVB2 ,Offweight);      //vertex+bone2
	case 5: BBEET(BaseVTB2,Offweight);     //vertex+tan+bone2
	};
#undef BBEET
	return 0;
}
DWORD Surface::GetBonesNum      (DWORD T)
{
	switch(T)
	{
	case 0: return 0;//vertex only
	case 1: return 0;      //vertex+tan
	case 2: return 4;      //vertex+bone
	case 3: return 4;     //vertex+tan+bone
	case 4: return 2;      //vertex+bone2
	case 5: return 2;     //vertex+tan+bone2
	};
	return 0;
}

LPCSTR Surface::GetName()const{return Name;}
const GString &Surface::GetGName()const{return Name;}


SmartVector<int> BoneVector2;

int  Surface::Read(DWORD file)
{
	//read name
	{
		char bname[128];
		bReadFile(bname,sizeof(bname),1,file);
		MaterialName=bname;
	}
	//read type
	DWORD type;
	bReadFile(&type,sizeof(type),1,file);
	//read num verticles
	bReadFile(&NumVertex,sizeof(NumVertex),1,file);
	//read num tris
	bReadFile(&NumTris,sizeof(NumTris),1,file);
	//read num bones
	DWORD numbones;
	bReadFile(&numbones,sizeof(numbones),1,file);
    BoneVector.SetSize(numbones);
	BoneVector2.SetSize(numbones);
    {
    for(DWORD j=0;j<numbones;++j)
    {
		char bname[128];
		bReadFile(bname,sizeof(bname),1,file);
        BoneVector[j].Name=bname;
        bReadFile(&BoneVector[j].parent,sizeof(BoneVector[j].parent),1,file);
        //=-1;//unset
    }
    bReadFile(BoneVector2.begin(),numbones*4,1,file);
    }
	// init verticles
	SetVertexType(type);
	VertexBuffer.SetType(VBO_STATIC_GEOM);
	VertexBuffer.SetSize(NumVertex*GetVertexTypeSize(type));
	VertexData.SetSize(NumVertex*GetVertexTypeSize(type)>>2);
	bReadFile(VertexData.begin(),NumVertex*GetVertexTypeSize(type),1,file);
	/**
	DWORD OFF=0;
	for(DWORD i=0;i<NumVertex;++i)
		{
		 if(i==519)
			 i=i;
         bReadFile(VertexData.begin()+OFF,GetVertexTypeSize(type),1,file);
		 BaseVTB *V=(BaseVTB*)(VertexData.begin()+OFF);
		 if((DWORD)V->tangent[0]!=i)
			 i=i;
		 OFF+=GetVertexTypeSize(type)>>2;
		}
	/**/
	VertexBuffer.SybData(VertexData.begin(),NumVertex*GetVertexTypeSize(type),0);
	//bReadFile(VertexBuffer.Lock(),NumVertex*GetVertexTypeSize(type),1,file);VertexBuffer.Unlock();

	//read num tri streams
	DWORD numtristreams;
    bReadFile(&numtristreams,sizeof(numtristreams),1,file);
	IndexBuffers.SetSize(numtristreams);

    for(DWORD i=0;i<numtristreams;++i)
	{
      DWORD vertoffset;
	  DWORD indmax;
	  DWORD indmin;
	  DWORD indtype;
	  DWORD indsize;
	  DWORD format;
	  bReadFile(&vertoffset,sizeof(vertoffset),1,file);
	  bReadFile(&indmax    ,sizeof(indmax    ),1,file);
	  bReadFile(&indmin    ,sizeof(indmin    ),1,file);
	  bReadFile(&indtype   ,sizeof(indtype   ),1,file);
	  bReadFile(&indsize   ,sizeof(indsize   ),1,file);
	  bReadFile(&format    ,sizeof(format    ),1,file);

      IndexBuffers[i].SetType  (VBO_STATIC_INDEX);
	  IndexBuffers[i].SetSize  (indsize*indtype);
      IndexBuffers[i].SetLen   (indsize);
	  IndexBuffers[i].SetStart (indmin);
	  IndexBuffers[i].SetEnd   (indmax);
	  IndexBuffers[i].SetIType (indtype);
	  IndexBuffers[i].SetFormat(format);
      IndexBuffers[i].SetStreamOffset(vertoffset);
	  //if(NEED_CPU_SKIN)
	  {
	   //CPUSkin.SetSize(indsize*indtype);
       bReadFile(IndexBuffers[i].Lock(),indsize*indtype,1,file);
	  }
      IndexBuffers[i].Unlock();     
	}
  //create render stream...

	Draw::CRenderStream StreamOne;
	StreamOne.SetVBOID(&VertexBuffer);
    StreamOne.SetStride(GetVertexTypeSize(type));
	StreamOne.SetVertex(0);//(LPVOID)(5*4));
	StreamOne.SetTex(0,(LPVOID)(3*4));
	StreamOne.SetTexCount(0,2);
	StreamOne.SetSkinBones      ((LPVOID)GetBonesOffset (type));
	StreamOne.SetSkinBonesCount (        GetBonesNum    (type));
	StreamOne.SetSkinWeight     ((LPVOID)GetWeightOffset(type));
	StreamOne.SetSkinWeightCount(        GetBonesNum    (type));

	SkinVertex.SetIndex(&IndexBuffers[0]);

	SkinVertex.AddStream(StreamOne);

  return true;
};


SkinnedMesh::Bone *MYB;

int Surface::Render(SkinnedMesh *Mesh)
{
 //flow bones to GPU
 static MakeSectionProfile(pr1,"SKIN::Compile");
  {
   Profile(pr1);
 DWORD ND=NumBones();
 cgShader *sh=Draw::GetCurrentVertexShader();
 if(ND)
 {
   CModelShaderInstrument*SKIN=NULL;
   if(sh)
       SKIN=(CModelShaderInstrument*)sh->Instrument(CGI_SKIN);
   if(!SKIN)
   {
     Error2Console(SYS_GRAPHIC,"Unable to skin model - current shader in not skin");
     //return 0;
   }
   else
   {
    SKIN->StartTransform();
    CModelShaderInstrument::GPUBone *B=Mesh->GetGPUBonesFor(this);
	 {
      CModelShaderInstrument::GPUBone B0;
	  neM4 M;M.SetIdentity();
	  B0.Matrix.SetTranspose(M);
	  SKIN->AddBone(0,B0);
	 }
	BaseVTB *V=(BaseVTB *)VertexData.begin();
#if 0
    for(DWORD ki=0;ki<NumVertex;++ki,++V)
	{
	 bVector3f xyz;
	 bVector3f X=V->xyz;
	 for(WORD k=0;k<4;++k)
		 {
		 if(V->weights[k])
			 {
		     if(V->bones[k])
			 {  
			   xyz+=B[(DWORD)V->bones[k]-1].Matrix*X*V->weights[k];
			 }
		     else 
			 xyz+=M*X*V->weights[k];
			 }else break;
		 }
     //memcpy(V->normal,xyz,12);
     V->normal[0]=xyz[1];
	 V->normal[1]=xyz[2];
     V->normal[2]=xyz[0];	
	}
    VertexBuffer.SybData(VertexData.begin(),NumVertex*SkinVertex.GetStream(0).STRIDE,0);
#endif
    for(DWORD i=0;i<ND;++i,++B)
    {
	
        SKIN->AddBone(i+1,*B);
    }
    
    SKIN->EndTransform(); 
   }
 }
  }
 RENDER->FlushRV(&SkinVertex);
 return 0;
}

/*****************************************************************************/
/*                                                                           */
/* SkinnedMesh                                                               */
/*                                                                           */
/*****************************************************************************/

/*
 */
SkinnedMesh::SkinnedMesh(LPCSTR name,LPCSTR LuaName) 
{
	transform.SetIdentity();
	min.SetZero();
	max.SetZero();
	center.SetZero();
	radius            = 0;
	MainAnimation     = NULL;
	CurrentAnimation  = NULL;
//	current_animation = 0;
    MyBase            = NULL;
	current_time      = 0.0;
	last_frametime    = 0.0;
	used_time         = current_time;
	old_frame         = NULL;
    SetBeforeRenderCallback       (NULL);
    SetBeforeSurfaceRenderCallback(NULL);
    SetAfterRenderCallback        (NULL);
    SetAfterSurfaceRenderCallback (NULL);
	if(!LuaName) LuaName=name;
	if(name)
	 Load(name,LuaName);
}                 

SkinnedMesh::~SkinnedMesh() 
{
	for(int i = 0; i < (int)Anims   .size(); ++i)	RemoveAnimation(i);
	for(int i = 0; i < (int)surfaces.size(); ++i)	RemoveSurface(i);
	ConfirmSurfaceDelete();
	delete MainAnimation;
}

void SkinnedMesh::SetBeforeRenderCallback       (InRenderCallback CB){BeforeRender       =CB;}
void SkinnedMesh::SetAfterRenderCallback        (InRenderCallback CB){AfterRender        =CB;}
void SkinnedMesh::SetBeforeSurfaceRenderCallback(InRenderCallback CB){BeforeSurfaceRender=CB;}
void SkinnedMesh::SetAfterSurfaceRenderCallback (InRenderCallback CB){AfterSurfaceRender =CB;}

/*****************************************************************************/
/*                                                                           */
/* animation                                                                 */
/*                                                                           */
/*****************************************************************************/

/*
 */
bool SkinnedMesh::AddAnimation(SkinnedMeshAnimation *animation) 
{
	if(!animation)return false;
	if(!animation->AssertBase(MyBase))
    {
		Error2Console(SYS_GRAPHIC,"SkinnedMesh[%s]::addAnimation(): base[%s] not assepted by animation[%s]",(LPCSTR)Name,MyBase->GetName(),animation->GetName());
		return false;
	}
	animation->ReadyAdd();
	SkinnedMeshAnimation *Anim=animation->ValidateBase(MyBase);
	Anims.push_back(Anim);
	if(Anims.size()==1)
     if(MainAnimation==NULL)
	 {
		 MainAnimation=Anim;
		 CurrentAnimation=Anim;
	 }
	if(animation->KillOnce())
		delete animation ;
	return true;
}



void SkinnedMesh::setFrame(int animation,double time,int from,int to) {
    static MakeSectionProfile(PsmshFrame,"SKIN::FRAME"); 
	Profile(PsmshFrame);
	float Otime=current_time;
	current_time=time;
	//time=
    //not faster that 20 FPS
   if(Otime!=0 && fabs(current_time-used_time)<0.05/**VARS::fvars(VARS::fv_FTIME_d_20)/**/) return;
   double TH=Time::Time();
   if(fabs(TH - last_frametime)<50)return;
   last_frametime=TH;
	/////////////////////////////////
	// set bones transformatons
	/////////////////////////////////
	SkinnedMeshAnimation *a = NULL;
	// select animation
	if(animation == 0)
	{ a = CurrentAnimation;
	  if(!a) a=MainAnimation;
	}
	else if(animation <= (int)Anims.size()) a = Anims[animation - 1];
	else {
		Error2Console(SYS_GRAPHIC,"SkinnedMesh::setFrame(): bad animation \"%d\"",animation);
		return;
	}
	if(!a)return;
	CurrentAnimation=a;
	if(from < 0) from = 1;
	if(to < 0) to = a->num_frames();
	
	SkinnedMeshAnimation::Frame *frame_0 = NULL;
	SkinnedMeshAnimation::Frame *frame_1 = NULL;
	float k = 0.0f;

	
	/**
	if(time < 1.0f) {
		frame_0 = old_frame;
		if(frame_0 == NULL) frame_0 = a->frames[from];
		frame_1 = a->frames[from + 1];
		k = time;
	} else {
		frame_0 = a->frames[((int)time - 1) % (to - from) + from];
		frame_1 = a->frames[((int)time) % (to - from) + from];
		k = time - (int)time;
		
		if(k < 0.5f) old_frame = frame_0;
		else old_frame = frame_1;
	}
	/**/

	while(time>=a->num_frames()-1) 
	 time-=a->num_frames();
    frame_0 = a->frames[((int)(time    )) ];
	frame_1 = a->frames[((int)(time + 1)) ];
	k       = time - (int)time;
	/**/
	// calculate matrixes
   // k=0;
	const float _frame=k;
    const float _oframe=1.0f-k;
	for(int i = 0; i < num_bones(); ++i) 
	{
		neQ rot;
		rot.slerp(frame_0[i].rot,frame_1[i].rot,k); 
		neM4 translate;
		translate.SetIdentity();
		translate.SetTranslation(frame_0[i].xyz * _oframe + frame_1[i].xyz * _frame); 
		neM4 transform = translate * rot.BuildMatrix(); 
		neM4 transform_offset = transform * a->bones[i].offset;
		Bones[i].resultQ.SetupFromMatrix(transform_offset);
        //Bones[i].offset=transform_offset;
       // Bones[i].TRMatrix.SetTranspose(transform_offset); 
		neM4 Tmp;
		for(WORD y=0;y<4;++y)Tmp.M[0][y]=transform_offset.M[1][y];
		for(WORD y=0;y<4;++y)Tmp.M[1][y]=transform_offset.M[2][y];
		for(WORD y=0;y<4;++y)Tmp.M[2][y]=transform_offset.M[0][y];
		for(WORD y=0;y<4;++y)Tmp.M[3][y]=transform_offset.M[3][y];
//		bVector3f X1(V->xyz[2],V->xyz[0],V->xyz[1]);

		Bones[i].TRMatrix=Tmp;
		
	}

    //MYB=Bones.begin();
	
	// save current animation and time
	//current_animation = animation;
	used_time=current_time;
    LastBonesFor=NULL;
}


int SkinnedMesh::render(int surface)
{
  static MakeSectionProfile(PsmshDRAW,"SKIN::DRAW"); 
  Profile(PsmshDRAW);
  if(BeforeRender)if(!BeforeRender(0,0))return 0;
  setFrame(0,current_time+VARS::fvars(VARS::fv_FRAMETIME)*CurrentAnimation->FrameRate()*(lua::getfloat("models._ANIMFACTOR")+1));
  int num_triangles = 0;

  float ON255=(float)(1.0/0xFF);

  Draw::cgShader *sh=Draw::GetCurrentVertexShader();
  RENDER->StartSession();
  for(int i = 0; i < (int)surfaces.size(); ++i)
  {
		Surface *s = surfaces[i];
        CurrentSurface=s;
        CurrentSurfaceN=i;
		bool DoDraw=true;
        if(BeforeSurfaceRender)
         DoDraw=BeforeSurfaceRender(i,s);
		if(DoDraw)
          {
		   num_triangles+=s->Render(this);
           if(AfterSurfaceRender)AfterSurfaceRender(i,s);
          }
  }
  RENDER->EndSession();
  if(AfterRender)AfterRender(0,0);
  return num_triangles;

}

int SkinnedMesh::getNumSurfaces()const { return (int)surfaces.size(); }
LPCSTR SkinnedMesh::getSurfaceName(int surface)const  { return surfaces[surface]->GetName(); }


CModelShaderInstrument::GPUBone*   SkinnedMesh::GetGPUBonesFor(Surface *For)
{
   int index=CurrentSurfaceN;
   if(For!=CurrentSurface || LastBonesFor!=For)
   {
    
    if(For!=CurrentSurface)
    {//find...
      for(int i = 0; i < (int)surfaces.size(); ++i)
          if(surfaces[i]==For){index=i;break;}
      if(surfaces[index]!=For) return NULL;
    }
    GPUBones.SetSize(For->NumBones(),false);
    DWORD NB=For->NumBones();
    CModelShaderInstrument::GPUBone* GB=GPUBones.begin();
    SurfaceBones* SB=surfacesBones[index];
    for(DWORD i=0;i<NB;++i,++GB)
    {
     Bone &B=Bones[(*SB)[i]];
     GB->Q=B.resultQ;
     GB->Translate.Set(B.TRMatrix.M[0][3],
                       B.TRMatrix.M[1][3],
                       B.TRMatrix.M[2][3]);
	 GB->Matrix.SetTranspose(B.TRMatrix);
    }
    LastBonesFor=For;
   }
   return GPUBones.begin();
   
}

bool SkinnedMesh::CreateSurfaceBones(SurfaceBones *Bones,Surface *Surface)
{
  DWORD NS=Surface->NumBones();
  DWORD NB=MyBase->num_bones();
  Bones->clear();
  const SkinnedBase::Bone *SB=Surface->BoneVector.begin();
  for(DWORD i=0;i<NS;++i,++SB)
  {
    SkinnedBase::Bone* B=MyBase->GetBoneBegin();
    bool f=false;
    for(DWORD j=0;j<NB;++j,++B)
        if((*SB).Name==B->Name)
            {
             Bones->push_back(j);
			 if((*Bones)[i]!=BoneVector2[i])
				 i=i;
             f=true;
             break;
            }
    if(!f)
    {
     Error2Console(SYS_GRAPHIC,"%s - unable to add surface\n.No bone %s in base",(LPCSTR)MeshName,(LPCSTR)(*SB).Name);
     return false;
    }
  }
  return true;
}


void SkinnedMesh::AddSurface(Surface *s)
{
	if(!s) return;
    SurfaceBones *SB=new SurfaceBones(s->NumBones());
    if(CreateSurfaceBones(SB,s))
    {
	  s->Attach();
	  surfaces     .push_back(s);
      surfacesBones.push_back(SB);
    }
    else delete SB;
}
void SkinnedMesh::RemoveSurface(const int ID)
{
	Surface *s=surfaces[ID];
	if(!s) return;
	s->Detach();
	if(s->IsFree())delete s;
	surfaces[ID]=NULL;
}
void SkinnedMesh::RemoveAnimation(const int ID)
{
}

void SkinnedMesh::ConfirmSurfaceDelete()
{
	for(int i=0;i<surfaces.size();++i)
	{
		if(surfaces[i]==NULL){surfaces.erase(i);--i;}
	}
}


SkinnedBase* GetSkinBase(LPCSTR Name,SkinnedMesh *Mesh,DWORD file,SkinnedBase *oldBase)
{
  if(oldBase) delete oldBase;
  oldBase=new SkinnedBase();
  oldBase->SetName(Name);
  oldBase->Read(file);
  
  return oldBase;
}

Surface *GetSkinSurface(LPCSTR Name,SkinnedMesh *Mesh,DWORD file)
{
	Surface *s=new Surface();
	s->SetName(Name);
	if(!s->Read(file))
	{
		delete s;
		return NULL;
	}
	else
	return s;
};

SkinnedMeshAnimation *GetSkinAnimation(LPCSTR Name,SkinnedMesh *Mesh,DWORD file)
{
    SkinnedMeshAnimation *AN=new SkinnedMeshAnimation();
	AN->SetName(Name);
	if(!AN->Read(file))
	{
		delete AN;
		return NULL;
	}
	else
	return AN;
}

CModelShaderInstrument::GPUBone* SkinnedMesh::GetBone(int i)
{
 const Bone &B=Bones[i];
 static CModelShaderInstrument::GPUBone GB;
 GB.Q=B.resultQ;
 //M[3][0] = V.n.X; M[3][1] = V.n.Y; M[3][2] = V.n.Z;
/* GB.Translate.Set(B.offset.M[3][0],
                  B.offset.M[3][1],
                  B.offset.M[3][2]);
*/
 return &GB;
}

bool SkinnedMesh::IsUsable()const
{
	return surfaces.size();
}

/*
 */
bool SkinnedMesh::Load(LPCSTR name,LPCSTR LuaName) 
{
 //open file
 static bool INITPATH=0;
 if(INITPATH==0)
 {
   lua::dostring("models={}");
   lua::dostring("models.assemblers={}");
   lua::dostring("models.surfaces={}");
   lua::dostring("models.animations={}");
   lua::dostring("models.bases={}");
   INITPATH=1;
 }
 GString tmpLuaName;
 if(!LuaName){tmpLuaName=name;LuaName=tmpLuaName.v_str();}

 
 {
  
  int NameLen=strlen(LuaName);
  MeshName=LuaName;
  for(int i=0;i<NameLen;++i)
   if(LuaName[i]=='/' || LuaName[i]=='\\') 
    MeshName=LuaName+(i+1);
  if(MeshName.GetAt(-4)=='.')
    MeshName.SetAt(-4,0);
 }
 DWORD f=bOpenFile(name,"rb");
 if(!f) return false;

 //read header
 char HEAD[8];//HEAD[3]=0;
 bReadFile(HEAD,sizeof(HEAD),1,f);
 if(strcmp(HEAD,"fSM1"))
 {
	 Error2Console(SYS_GRAPHIC,"Skin[%s]:this file is not a skin model file",name);
	 bCloseFile(f);
 }
 AssemblersAdd(this,MeshName);
 DWORD NS=1;
 DWORD NA=1;
 while(1)
 {
	 DWORD tagType;
     DWORD fp=bTellFile(f);
	 if(!bReadFile(&tagType,sizeof(tagType),1,f))break;
     int LenOff;
     if(!bReadFile(&LenOff,sizeof(LenOff),1,f))break;
     char bname[128];
	 if(!bReadFile(&bname,sizeof(bname),1,f))break;
     bool EXIT=false;
	 switch(tagType)
	 {
       case skintag_END:
           EXIT=true;
           break;
	   case skintag_BASE:
			MyBase = GetSkinBase(MeshName+"_"+bname,this,f,MyBase);
            BasesAdd(MyBase,MeshName+"_"+bname);
            lua::doVstring("models.assemblers.%s.Base=\"%s\"",
                           (LPCSTR)MeshName,(LPCSTR)(MeshName+"_"+bname));
            Bones.SetSize(MyBase->num_bones());
		   break;
	   case skintag_surface:
        {
            Surface *S=GetSkinSurface(MeshName+"_"+bname,this,f);
			AddSurface(S);
            SurfacesAdd(S,MeshName+"_"+bname);
            lua::doVstring("models.assemblers.%s.S[%d]=\"%s\"",
             (LPCSTR)MeshName,NS,(LPCSTR)(MeshName+"_"+bname));
            ++NS;
		   break;
        }
	   case skintag_animation:
        {
            SkinnedMeshAnimation *A=GetSkinAnimation(MeshName+"_"+bname,this,f);
            AnimationsAdd(A,MeshName+"_"+bname);
            lua::doVstring("models.assemblers.%s.A[%d]=\"%s\"",
             (LPCSTR)MeshName,NA,(LPCSTR)(MeshName+"_"+bname));
            ++NA;
			AddAnimation(A);
		   break;
        }
	 };
     if(EXIT)break;
 };

 bCloseFile(f);

 return true;
}


CSkinViewerGraphNode DefSkin;

LUAFUNC(smConvertBBE2SFM)
{
 GString FileFrom=lua::_tostring(L,1);
 GString FileTo=lua::_tostring(L,1);
 lua::dostring("core_AddModule(\"utils\\bbo2fsm.dll\"");
 lua::doVstring("utils.bbo2fsm(\"%s\",\"%s\",1,4);",(LPCSTR)FileFrom,(LPCSTR)FileTo);
 return 0;
}

LUAFUNC(smConvertBBE2SFMDialog)
{
 lua::dofile("scripts/utils/convert/bbe2fsmdiag.lua");
 lua::doVstring("OpenD(%d,%d);OpenD=nil;",(DWORD)lua::_tonumber(L,1),(DWORD)lua::_tonumber(L,2));
 return 0;
}

class CSkinViewer2Lua:public lua::LuaObject
{
protected:
    void DoExport()
    {
      Print2Console(0,"ok CSkinViewer2Lua working...");
      lua::luafunction("bbe2fsm" ,smConvertBBE2SFM      ,"utils.convert");
      lua::luafunction("bbe2fsmD",smConvertBBE2SFMDialog,"utils.convert");
    }
public:
        CSkinViewer2Lua():lua::LuaObject(2){};
};

CSkinViewerGraphNode::CSkinViewerGraphNode()
{
  CSkinViewer2Lua *Skin2Lua=new CSkinViewer2Lua();
  //Skin2Lua->SetStartLevel(2);
}