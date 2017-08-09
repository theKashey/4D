/*===========================================================================
    Заголовок: gui_draw.cpp
    Автор: kashey
    Информация: графическая база
===========================================================================*/
#include "stdafx.h"
#include "GUI.h"
#include "ProjectGraphic.h"

class gr_BASE
{
public:
	virtual void render(){};
	virtual ~gr_BASE(){}
};

bVector4b FloatColor2Byte(const bVector4f &Color)
{
  bVector4b Out;
  if(Color.x>1) Out.x=0xFF;else Out.x=0xFF*Color.x;
  if(Color.y>1) Out.y=0xFF;else Out.y=0xFF*Color.y;
  if(Color.z>1) Out.z=0xFF;else Out.z=0xFF*Color.z;
  if(Color.w>1) Out.w=0xFF;else Out.w=0xFF*Color.w;
  return Out;
}




class gr_RECT:public gr_BASE
{
public:
	bVector3f Start,Box,Rep,Off;
	DWORD     Texture;
    DWORD     Blend;
	bVector4f Color;
	void Set(const bVector3f &_Pos,const bVector3f &_Size,const DWORD _Texture,const bVector4f &_Color,const bVector3f &_Rep,const bVector3f &_Off,DWORD inBlend=BLEND_NULL)
	{
		Start  =_Pos;
		Box    =_Size;
		Texture=_Texture;
		Color  =_Color;//FloatColor2Byte(_Color);
		Rep    =_Rep;
		Off    =_Off;
        Blend  =inBlend;
	}
	void render()
	{
		CGenericGraphic::GetOverlay()->QuadT(Texture,Start[0],Start[1],Box[0],Box[1],Color,Rep[0]+Off[0],Rep[1]+Off[1],Off[0],Off[1],(BlendingTypes)Blend);
	}
};

SmartVector<gr_RECT*> gRECTS;
int NumRects=0;
gr_RECT*GetRect()
{
	if(gRECTS.size()<NumRects+1)
	{
      gr_RECT *RET=new gr_RECT;
      gRECTS.push_back(RET);
	  return RET;
	}
	else 
	return gRECTS[NumRects++]; 
}


class gr_TEXT:public gr_BASE
{
public:
	bVector3f Pos;
	bVector3f Size;
	GString   Text;
	DWORD     Font;
	bVector4f Color;
	float     FontHeight; 
	void render()
	{
	   //Font=ga.data.materials.font;
		CGenericGraphic::GetOverlay()->String(Pos[0],Pos[1],FontHeight,Text);
	}
	void Set(const bVector3f &_Pos,const bVector3f &_Size,const BYTE _Aling,LPCSTR _Text,const DWORD _Font,const bVector4f &_Color,const float _FontHeight)
	{
		Pos  =_Pos;
		Size =_Size;
		Text =_Text;
		Font =_Font;
		Color=_Color;
		FontHeight=_FontHeight*8;
	}

	~gr_TEXT()
	{
		Pos.Set(0,0,0);
	}

};

SmartVector<gr_TEXT*> gTEXT;
int NumTEXT=0;
gr_TEXT*GetTEXT()
{
	if(gTEXT.size()<NumTEXT+1)
	{
      gr_TEXT *RET=new gr_TEXT;
      gTEXT.push_back(RET);
	  return RET;
	}
	else 
	return gTEXT[NumTEXT++]; 
}

class gr_MODEL:public gr_BASE
{
public:
	bVector3f Pos;
	bVector3f Scale;
	bVector3f Rotate;
	bVector3f Translate;
};

class gr_CAMERA:public gr_BASE
{
public:
	bVector3f Pos;
	bVector3f Size;
	bVector3f CamTo;
	bVector3f CamAt;
};

bVector4f LastScissor;



