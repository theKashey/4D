#include "ProjectSpace.h"
#include "ProjectGraphic.h"
#include "overlay.h"

#define OVERLAY_INDEX_LEN 6*1000
#define OVERLAY_VERTEX_LEN 4*1000

struct COverVertex
{
	float V[3];
	float T[3];
	void Set(float x,float y,float z,float tx,float ty,float tz=0)
	{
		V[0]= x;V[1]= y;V[2]= z;
		T[0]=tx;T[1]=ty;T[2]=tz;
	}
};

DWORD COverlay::qdef_FONT=0;

#define OQ_QUAD 0
#define OQ_SBOX 1

struct COverQuad
{
	DWORD Mode;
	DWORD Material;
	DWORD Texture;
	DWORD NumVerticles;
	DWORD NumIndexes;
	DWORD VaStart;
	bVector4f Color;
	BlendingTypes Blend;
	DWORD WorkLevel;

	COverVertex *V;

	COverQuad()
	{
		Mode=OQ_QUAD;
		Color.Set(1,1,1,1);
		VaStart=0;
		V=0;
		Blend=BLEND_NULL;
		WorkLevel=0;
		NumIndexes=0;
        NumVerticles=0;
	}

	~COverQuad()
	{
	  //if(V) delete []V;
	}
	void SetMaterial(const DWORD M){Material=M;Texture=0;}
	void SetTexture (const DWORD T){Material=0;Texture=T;}
	void SetColor(const bVector4f &Col){Color=Col;}
	void Alloc(const DWORD NumIndex,const DWORD NumVerts,COverlay *Over)
	{
		static MakeSectionProfile(qA2,"overlay::quadalloc");
		Profile(qA2);
	  NumIndexes=NumIndex;
	  NumVerticles=NumVerts;
	  //V=new COverVertex[NumVerts];
	  V=(COverVertex*)((DWORD)Over->EnterPoint+Over->UsedV*sizeof(COverVertex));
	};


};

void *COverlay::AllocQuad()
{
  Profile(pAddQuad);
  COverQuad *Q=new COverQuad();
  return Q;
}

bool COverlay::IsSkipingFrame()const{ return WorkSkip==0;}
void COverlay::CommitChanges()
{
		if(NumOperations>0)
		{
			static MakeProfile(p1,"Store VBO");
     		Profile(p1);

		    //Store.SetBuffer(NULL,-1);
			Store.SybData(EnterPoint,UsedV*sizeof(COverVertex),0);
		    NumOperations=0;
		}
		WorkSkip=0;
}

void COverlay::UpdateSkiping()
{
	if(WorkSkip)return;
	//double FT=VARS::fvars(VARS::fvars(VARS::fv_FTIME_d_20));
	double T=Time::Time();
	if(T-LastUpdateTime>0.05)
	{
		//clean up
			{
				static MakeProfile(p1,"Clean");
				Profile(p1);
				for(DVector::iterator it=quads.end()-1;it!=quads.begin()-1;--it)
				{
					COverQuad *Q=(COverQuad*)(*it);
					if(Q)
						delete Q;
				}
				quads.clear();
				AvaiableV=OVERLAY_VERTEX_LEN;
				UsedV    =0;
			}
	WorkSkip=1;
	LastUpdateTime=T;
	}
	
}



void COverlay::SetWorkLevel(int L){WorkLevel=L;}

bool COverlay::AddQuad(void *_Q)
{
  if(IsSkipingFrame())return false;
  Profile(pAddQuad);
  static MakeProfile(AQ,"overlay::AddQuad");
  {Profile(AQ);
  COverQuad *Q=(COverQuad*)_Q;
  if(Q->NumVerticles<=AvaiableV)
  {
   Q->VaStart=(UsedV*3);//every 4 verticles add 6 WORD indexes((6/4)*sizeof(WORD)[2])
   //Store.SybData(Q->V,sizeof(COverVertex)*Q->NumVerticles,UsedV*sizeof(COverVertex));
   AvaiableV-=Q->NumVerticles;
   UsedV    +=Q->NumVerticles;
   Q->WorkLevel =WorkLevel;
   NumOperations++;
   quads.push_back((DWORD)Q);
   return true;
  }
  else
  {
   Print2Console(SYS_GRAPHIC,"Overlay: no free quads");
   delete Q;
   return false;
  }
  }
}

#define CALC_WH640 	{scb_dx=World()->Camera()->GetWidth() / 640.0f;scb_dy=World()->Camera()->GetHeight()/ 480.0f;}

