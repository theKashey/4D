#ifndef _SKIN_MODEL_H
#define	_SKIN_MODEL_H

#define SKINNEDMESH_MAGIC			('s' | 'm' << 8 | ' ' << 16 | '1' << 24)
#define SKINNEDMESHAMIMATION_MAGIC	('s' | 'm' << 8 | 'a' << 16 | '1' << 24)

namespace Skin
{

enum SkinTagTypes
{
    skintag_END        =0,
	skintag_BASE       =1,
	skintag_surface    =100,
	skintag_animation  =200
};



	class DLLTYPE SkinnedBase
	{
    public:
		struct Bone
		{
			GString Name;
			int parent;
		};

		typedef SmartVector<Bone> CBoneVector;
    protected:
		CBoneVector BaseBones;		// bones
		GString     BaseName;
	public:
		void   SetName(LPCSTR N);
        LPCSTR GetName()const;
		int num_bones()const;
        Bone *GetBoneBegin(){return BaseBones.begin();}
		void Read(DWORD f);
	};

class DLLTYPE SkinnedMeshAnimation //: public FileSystemWrapper 
{
public:
	SkinnedMeshAnimation();
	SkinnedMeshAnimation(LPCSTR name);
	~SkinnedMeshAnimation();
	LPCSTR GetName()const;
	void SetName(LPCSTR S);
	int  Read(DWORD file);

    struct Bone
	{
		int BaseID;
		int LocalID;
		neM4 transform;			// real bone transformation
		neM4 offset;			// offset matrix
		neM4 transform_offset;	// transformation * offset
	};

	struct Frame 
	{
		int       BaseBone;
		bVector3f xyz;		// pivot
		neQ       rot;		// rotation
		neM4      FinalMat;
	};
	float FrameRate()const;
protected:
    SkinnedBase *BaseParent;
	GString      Name;
	GString      BaseName;
	int          NumBones;
    float        AnimationSpeed;

	friend class SkinnedMesh;
	bool  LoadFromFile(LPCSTR name);
	int   LoadFromStream(LPCSTR Datain);
	

	
	typedef SmartVector<Bone  > CBoneVector;
	typedef SmartVector<Frame*> CFrameVector;
	CBoneVector    bones;		// bones
	CFrameVector  frames;
	int num_bones ()const;
	int num_frames()const;
	
	bool AssertBase(SkinnedBase *Base);
    SkinnedMeshAnimation* ValidateBase(SkinnedBase *Base);
	void ReadyAdd();
	bool KillOnce();
};


struct DLLTYPE Surface {
private:
	int     RefCount;
	int     Started;
	DWORD   Material;
	GString Name;			// name
	GString MaterialName;
	BYTE    Vtype;
public:

	//-------------------------
    struct BaseVertexHW
	{
		float xyz[3];
		float texcoord[2];
	};
	struct BaseTangent
	{
		float normal  [3];
		float tangent [3];
		//float normal[3]
	};
	struct BaseBone
	{
		float bones  [4];
		float weights[4];
	};
	struct BaseBone2
	{
		float bones  [2];
		float weights[2];
	};
#define BONE_FUNC DWORD Offbones(){return (DWORD)(&this->bones)-(DWORD)this;}DWORD Offweight(){return (DWORD)(&this->weights)-(DWORD)this;}
    //-------------------------

	struct BaseVT  :public BaseVertexHW,public BaseTangent{};
    struct BaseVB  :public BaseVertexHW,public BaseBone   {BONE_FUNC};
	struct BaseVB2 :public BaseVertexHW,public BaseBone2   {BONE_FUNC};
	struct BaseVTB :public BaseVertexHW,public BaseTangent,public BaseBone{BONE_FUNC};
	struct BaseVTB2:public BaseVertexHW,public BaseTangent,public BaseBone2{BONE_FUNC};

	/**
	typedef SmartVector<BaseVT > *CVT;
	typedef SmartVector<BaseVB > *CVB;
	typedef SmartVector<BaseVTB> *CVTB;
	/**/

	Surface();

	int num_vertex()const;
	void Attach();
	void Detach();
	bool IsFree();

	int Render(SkinnedMesh *Mesh);
	int Read(DWORD file);