class gr_SCISSOR:public gr_BASE
{
   bVector4f Box;
public:
   void Set(float x,float y,float w,float h)
   {
	   Box.Set(x,y,w,h);
   }
   void render()
   {
	   if(LastScissor[0]!=Box[0] ||
	      LastScissor[1]!=Box[1] ||
		  LastScissor[2]!=Box[2] ||
		  LastScissor[3]!=Box[3])
	   {
    	CGenericGraphic::GetOverlay()->ScissorBox(Box[0],Box[1],Box[2],Box[3]);
        LastScissor=Box;
	   }
   }
};

SmartVector<gr_SCISSOR*> gSCISS;
int NumSCISS=0;
gr_SCISSOR*GetSCISS()
{
	if(gSCISS.size()<NumSCISS+1)
	{
      gr_SCISSOR *RET=new gr_SCISSOR;
      gSCISS.push_back(RET);
	  return RET;
	}
	else 
	return gSCISS[NumSCISS++]; 
}


typedef SmartVector<gr_BASE*> CgrList;

CgrList grList;

struct COffsetStack
{
	bVector3f ItemOffset;
    bVector3f ItemSize;
	COffsetStack(const bVector3f &IO,const bVector3f &IS)
	{
		ItemOffset=IO;
        ItemSize  =IS;
	}
};

SmartVector<COffsetStack> OffsetStack;
int                     OffsetDepth;

void guifaceClear()
{
	GString Tmp;
	CgrList::iterator it=grList.begin();
	CgrList::iterator end=grList.end();
	NumSCISS=0;
	NumRects=0;
	NumTEXT =0;
	try
	{
	//for(;it!=end;++it)
	//{if(*it)delete (*it);*it=NULL;}
	}
	catch(...){};
	grList.clear();
	guiClearDepthStack();
}

bVector3f ItemOffset;
bVector3f ItemSize;

bVector3f ItemMOFF;

void guiClearDepthStack()
{
    OffsetDepth=0;
	OffsetStack.clear();
	ItemSize.Set(640,480,0);
	ItemOffset.Set(0,0,0);
	ItemMOFF.Set(0,0,0);
	//world->over.ScissorBox(0,0,Box[2],Box[3]);
//	glScissor(0,0,TSystem::tvars.v_width->ivalue,TSystem::tvars.v_height->ivalue);
	gr_SCISSOR *SR=GetSCISS();
	SR->Set(0,0,640,480);
	grList.push_back(SR);
	LastScissor.Set(0,0,0,0);
}

void UpdateScissor()
{
	float x,y,w,h;
	x=ItemOffset[0];
	y=ItemOffset[1];
	w=ItemSize[0];
	h=ItemSize[1];
	if(w>0 && h>0 && x<640 && y<480)
	{
//      AdjustFrom640(&x,&y,&w,&h);
	  gr_SCISSOR *SR=GetSCISS();
	  SR->Set(x,y,w,h);
	  grList.push_back(SR);
      //grList.push_back(new gr_SCISSOR(x,y,w,h));
	}
}

void guifaceSETRECT(const bVector3f &From,const bVector3f &To,bool Stensil)
{
	ItemMOFF.Set(0,0,0);
    bVector3f StartOffset;
    bVector3f StartSize;
	if(OffsetDepth)
	{
		StartOffset=OffsetStack[OffsetDepth-1].ItemOffset;
		StartSize  =OffsetStack[OffsetDepth-1].ItemSize;
	}
	else
	{
		//StartOffset[1]+=15;
		StartSize.Set(640,480,0);
	}
	ItemOffset=From+StartOffset;
	ItemSize  =To;
	ItemMOFF.Set(0,0,0);
	for(int j=0;j<3;j++)if(ItemSize[j]>StartSize[j])ItemSize[j]=StartSize[j]-From[j];else ItemSize[j]=To[j];
	
	COffsetStack Stack(ItemOffset,ItemSize);
	while(OffsetStack.size()-1<OffsetDepth)OffsetStack.push_back(Stack);
    OffsetStack[OffsetDepth]=Stack;
    OffsetDepth++;
	UpdateScissor();
}