COverlay::COverlay(void):pAddOver("overlay::add"),pAddQuad("overlay::vbo")
{
	WorkLevel=0;
	WorkSkip=0;
	LastUpdateTime=Time::GetD()+1;
}

COverlay::~COverlay(void)
{
}

void COverlay::Init  ()
{
    CALC_WH640;
    qdef_FONT=Textures::LoadTexture("font.tga",Textures::CTextureParameters::GET()
                                               .SetFilter(TEX_FILTER_LINEAR)
                                               .ref());
	IndexStore.SetType(VBO_STATIC_INDEX);
	IndexStore.SetSize(2*OVERLAY_INDEX_LEN);
	IndexStore.SetTri();
    {
		WORD *elems=(WORD*)IndexStore.Lock();
		for(int i=0;i<OVERLAY_INDEX_LEN/6;++i)
	    {
  			elems[i*6+0]=i*4+0;
			elems[i*6+1]=i*4+1;
			elems[i*6+2]=i*4+2;
			elems[i*6+3]=i*4+2;
			elems[i*6+4]=i*4+1;
			elems[i*6+5]=i*4+3;
		}
	}
	IndexStore.Unlock();
	Store.SetType(VBO_DYNAMIC_GEOM);
	Store.SetSize(sizeof(COverVertex)*OVERLAY_VERTEX_LEN);
	AvaiableV=OVERLAY_VERTEX_LEN;
	UsedV    =0;
	EnterPoint=new COverVertex[OVERLAY_VERTEX_LEN];
    NumOperations=0;

}

void COverlay::DoSwap()
{
}

void COverlay::Render(bool Lock)
{
	//if(IsRenderSkiping())return;
    static MakeProfile(p0,"COverlay::Render");
	Profile(p0);
    {
	static MakeProfile(p1,"Check");
	CALC_WH640;
	Profile(p1);
    bool F=false;
    for(DVector::Citerator it=quads.begin();it!=quads.end();++it)
    {
       const COverQuad *Q=(const COverQuad*)(*it);
		if(Q)
		 if(Q->WorkLevel==WorkLevel)
             {
               F=true;
               break;
             }
    }
    if(!F)return;
    }

	
	Draw::CRenderVertex RV;

	{
		static MakeProfile(p1,"Prepare VBO");
		Profile(p1);
	RENDER->DisableWrite(LAYER_DEPTH);
	RENDER->EnableWrite(LAYER_COLOR);
	RENDER->CullFace(CULL_NONE);
	RENDER->SetZMode(DSF_DEPTH_OFF);
	

	Draw::CRenderStream StreamOne;
	StreamOne.SetVBOID(&Store);
	StreamOne.StrideSet(*(COverVertex*)1);
	StreamOne.SetVertex(Store.BufferOffset(0));
	StreamOne.SetTex(0,(LPVOID)(Store.BufferOffset(3*4)));
	StreamOne.SetTexCount(0,3);

	RV.SetIndex(&IndexStore);

	RV.AddStream(StreamOne);
	}
	DEF_RENDER *Driver=RENDER;

	{
	static MakeProfile(p1,"Do Render");
	Profile(p1);
	Driver->StartSession();
	bool FirstRender=true;
    DWORD QLen=0;
	for(DVector::iterator it=quads.begin();it!=quads.end();++it)
	{
		COverQuad *Q=(COverQuad*)(*it);
		if(Q)
		 if(Q->WorkLevel==WorkLevel)
		{
		switch(Q->Mode)
		{
		case OQ_QUAD:
			Driver->SetMaterial(Q->Material);
			Driver->SetTexture (Q->Texture);
			Driver->SetIMColor (Q->Color);
			RV.SetIndexOffset  (Q->VaStart);
			IndexStore.SetLen  (Q->NumIndexes);
            if(Q->Blend==BLEND_NULL)
            {
              if(Textures::HaveAlpha(Q->Texture) || Q->Color[3]<=0.99f)
                 Driver->SetBlending(BLEND_ALPHA);
              else
                 Driver->SetBlending(BLEND_OFF);
            }
            else
			 Driver->SetBlending(Q->Blend);
			if(FirstRender)RV.Flush();
			else           RV.FlushI();
			FirstRender=false;
			break;
		case OQ_SBOX:
			break;
		}
		 //delete Q;
		 //*it=NULL;
		}
	}
	Driver->EndSession();
	}


}

#define NewQuad	(COverQuad *)this->AllocQuad()
#define AddElement(a) this->AddQuad((void*)a);
#define NewScissorQuad(x,y,z,w) MakeBox(NewQuad,x,y,z,w)

