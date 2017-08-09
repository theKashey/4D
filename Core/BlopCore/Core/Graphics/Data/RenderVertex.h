#pragma once

#define MAX_RENDER_STREAMS 4

#include "VBOElement.h"

class DLLTYPE CRenderStream
{
public:
	CVBOElement *VBOElem;
	int   VBOOFFSET;
    DWORD STRIDE;
	DWORD SIZE;

	//
	DWORD START;     // element start;
	DWORD Vertex;    //vertex pointer
	BYTE  VCount;
	DWORD Vertex2;   //vertex 2 pointer (for rkm)
	BYTE  V2Count;
	DWORD Color;     //color pointer
	BYTE  CCount;
	DWORD Color2;    //color2 pointer
	BYTE  C2Count;
	DWORD Normal;    //normal pointer
	BYTE  NCount;
	DWORD basisx,basisy,basisz; // tangent space
	BYTE  BXCount,BYCount,BZCount;
	DWORD bones,bweigth,weights;// skin mesh
	BYTE  SBCount,SWCount,SBWCount;
	DWORD Tex[32]; // 32 tex coords
	BYTE  TCount[32];
	DWORD NumTexs;

	CRenderStream();
	void SetVBOID(CVBOElement* ID){VBOElem=ID;} 
	void MakeStaticGeom(LPCVOID In,DWORD Size)
	{
		CVBOElement* Elem=new CVBOElement(VBO_STATIC_GEOM);
		Elem->SetBuffer(In,Size*STRIDE);
		SetVBOID(Elem);
	}
	void MakeLocalGeom(LPCVOID In,DWORD Size)
	{
		CVBOElement* Elem=new CVBOElement(VBO_DATA_STORE);
		Elem->SetBuffer(In,Size*STRIDE);
		SetVBOID(Elem);
	}
	
	template<class T>
	void StrideSet(const T& A){STRIDE=SIZE=sizeof(T);}
	void SetStride(const DWORD S){STRIDE=S;}

	DWORD off(LPVOID V){ if(START) return ((DWORD)V)-START;else return(DWORD)V;}

	void SetStart (LPVOID V){ START=(DWORD)V;}
	void SetStart (DWORD  V){ START=(DWORD)V;}
	void SetVertex(LPVOID V){ Vertex=off(V);}
	void SetVertexCount(const BYTE c){VCount=c;}
	void SetColor(LPVOID V){ Color=off(V);}
	void SetColorCount(const BYTE c){CCount=c;}
	void SetColor2(LPVOID V){ Color2=off(V);}
	void SetColor2Count(const BYTE c){C2Count=c;}
	void SetTangent(LPVOID B1,LPVOID B2,LPVOID B3)
	{
     basisx=off(B1);
	 basisy=off(B2);
	 basisz=off(B3);
	}
	void SetTangentCount(const BYTE  B1,const BYTE  B2,const BYTE  B3)
	{
     BXCount=(B1);
	 BYCount=(B2);
	 BZCount=(B3);
	}
	void SetTex(const BYTE ID,LPVOID V)
	{
		NumTexs=MAX(NumTexs,ID);
		Tex[ID]=off(V);
	}
	void SetTexCount(const BYTE ID,const BYTE c)
	{
		TCount[ID]=c;
	}
	void SetSkinBones  (LPVOID V){bones  =off(V);} 
	void SetSkinBonesCount(const BYTE c){};
	void SetSkinWeight (LPVOID V){weights=off(V);}
	void SetSkinWeightCount(const BYTE c){};
	void SetSkinBWeight(LPVOID V){bweigth=off(V);}
	void SetSkinBWeightCount(const BYTE c){};
};

enum RI_DRAW_TYPE
{
 RI_TRIS		=  1 ,
 RI_TRIStrips   =  2 ,
 RI_QUADS	    =  3 ,
 RI_QUADStrips  =  4 ,
 RI_POINTS	    =  5 ,
 RI_LINES	    =  6 ,
 RI_LINEStrip   =  7
};

enum RI_DRAW_FORMAT
{
 RI_WORD		=  2,
 RI_DWORD	    =  4
};

class DLLTYPE CBaseRenderIndex: public CVBOElement
{
	DWORD  Min,Max;
	DWORD  Len;
	BYTE   Type;
	BYTE   Format;
    int    StreamOffset;
public:
	void Clear(){Min=0;Max=0;Len=0;}
	CBaseRenderIndex():CVBOElement()
	{
	 Clear();
	 SetTri();
	 Type=RI_WORD;
     StreamOffset=0;
	}
	~CBaseRenderIndex()
	{
      Clear();
	}
	DWORD GetFormat()const{return Format;}
	DWORD GetLen   ()const{return Len;}
	DWORD GetType  ()const{return Type;}
	DWORD GetStart ()const{return Min;}
	DWORD GetEnd   ()const{return Max;}
    int   GetStreamOffset()const {return StreamOffset;}
	void SetStart  (const DWORD M){Min=M;}
	void SetEnd    (const DWORD M){Max=M;}  
	void SetIType  (const DWORD T){Type=T;}
	void SetLen    (const DWORD L){Len=L;}
	void SetTri    ()       {Format=RI_TRIS;}
	void SetTriS   ()       {Format=RI_TRIStrips;}
	void SetQua    ()       {Format=RI_QUADS;}
	void SetQuaS   ()       {Format=RI_QUADStrips;}
	void SetPoint  ()       {Format=RI_POINTS;}
	void SetLine   ()       {Format=RI_LINES;}
	void SetLineS  ()       {Format=RI_LINEStrip;}
    void SetFormat (DWORD F){Format=F;}
    void SetStreamOffset(int Off){StreamOffset=Off;}
	//void SetPonter(LPCVOID p){ P
	template<class T>
	void SetData(DWORD Off,T *Data,DWORD Len,bool CalcMm,int _Min=-1,int _Max=-1)
	{
		if(sizeof(T)==4)Type=RI_DWORD;else Type=RI_WORD;
		SetSize((Off+Len)*sizeof(T));
		SybData(Data,Len*sizeof(T),Off*sizeof(T));
		if(CalcMm)
		{
			Min=0xFFFFFF;
			Max=0;
			for(DWORD i=0;i<Len;++i)
			{
				Min=MIN(Min,Data[i]);
				Max=MAX(Max,Data[i]);
			}
		}
		if(_Min!=-1 && _Max!=-1)
		{Min=_Min;_Max=_Max;}
	}
};


class DLLTYPE CRenderVertex
{
	DWORD            NumStreams;
	CRenderStream    Streams[MAX_RENDER_STREAMS];
	CBaseRenderIndex*Index;
	int              IndexOffset;
public:
	DWORD GetNumStreams()const{return NumStreams;}
	const CRenderStream &GetStream(const BYTE ID)const{return Streams[ID];}
	CBaseRenderIndex &GetIndex()const{return *Index;}
	bool HaveIndex()const{return Index!=NULL;}
	CRenderStream *ModStream(const BYTE ID){return &Streams[ID];}
	DWORD AddStream(const CRenderStream S){Streams[NumStreams++]=S;return NumStreams-1;}
	 CRenderVertex(void);
	~CRenderVertex(void);

	void SetIndex(CBaseRenderIndex* _Index){Index=_Index;}
	void SetIndexOffset(const int Off)    {IndexOffset=Off;}
	DWORD GetIndexOffset()const{return IndexOffset;}
	void Flush()const;
	void FlushI()const;
};