void guifaceMODRECT(const bVector3f &From,const bVector3f &To)
{
   ItemMOFF=From;
   //ItemSize  +=To;
}

void guifaceENDRECT()
{
	ItemMOFF.Set(0,0,0);
    bVector3f StartOffset;
    bVector3f StartSize;
	OffsetDepth--;
	if(OffsetDepth)
	{
		StartOffset=OffsetStack[OffsetDepth-1].ItemOffset;
		StartSize  =OffsetStack[OffsetDepth-1].ItemSize;
	}
	else
	{
		//StartOffset[1]+=15;
		StartSize.Set(640,480,0);
	}
	ItemOffset=StartOffset;
	ItemSize  =StartSize;
	UpdateScissor();
}

void guifaceSetResolution(const float X,const float Y)
{

}

void guifaceSetResMode(const DWORD Mode)
{
}

bVector3f vNULL;

bool CheckSize(const bVector3f &Pos,const bVector3f &Size)
{
	if(Pos[0]+ItemMOFF[0]>=ItemSize[0] ||
	   Pos[1]+ItemMOFF[1]>=ItemSize[1] ) return false;
	bVector3f TMP=Pos+Size+ItemMOFF;
	if(TMP[0]<=0 ||
	   TMP[1]<=0 ) return false;
	return true;
};

	bVector3f OffNULL;

void guifaceRECT(const bVector3f &Pos,const bVector3f &Size,const DWORD Texture,const bVector4f &Color,const bVector3f &Rep,const DWORD Blend)
{
    guifaceRECT(Pos,Size,Texture,Color,Rep,OffNULL,Blend);
}

void guifaceRECT(const bVector3f &Pos,const bVector3f &Size,const DWORD Texture,const bVector4f &Color,const bVector3f &Rep,const bVector3f &Off,const DWORD Blend)
{
	if(Color[3]<0.001f)return;
	if(Texture)
	if(CheckSize(Pos,Size))
	{
      gr_RECT *REC=GetRect();
	  REC->Set(ItemOffset+ItemMOFF+Pos,Size,Texture,Color,Rep,Off,Blend);
      grList.push_back(REC);
	}
}
void guifaceTEXT(const bVector3f &Pos,const bVector3f &Size,const BYTE Aling,LPCSTR Text,const DWORD Font,const bVector4f &Color,const float FH)
{
	if(Color[3]<0.001f)return;
	if(CheckSize(Pos,Size))
	{
		gr_TEXT *TEX=GetTEXT();
		TEX->Set(ItemOffset+ItemMOFF+Pos,Size,Aling,Text,Font,Color,FH);
        grList.push_back(TEX);
	}
}

void guifaceRENDER()
{
	CgrList::iterator it=grList.begin();
	CgrList::iterator end=grList.end();
	for(;it!=end;++it)(*it)->render();

//	glScissor(0,0,TSystem::tvars.v_width->ivalue,TSystem::tvars.v_height->ivalue); 
}


bVector3f OldMouse,CurMouse;

const bVector3f &gui_getoldmouse(){return OldMouse;}
void      gui_setoldmouse(const bVector3f &Mouse)
{
	if(Mouse[0]<32000) OldMouse[0]=Mouse[0];
	if(Mouse[1]<32000) OldMouse[1]=Mouse[1];
}

const bVector3f &gui_getcurmouse(){return CurMouse;}
void      gui_setcurmouse(const bVector3f &Mouse)
{
	if(Mouse[0]<32000) CurMouse[0]=Mouse[0];
	if(Mouse[1]<32000) CurMouse[1]=Mouse[1];
}


int _IsMouseLocked=0;

void      LockMouse()
{
  _IsMouseLocked=true;
}
void      UnlockMouse()
{
	_IsMouseLocked=false;
}
bool  IsMouseLocked(){return _IsMouseLocked>0;}