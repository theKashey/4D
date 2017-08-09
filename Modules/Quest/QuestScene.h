#ifndef _QUEST_SCENE_H_
#define _QUEST_SCENE_H_

#define QD_NONE   0
#define QD_COLOR  1
#define QD_Z      2

struct CQuestQuad
{
	Draw::CVBOElement Data;
	DWORD X,Y;
	DWORD W,H;

	float zNear;
	float zFar;

	DWORD  Material;
	BYTE  QType;

	DWORD glList;

	CQuestQuad();
	~CQuestQuad();
	bool LoadZ(LPCSTR Name);
	bool LoadColor(LPCSTR Name);
	int Render();
	void SetPos(float _X,float _Y){X=_X;Y=_Y;}
	void Drop();
};

#define QO_OBJECT    1
#define QO_ACTOR     2
#define QO_PICABLE   4
//#define QO_DYNAMIC   8
//#define QO_MOVABLE   16
#define QO_CONST     32 
#define QO_VISIBLED  64
#define QO_ACTIVE    128

class CQuestObject
{
   GString      Name;
   BYTE         AlphaKey;
   DWORD        Location;
   int          UseCount;
public:
   CQuestObject()
   {
	 Location=0;
	 UseCount=0;
   }
   CQuestObject(LPCSTR _Name,DWORD Loc)
   {
     Name    =_Name;
	 Location=Loc;
	 UseCount=0;
   }
   void         SetName(LPCSTR N){Name=N;}
   BYTE         GetAlphaKey()const{return AlphaKey;}
   bool         IsStatic   ()const{return (Location & QO_CONST)!=0;}
   void         UseKey     (BYTE Key){AlphaKey=Key;}
   bool         IsYou(const GString &Ref)const{return Name==Ref;}
   bool         IsYouSurface(const GString &Ref)const{return Name==Ref;}
   DWORD        Visibled()const{return Location & QO_VISIBLED;}
   DWORD        Active  ()const{return Location & QO_ACTIVE;}
   LPCSTR       GetName ()const{ return Name;}
   void         Use  (){++UseCount;}
   void         Unuse(){--UseCount;}
   DWORD        GetLocation()const{return Location;}
   void         SetLocation(DWORD L){Location=L;}
   void         SetVisibled(bool V=true){Location=(Location & (~QO_VISIBLED));if(V)Location|=QO_VISIBLED;}
   void         SetStatic(bool V=true){Location=(Location & (~QO_CONST));if(V)Location|=QO_CONST;}
   void         SetActive(bool V=true){Location=(Location & (~QO_ACTIVE));if(V)Location|=QO_ACTIVE;}
   void         Drop()
   {
	   Name.Empty();
   }
};

class CQuestScriptDrive
{
	DWORD   Type;
	GString LineScript;
	GString Name;
};

class CQuestActor:public CQuestObject
{
	Draw::Skin::SkinnedMesh *Mesh;
	bVector3f                Position;
	bVector3f                Orientation;
	bVector3f                Scale;
	DWORD                    Tag;
	WORD                     Drived;
	DWORD                    CurState;
	DWORD                    CurAutomate;
	GString                  ScriptDrive;
	typedef SmartVector<CQuestScriptDrive*> CQuestScriptPack;
	CQuestScriptPack         ScriptStack;
	DWORD                    VectorStackID;
public:
	CQuestActor();
	~CQuestActor();
	void Clear ();
	void ClearScriptStack();
	bool Define(LPCSTR ActorName);
	void SetStackID(DWORD SID);
	bool RenderFront(DWORD Path=0);

	void Drive();
	void Undrive();
};


class CQuestActiveElement
{
	CQuestObject *Obj;
	DWORD         As;
public:
    DWORD         GetAs   ()const{return As;}
    CQuestObject *AsObject(){return Obj;}
    const CQuestObject *AsObject()const{return Obj;}
    CQuestActor  *AsActor (){return (CQuestActor*)Obj;}
    bool          Empty   ()const{return  (As==0);}
    CQuestActiveElement(){As=0;Obj=NULL;}
    CQuestActiveElement(CQuestObject *O,DWORD AS){Obj=O;As=AS;}

