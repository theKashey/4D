#include "projectspace.h"
#include "projectgraphic.h"
#include "VBOElement.h"
#include "coregraphics.h"
using namespace Draw;

DWORD TotalVBOSize=0;
DWORD GetTotalVBOSize(){return TotalVBOSize;}


DWORD VBOLastIB=0;
DWORD VBOLastVB=0;
DWORD VBOLastPB=0;
DWORD VBOLastSB=0;
//DWORD vboBinded=0;

extern DWORD SYS_GRAPHIC;

static bool VBO_SUPPORT[8];

static void InitVBO()
{
 ASSERT(COpenGL::Iscore_IN_GL_READY());
 static bool VBO_INITED=false;
 if(VBO_INITED)return;
 if(!glewIsSupported("GL_ARB_vertex_buffer_object"))
 {
  Error2Console(SYS_GRAPHIC,"VBO IS NOT SUPPORTED!! \n FATAL!");
  exit(-1);
 }
 VBO_SUPPORT[0]=true;// VBO IS ALLWAYS ON
 bool PBO=glewIsSupported("GL_EXT_pixel_buffer_object")!=0;
 VBO_SUPPORT[1]=true;// VB IS ON
 VBO_SUPPORT[2]=true;// IB IS ON
 Print2Console(SYS_GRAPHIC,"PBO IS %s",PBO?"ON":"OFF");
 VBO_SUPPORT[3]=PBO;// PBO WriteTO IS ON
 VBO_SUPPORT[4]=PBO;// PBO ReadOut IS ON
 VBO_INITED=true;
}

inline bool IsTypeGeneric(const DWORD t)
{
	//return true;
	switch(t)                                        //0
	{
	 case  GL_ARRAY_BUFFER_ARB         :return true; //1
	 case  GL_ELEMENT_ARRAY_BUFFER_ARB :return true; //2
	 case  GL_PIXEL_UNPACK_BUFFER_ARB  :return VBO_SUPPORT[3];//3
	 case  GL_PIXEL_PACK_BUFFER_ARB    :return VBO_SUPPORT[4];//4
	 case  GL_LIGHT0                   :return false;
     case  GL_LIGHT1                   :return false;
	 default:
		 _assume(0);//return false;
	}
}

void CVBOElement::ResetVBO(DWORD MASK)
{
	if(MASK & VBO_RESET_VB){VBOLastIB=0;glBindBufferARB(GL_ARRAY_BUFFER_ARB        ,0);}
	if(MASK & VBO_RESET_IB){VBOLastVB=0;glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);}
	if(MASK & VBO_RESET_PB){VBOLastPB=0;glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB   ,0);}
	if(MASK & VBO_RESET_UB){VBOLastSB=0;glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB ,0);}	
}

DWORD CVBOElement::decodeVStype(BYTE TYPE)const
{
	switch(TYPE)
	{
	 case 0:return 0;
	 case memo_BUFFER    :return GL_ARRAY_BUFFER_ARB;
	 case memo_INDEX     :return GL_ELEMENT_ARRAY_BUFFER_ARB;
	 case memo_PIXELDATA :return GL_PIXEL_UNPACK_BUFFER_ARB;
	 case memo_PIXELSTORE:return GL_PIXEL_PACK_BUFFER_ARB;
	 case memo_DATASTORE :return GL_LIGHT0;
	 case memo_DATAREF   :return GL_LIGHT1;
	 default:_assume(0);
	}
	return GL_ARRAY_BUFFER_ARB;
}
DWORD CVBOElement::decodeVSmode(BYTE TYPE)const
{
  switch(TYPE)
	{
     case memo_STATIC   :   return GL_STATIC_DRAW_ARB;
     case memo_DYNAMIC  :	return GL_DYNAMIC_DRAW_ARB;
     case memo_STREAM   :	return GL_STREAM_DRAW_ARB;

     case memo_RSTATIC  :   return GL_STATIC_READ_ARB;
     case memo_RDYNAMIC :	return GL_DYNAMIC_READ_ARB;
     case memo_RSTREAM  :	return GL_STREAM_READ_ARB;

     case memo_CSTATIC  :   return GL_STATIC_COPY_ARB;
     case memo_CDYNAMIC :	return GL_DYNAMIC_COPY_ARB;
     case memo_CSTREAM  :	return GL_STREAM_COPY_ARB;


	 default:_assume(0);
	}
	return GL_STREAM_DRAW_ARB;
}

#define checkVBOerror() incheckVBOerror(__FUNCTION__,__LINE__)
int   incheckVBOerror(LPCSTR F,int L)
{
	GLenum ret=glGetError();
	if(!ret || ret==GL_NO_ERROR) return 0;
	GString str;
	switch(ret)
	{
	case GL_INVALID_ENUM     : 
		str="VBO:Parameter target set in bad value";break;
	case GL_INVALID_VALUE    : 
		str="VBO:Param n is less than zero.";break;
	case GL_INVALID_OPERATION:   
		str="VBO:Function called but vbo is not set.";break;
	case GL_OUT_OF_MEMORY    :  
		str="VBO:Unable to alloc memmory.";
		break;
	default: return 0;
	}
	Error2Console(SYS_GRAPHIC,"%s in %s(%d)",str.c_str(),F,L);
	return -1;
}