COverQuad *MakeBox(COverQuad *Q,float x,float y,float z,float w)
{
	Q->Mode=OQ_SBOX;
	Q->Color.Set(x,y,z,w);
	return Q;
}

void COverlay::QuadT     (int Texture, float x, float y, float w, float h, bVector4f Color,float repx, float repy,float offx,float offy,BlendingTypes Blend)
{
	if(IsSkipingFrame())return;
	Profile(pAddOver);
	static MakeSectionProfile(AQ,"overlay::QuadT");
	{
		Profile(AQ);
	COverQuad *Q=NewQuad;
    Q->SetTexture(Texture);
	Q->SetColor(Color);
	Q->Alloc(6,4,this);
	Q->V[0].Set(x  ,y  ,0,offx,offy);
	Q->V[1].Set(x+w,y  ,0,repx,offy);
	Q->V[2].Set(x  ,y+h,0,offx,repy);
	Q->V[3].Set(x+w,y+h,0,repx,repy);
    Q->Blend=Blend;
	Q->SetColor(Color);
	AddElement(Q);

	}
}
void COverlay::Quad      (int shader, float x, float y, float w, float h, bVector4f Color,float repx, float repy,float offx,float offy,BlendingTypes Blend)
{
	if(IsSkipingFrame())return;
	Profile(pAddOver);
	static MakeSectionProfile(AQ,"overlay::Quad");
	{
		Profile(AQ);

	COverQuad *Q=NewQuad;
    Q->SetMaterial(shader);
	Q->SetColor(Color);
	Q->Alloc(6,4,this);
	Q->V[0].Set(x,y,0,offx,offy);
	Q->V[1].Set(x+w,y,0,repx,offy);
	Q->V[2].Set(x,y+h,0,offx,repy);
	Q->V[3].Set(x+w,y+h,0,repx,repy);
    Q->Blend=Blend;
	Q->SetColor(Color);
	AddElement(Q);
	}
}
void COverlay::ScissorBox(float x,float y,float w,float h)
{
	if(IsSkipingFrame())return;
	Profile(pAddOver);
	static MakeSectionProfile(AQ,"overlay::ScissorBox");
	{
		Profile(AQ);

	x*=scb_dx;
	y*=scb_dy;
	w*=scb_dx;
	h*=scb_dy;
	AddElement(NewScissorQuad(x,y,w,h));
	}
}

float GetStringWidth(LPCSTR Str,float size)
{
	float xnext=0;
	const int num_chars=strlen(Str);
    for(int i=0; i<num_chars; ++i)xnext+=8*size;
	return xnext;
}


void COverlay::String    (float x, float y, float size,LPCSTR _Text,...)
{
	if(IsSkipingFrame())return;
	if(_Text==NULL || _Text[0]==0)return;
	Profile(pAddOver);
	static MakeSectionProfile(AQ,"overlay::String");
	{
		Profile(AQ);
	LPCSTR T;
	int numvalidchars=0;
	COverQuad *Q=NewQuad;
   // int num_chars=0;
	LPCSTR Text;
	Text=_Text;
	for(T=_Text;*T;++T) if((*T=='%' && (*(T+1)!='%' && *(T+1)!=0 && *(T+1)!=' ')) || *T=='/')
	{
	DOFORMAT(Text2,_Text);
	Text=Text2;
	break;
	}
	//num_chars=strlen(Text);
    
	Q->SetTexture(qdef_FONT);
	Q->Blend=BLEND_ALPHA;
	
	T=Text;
	{for(; *T; ++T)if(*T!=' ')numvalidchars++;}
	Q->Alloc(numvalidchars*6,numvalidchars*4,this);
	//}
	static const float one256=1/256.0f;
	       const float size8 =size;
	       const float size16=size*2;
	int V=0;
	static const float dx=15.0f/256.0f;
	static const float dy=16.0f/256.0f;
	T=Text;
	for(; *T; ++T)
	{
		const char c=*T;
		float xpos=  (float)((c &15)<<4)*one256;
		float ypos=1-(float)((c >>4)<<4)*one256;
		float xnext=x+size8;
		if(c ==' ')
		{
			x=xnext;
			continue;
		}
		Q->V[V++].Set(x    ,y       ,0,xpos   ,ypos);
		Q->V[V++].Set(xnext,y       ,0,xpos+dx,ypos);
		Q->V[V++].Set(x    ,y+size16,0,xpos   ,ypos-dy);
		Q->V[V++].Set(xnext,y+size16,0,xpos+dx,ypos-dy);

 		x=xnext;
	}
	AddElement(Q);
	}
}