    LPCSTR        GetName()const{if(Obj)return Obj->GetName();else return NULL;}
    
    bool Equals(const CQuestActiveElement& AE){return As==AE.GetAs() && Obj==AE.AsObject();}
    int  OnMouseIn   ();
	int  OnMouseOut  ();
	int  OnMouseClick();
	int  OnMouseMove ();
};

class CQuestMap
{
	float *HeightMap;
	bool  *BlockMap;
	bVector4f GroundPlane;

	DWORD W;
	DWORD H;
	bool  TryPosAtGround(bVector3f &Src);
	bool  IsPassable    (const bVector3f &Src);
	float HeightAt	    (const bVector3f &Src);
};



class CQuestScene  : public BlopBaseObject
{
public:
	typedef SmartVector<CQuestQuad  > CQQuads;
    typedef SmartVector<CQuestQuad  > CQDQuads;
    typedef SmartVector<CQuestObject> CQObjects;
    typedef SmartVector<CQuestActor*> CQActors;

	typedef SmartVector<int>  CglLists;
	static CglLists *ObjectAliasName;
private:
    

	DWORD        GUID;
	CQDQuads     ZScene;
	CQQuads      BackScene;
	CQQuads      FrontScene;
	CglLists     RenderList;

	CQObjects    Objects;

	CQActors     Actors;

	CQuestMap    Floor;
    CQuestActor *DrivedActor;


	void RenderZ();
	void RenderBack();
	void RenderModel(const bool ZGrid=false);
	void RenderFront();
	void RenderMisc();
	void DrawModels(const bool ZGrid);
public:
	DWORD        IdPos;
	GString      Name;
    GString      sFileName;
	void SetIDPos(const DWORD ipd){IdPos=ipd;}
	bool Load(LPCSTR FileName);

	CQuestActiveElement GetActiveByColor(const DWORD X,const DWORD Y,const DWORD RGBA);

    virtual void SetRenderKeys();
    virtual void ReadCamInfo(bool Read);
    virtual void Activate();
    virtual void Deactivate();
	virtual void Init(void){};
	virtual void Update(void);
	virtual void Render(void);
    virtual DWORD Tick(float T);
//	virtual OBJECT_TYPE Id(void){return OB_QUEST;}

	void SetFloor(bVector3f *V,bVector3f Nrm);

	bVector3f ModPosByFloor(const float *V);
	void Save();

	CQuestScene();
	~CQuestScene();

	static Draw::CPBuffer     BackBuffer;

    bool ModAlphaForObject(const DWORD I,Draw::Skin::Surface *For);
    bool DefineObject(LPCSTR Name,DWORD Location);
    bool DefineActor (LPCSTR Name,DWORD Location);
	bool DriveActor  (LPCSTR Name);
	const CQuestObject* GetObjectByName(LPCSTR N)const;
	CQuestObject* GetObjectByName(LPCSTR N);

	const CQObjects & AssessObjects()const{return Objects;}
};

class CQuestGraph: public CSceneNode
{
   typedef SmartVector<CQuestScene*> CQScenes;
   typedef SmartVector<DWORD>       CQScenesIds;

   CQScenes    Scenes;
   DWORD	   CurrentScene,LastScene;
public:
   static Draw::Skin::SkinnedMesh *zmesh;
   static float       ReadZ(const int x,const int y);
   static int         FrameSpan;
   static CQuestGraph *Entry;
   static bool IsQuestLoaded();
   static CQuestGraph* MakeEntry();

   CQuestGraph();
   ~CQuestGraph();

   bool    LoadScene(LPCSTR,bool SwitchTo=false);
   bool    ReloadScene(LPCSTR FileName,bool Switch);
   virtual void Init(void);
   virtual void Update(void);
   virtual int  Draw(CNodeHistory *H);
//   virtual OBJECT_TYPE Id(void){return OB_QUEST;}

   CQuestScene* GetCurrentScene();

   bVector3f ModPosition(const float *V);
   static bVector3f AssertPosition(const float *V);

   static void InitLayer(CQuestGraph *G);
};

extern void QluaObjects();

extern void ParseQuestCatalog();

#endif