LPVOID  CVBOElement::BufferOffset(const DWORD Off)
{
   if(!Data) return (LPVOID)Off;
   else      return Data+Off;
}

CVBOElement::CVBOElement(BYTE Type,BYTE Usage)
{

	VBOID    =0;
	UseCount =0;
	Type     =0;
	vboType  =0;
	LockCount=0;
	Size     =0;
	UseType  =0;
	WriteAddr=0;
	vboUsage =0;
	MySize   =0;
	Data     =NULL;
	SetType(Type,Usage);
	vboBinded=decodeVStype(Type);
    LockCapable=0;
}
CVBOElement::~CVBOElement(void)
{
	if(RENDER->GRCollapsing)return;
	//UseCount--;
	//if(UseCount<0)
	if(VBOID)
	{
	  GLuint B=VBOID;
	  glDeleteBuffersARB(1,&B);
	  VBOID=0;
	}
	if(Data)
	{
		if(vboType==decodeVStype(memo_DATASTORE))
		{
			delete []Data;
		}
	}
}

void  CVBOElement::SetPointer(LPCVOID p)
{
	SetType(VBO_DATA_REF);
	Data=(char*)p;//const_cast<char*>(p);
}

void CVBOElement::SetType(BYTE Type,BYTE Usage)
{
	if(!Type)return;
    InitVBO();
	checkVBOerror();
	if(VBOID && (vboUsage!=Usage || vboType!=decodeVStype(Type)))
	{
	  GLuint B=VBOID;
	  glDeleteBuffersARB(1,&B);
	  VBOID=0;
	}
	if(Data && Type!=memo_DATASTORE)  
	{
		if(vboType==memo_DATASTORE)
		{
			delete []Data;
			Data=NULL;
		}
	}
    vboType=decodeVStype(Type);
	checkVBOerror();
	if(!VBOID && IsTypeGeneric(vboType))
	{
     GLuint B;
	 glGenBuffersARB(1,&B);
	 VBOID=B;
	}
	vboUsage=Usage;
	checkVBOerror();
}


bool CVBOElement::IsValid()
{
	return vboType!=0;
}

bool CVBOElement::IsSet  ()
{
	return MySize!=0;
}

bool CVBOElement::Bind(const bool SkipCheck)
{
	if(!IsTypeGeneric(vboType))return true;
	if(SkipCheck)
	{
	switch(vboType)
	{
	case GL_ARRAY_BUFFER_ARB        :  if(VBOLastVB==VBOID)break;VBOLastVB=VBOID;break;
	case GL_ELEMENT_ARRAY_BUFFER_ARB:  if(VBOLastIB==VBOID)break;VBOLastIB=VBOID;break;
	case GL_PIXEL_PACK_BUFFER_ARB   :  if(VBOLastPB==VBOID)break;VBOLastPB=VBOID;break;
	case GL_PIXEL_UNPACK_BUFFER_ARB :  if(VBOLastSB==VBOID)break;VBOLastSB=VBOID;break;
	case GL_LIGHT0                  :  break;
	default:_assume(0);
	}
	}
	else
	{
	switch(vboType)
	{
	case GL_ARRAY_BUFFER_ARB        :  if(VBOLastVB==VBOID)return false;VBOLastVB=VBOID;break;
	case GL_ELEMENT_ARRAY_BUFFER_ARB:  if(VBOLastIB==VBOID)return false;VBOLastIB=VBOID;break;
    case GL_PIXEL_PACK_BUFFER_ARB   :  if(VBOLastPB==VBOID)return false;VBOLastPB=VBOID;break;
	case GL_PIXEL_UNPACK_BUFFER_ARB :  if(VBOLastSB==VBOID)return false;VBOLastSB=VBOID;break;
    case GL_LIGHT0                  :  break;
	default:_assume(0);
	}
	}
	vboBinded=vboType;
	glBindBufferARB(vboType,VBOID);
	checkVBOerror();
	return false;
}

bool CVBOElement::BindAs(const DWORD _As,const bool SkipCheck)
{
	if(!IsTypeGeneric(vboType))return true;
	const DWORD As=decodeVStype(_As);
	if(SkipCheck)
	{
	switch(As)
	{
	case GL_ARRAY_BUFFER_ARB        :  if(VBOLastVB==VBOID)break;VBOLastVB=VBOID;break;
	case GL_ELEMENT_ARRAY_BUFFER_ARB:  if(VBOLastIB==VBOID)break;VBOLastIB=VBOID;break;
	case GL_PIXEL_PACK_BUFFER_ARB   :  if(VBOLastPB==VBOID)break;VBOLastPB=VBOID;break;
	case GL_PIXEL_UNPACK_BUFFER_ARB :  if(VBOLastSB==VBOID)break;VBOLastSB=VBOID;break;
	case GL_LIGHT0                  :  break;
	default:_assume(0);
	}
	}
	else
	{
	switch(As)
	{
	case GL_ARRAY_BUFFER_ARB        :  if(VBOLastVB==VBOID)return false;VBOLastVB=VBOID;break;
	case GL_ELEMENT_ARRAY_BUFFER_ARB:  if(VBOLastIB==VBOID)return false;VBOLastIB=VBOID;break;
    case GL_PIXEL_PACK_BUFFER_ARB   :  if(VBOLastPB==VBOID)return false;VBOLastPB=VBOID;break;
	case GL_PIXEL_UNPACK_BUFFER_ARB :  if(VBOLastSB==VBOID)return false;VBOLastSB=VBOID;break;
	case GL_LIGHT0                  :  break;
	default:_assume(0);
	}
	}
	vboBinded=As;
	glBindBufferARB(As,VBOID);
	checkVBOerror();
	return false;
}