	void SetVertexType(DWORD T);
	static DWORD GetVertexTypeSize(DWORD T);
	static DWORD GetBonesOffset   (DWORD T);
	static DWORD GetWeightOffset  (DWORD T);
	static DWORD GetBonesNum      (DWORD T);
	LPCSTR GetName()const;
	const GString& GetGName()const;
	void   SetName(LPCSTR N);
    DWORD  NumBones(){return BoneVector.size();}
protected:
	/*union
	{
      CVT  VT;
	  CVB  VB;
	  CVTB VTB;
	};*/

	DWORD NumVertex,NumTris;
	
	bVector3f min;					// bounds
	bVector3f max;
	bVector3f center;
	float     radius;
	
	Draw::CVBOElement       VertexBuffer;
	SmartVector<DWORD>      VertexData;
	SmartVector<Draw::CBaseRenderIndex>  IndexBuffers;
    SkinnedBase::CBoneVector BoneVector;

    Draw::CRenderVertex     SkinVertex;
    friend class SkinnedMesh ;
};


struct SurfaceBone
{
 DWORD ID;
};

/*
 */
class DLLTYPE SkinnedMesh// : public FileSystemWrapper 
{
public:
	SkinnedMesh(LPCSTR name=NULL,LPCSTR LuaName=NULL);
	~SkinnedMesh();

	// animation
	bool AddAnimation   (SkinnedMeshAnimation *animation);
	bool RemoveAnimation(SkinnedMeshAnimation *animation);
	void RemoveAnimation(const int ID);
	SkinnedMeshAnimation *FindAnimation(LPCSTR Name);

	void setFrame(int Animation,double time,int from = -1,int to = -1);
	void setFrame(int *Animations,int *Surfaces,int numpairs,double time,int from = -1,int to = -1);
	// rendering
	int render(int surface = -1);

	//BONES
    struct Bone {
		//neM4 offset;			// offset matrix
		neQ  resultQ;
		int  BaseID;				// parent bone
        neM4 TRMatrix;
	};
	int num_bones(){return Bones.size();}

    //SURFACES
	int FindSurface(LPCSTR name);	
    int getNumSurfaces()const;
	LPCSTR getSurfaceName(int surface)const;
	void AddSurface(Surface *s);
	void RemoveSurface(const int ID);

	bool Load(LPCSTR name,LPCSTR LuaName=NULL);

    CModelShaderInstrument::GPUBone* GetBone(int i);

    CModelShaderInstrument::GPUBone*   GetGPUBonesFor(Surface *For);


    typedef bool(*InRenderCallback)(const DWORD I,Surface * For);

    void SetBeforeRenderCallback       (InRenderCallback CB);
    void SetAfterRenderCallback        (InRenderCallback CB);
    void SetBeforeSurfaceRenderCallback(InRenderCallback CB);
    void SetAfterSurfaceRenderCallback (InRenderCallback CB);
	bool IsUsable()const;
protected:
    InRenderCallback   BeforeRender,
                       AfterRender,
                       BeforeSurfaceRender,
                       AfterSurfaceRender;
	GString      Name;
    SkinnedBase *MyBase;

	neM4 transform;		// transformation


	
	bVector3f min;			// bound box
	bVector3f max;
	bVector3f center;		// bound sphere
	float     radius;
	
	typedef SmartVector<Surface*             > SurfaceVector;
    typedef SmartVector<DWORD                > SurfaceBones;
    typedef SmartVector<SurfaceBones*        > CSurfaceBones;
	typedef SmartVector<SkinnedMeshAnimation*> AnimationVector;
	typedef SmartVector<Bone                 >  CBones;
	SurfaceVector   surfaces;					// surfaces
    CSurfaceBones   surfacesBones;              // link bones
    SmartVector<CModelShaderInstrument::GPUBone> GPUBones;
	AnimationVector Anims;
	CBones          Bones;

	void ConfirmSurfaceDelete();


	SkinnedMeshAnimation *MainAnimation;
	SkinnedMeshAnimation *CurrentAnimation;
	//int current_animation;

	SkinnedMeshAnimation::Frame *old_frame;
	double current_time;
	double used_time;
	double last_frametime;

	friend class SkinnedMeshAnimation;

    Surface *CurrentSurface;
    Surface *LastBonesFor;
    DWORD    CurrentSurfaceN;

    bool CreateSurfaceBones(SurfaceBones *Bones,Surface *Surface);


    GString MeshName;
};

class DLLTYPE CSkinViewerGraphNode
{
public:
   CSkinViewerGraphNode();
};

};

#endif