void CVBOElement::Unbind()
{
	if(!IsTypeGeneric(vboType))return;
	//no <if> return . bug fix
	switch(vboBinded)
	{
	case GL_ARRAY_BUFFER_ARB        :  ASSERT(VBOLastVB==VBOID);VBOLastVB=0;break;
	case GL_ELEMENT_ARRAY_BUFFER_ARB:  ASSERT(VBOLastIB==VBOID);VBOLastIB=0;break;
    case GL_PIXEL_PACK_BUFFER_ARB   :  ASSERT(VBOLastPB==VBOID);VBOLastPB=0;break;
	case GL_PIXEL_UNPACK_BUFFER_ARB :  ASSERT(VBOLastSB==VBOID);VBOLastSB=0;break;
	case GL_LIGHT0                  :  break;
	default:_assume(0);
	}
	glBindBufferARB(vboBinded,0);
	checkVBOerror();
}
LPVOID CVBOElement::Lock(BYTE TO)
{
	    if(!vboType)return NULL;
        if(TO==0)TO=LockCapable;
		LockCount++;
		if(	LockCount>0 && WriteAddr) return WriteAddr;
		if(!IsTypeGeneric(vboType))return Data;
		Bind();
        DWORD EndTo=0;
        if(TO & memo_READ)EndTo=GL_READ_ONLY;
        if(TO & memo_WRITE)EndTo=GL_WRITE_ONLY;
        if((TO & memo_READ) && (TO & memo_WRITE))EndTo=GL_READ_WRITE;
        if(!EndTo){ASSERT(FALSE); return NULL;}
		WriteAddr= glMapBufferARB(vboType,GL_WRITE_ONLY_ARB);
		Unbind();
		checkVBOerror();
		return WriteAddr;
}
void   CVBOElement::Unlock(bool Full)
{
	if(!vboType)return;
	if(!LockCount)return;
	LockCount--;
	if(	LockCount<1 || Full)
	{
		if(IsTypeGeneric(vboType))
		{
		 Bind();
		 glUnmapBufferARB(vboType);
		 Unbind();
		}
		WriteAddr=NULL;
		LockCount=0;
	}
	checkVBOerror();
}

void CVBOElement::SetSize(int Size)
{
	if(Size==MySize)return;
	if(!IsTypeGeneric(vboType))
	{
		if(Data && vboType==decodeVStype(memo_DATASTORE))
		{
			delete []Data;
		}
		Data=new char[Size];
        LockCapable=memo_READ | memo_WRITE;
	}
	else
	{
	 Bind();
     BYTE USE=vboUsage & 3;
          if(!USE   ) LockCapable=memo_WRITE;
     else if( USE==1) LockCapable=memo_READ;
     else             LockCapable=memo_READ | memo_WRITE;
	 glBufferDataARB(vboType,Size,NULL,decodeVSmode(vboUsage));
	}
	TotalVBOSize-=MySize;
	MySize=Size;
	TotalVBOSize+=MySize;
	checkVBOerror();
}
void CVBOElement::SetBuffer(LPCVOID Buffer,int Size)
{
    if(Size==-1)Size=MySize;
	if(!IsTypeGeneric(vboType))
	{
	  memcpy(Data,Buffer,Size);
	}
	else
	{
	 Bind();
	 glBufferDataARB(vboType,Size,Buffer,decodeVSmode(vboUsage));
	}
	checkVBOerror();
}
void CVBOElement::SybData(LPVOID Buffer,int size,int offset)
{
	if(!IsTypeGeneric(vboType))
	{
	  memcpy(Data+offset,Buffer,size);
	}
	else
	{
	 Bind();
	 glBufferSubDataARB(vboType,offset,size,Buffer);
	}
	checkVBOerror();
}
LPVOID CVBOElement::GetData(LPVOID Buffer,int size,int offset)
{
	if(!IsTypeGeneric(vboType))
	{
	  memcpy(Buffer,Data+offset,size);
	}
	else
	{
 	 Bind();
	 glGetBufferSubDataARB(vboType,offset,size,Buffer);
	}
	checkVBOerror();
	return Buffer;
}
void CVBOElement::Resize(int NewSize)
{
}
