/*===========================================================================
    Заголовок: bStrings.cpp
    Автор: Kashey
    Информация: MyStrings

===========================================================================*/
//---------------------------------------------------------------------------


#include "ProjectSpace.h"
#include "bStrings.h"

#include "..\Memory\MemoryPool.h"

#include <stdio.h>


char MidBuf[0xFF*4*1024];

CStringInfo StringInfo;
MemoryPool * GMEM=NULL;

void StringDelete(SlotPoint *&Slot);

SectionLocker StringLocker;

DWORD StringReadyStart=0x096AC;
extern bool gMEM2TCollapsing;

void StringsReady(){StringReadyStart=0xABDF0;}



LPSTR StringNew(WORD Count,LPCSTR Buffer,int CopyLen,SlotPoint *&Slot)
{
 if(StringReadyStart!=0xABDF0)return NULL;
 if(gMEM2TCollapsing)return NULL;
 if(GMEM==(MemoryPool*)1)return NULL;
 if(!GMEM){  GMEM=(MemoryPool*)1;GMEM=new MemoryPool(32*4000,32);
             StringInfo.Clear();
             GMEM->Alloc(-2);
          }

 if(Slot==NULL)
 {
   Slot=GMEM->Alloc(Count+4);
   if(!Slot)
   {
	   ASSERT("Strings out of memmory");
	   return NULL;
   }
   LPSTR Buf=(char*)Slot->GetAddr();
   ++StringInfo.BlockCount;
   StringInfo.AllocCount+=Slot->Size();
   return Buf;
 }
 else
 {
   if(Slot->Size()<(DWORD)(Count+4))
   {
	   delete Slot;
	   Slot=NULL;
	   LPSTR NewBuffer=StringNew(Count,NULL,0,Slot);
	   if(Buffer!=NewBuffer && CopyLen)
	   {
		   memcpy(NewBuffer,Buffer,CopyLen);
	   }//else *NewBuffer=*Buffer;
	   return NewBuffer;
   } 
   else 
	   return LPSTR(Buffer);
 }
}



void StringDelete(SlotPoint *&Slot)
{
 if(!Slot) return;
 if(gMEM2TCollapsing) return;
 --StringInfo.BlockCount;
 StringInfo.AllocCount-=Slot->Size();
 delete Slot;
 Slot=NULL;
 //Val=NULL;
}
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
#define CrNull   {SetSize(1);if(CharPointer){CharPointer[0]=0;_Size=0;} }
#define NewStr ++StringInfo.NumCreated;++StringInfo.NumAlive;CharPointer=NULL;_Size=0;_Hash=0;MemSlot=0;
#define CopyStr(S,_a) {int a=(_a);if(S && a){SetSize(a+1,0);strncpy_s(CharPointer,a+1,S,a);CharPointer[a]=0; CalcSize();} else CrNull;}

inline int smartstrlen(LPCSTR b){ if(b) return (int)strlen(b);return 0;}

  GStringBase::~GStringBase()                      
   { 
     Empty(); 
     ++StringInfo.NumDeleted;
     --StringInfo.NumAlive;
   }
  GStringBase::GStringBase ()                       
  { 
	  NewStr;//CrNull; 
  }
  void GStringBase::HardEmpty ()                       
      { NewStr;CrNull; }
  GStringBase::GStringBase (LPCSTR S)          
      { NewStr; CopyStr(S,smartstrlen(S));}
  GStringBase::GStringBase (LPCSTR S,int nLen) 
      { NewStr; CopyStr(S,MIN(nLen,smartstrlen(S)));}
  GStringBase::GStringBase (const GStringBase &S)  
      { NewStr; CopyStr(S.c_str(),S.GetLength());}

//*******************************************************************************************************************************************

  void GStringBase::TrimLeft()
  {
    if(!CharPointer)return;
    LPCSTR Buf=CharPointer;
    int slen=length();
    const char *S=CharPointer;
	char *D=CharPointer;
	int W=0;
	int x;
    for(x=0;x<slen;++x,++S)
     if(*S!=' ')break;
	if(S==CharPointer)return;
	for(;x<slen;++x,++S,++D)
		*D=*S;
	*D=0;
    //memcpy(CharPointer,C,);
    CalcSize();
  }

  void GStringBase::TrimRight()
  {
    if(!CharPointer)return;
    LPCSTR Buf=CharPointer;
    char *C=CharPointer+_Size-1;
    for(DWORD x=_Size-1;x>=0;--x,--C)
     if(*C==' ')*C=0;else break;
    CalcSize();
  }

  LPSTR       GStringBase::PointAt(DWORD Index)const
  {
    if(!CharPointer || Index<0 || Index>=_Size)return NULL;
    return &CharPointer[Index];
  }



  GStringBase GStringBase::Mid(int nFirst,unsigned int nCount)
  {
   if((int)nCount>smartstrlen(c_str())-nFirst) nCount=smartstrlen(c_str())-nFirst;
   strncpy_s(MidBuf,sizeof(MidBuf),c_str()+nFirst,nCount);
   MidBuf[nCount]=0;
   return (MidBuf);
 }

 GStringBase GStringBase::Right(unsigned int nCount) const
 {
   nCount=MIN((int)nCount,GetLength());
   strncpy_s(MidBuf,sizeof(MidBuf),(CharPointer+length())-nCount,nCount);
   MidBuf[nCount]=0;
   return (MidBuf);
 }

 GStringBase GStringBase::Left(unsigned int nCount ) const
 {
  nCount=MIN((int)nCount,GetLength());
  strncpy_s(MidBuf,sizeof(MidBuf),c_str(),nCount);
  MidBuf[nCount]=0;
  return (MidBuf);
 }

 void GStringBase::MakeLower()
 {
  char *C=CharPointer;
  while(*(C++))
   *C=flocase(*C);
 }

 int GStringBase::Find(LPCSTR s) const
 {
  int sl=smartstrlen(CharPointer);
  int ls=smartstrlen(s);
  for(int x=0;x<sl;++x)
   if(strncmp(CharPointer+x,s,ls)==0) return x;
  return -1;
 }

 int GStringBase::find(const char s) const
 {
  char *C=CharPointer;
  while(*(C++))
    if(*C==s) return C-CharPointer;
  return -1;
 }

 int GStringBase::ReverseFind(LPCSTR s) const
 {
  int sl=smartstrlen(CharPointer);
  for(int x=sl;x!=0;++x)
   if(strncmp(CharPointer+x,s,smartstrlen(s))==0) return x;
  return -1;
 }

   void GStringBase::resize(int NewSize)
   {
     /*if(NewSize<0) NewSize=0;
     CharPointer=StringNew(NewSize+1,CharPointer);
     CharPointer[NewSize]=0;
     CalcSize();
     */
      SetSize(NewSize);
   }

   void GStringBase::insert(GStringBase::iterator begin,char a)
   {
     SetSize(length()+2);
     int l=length();
     char *b=end()+1;
     *b=0;
     for(;b!=begin;--b)*b=*(b+1);
     *b=a;
   }

  GStringBase GStringBase::substr(int from,int len)const
  {
   int _from,_len;
   if(from>length()) return "";
   if(from<0) from=0;
   _from=from;
   if(len<0) len=0;
   if(_from+len>length()) len=length()-from;
   _len=len;
   if(_len>0)
    strncpy_s(MidBuf,sizeof(MidBuf),CharPointer+_from,_len);
    MidBuf[_len]=0;
   return MidBuf;
  }

   GStringBase::iterator GStringBase::erase(GStringBase::iterator first,GStringBase::iterator last)
   {
     return erase((int)(first-CharPointer),(int)(CharPointer-last));
   }

   GStringBase::iterator GStringBase::erase(int from,int to)
   {
    if(to==0) to=from;
    if(from>length() || from>to) return NULL;
    if(from<0) from=0;
    if(from+to>length()) to=length()-to;
//    if(from==to) to++;
    strcpy(CharPointer+from,CharPointer+to);
    CalcSize();
    return begin();
   }


   void GStringBase::Empty(){ if(!this)return; SetSize(0,0);CharPointer=NULL;CalcSize();}
   bool GStringBase::IsEmpty()const{ return CharPointer==NULL || CharPointer[0]==0;}


   
   int GStringBase::CompareNoCase(LPCSTR S)const {  if(CharPointer)return Fstrnccmp(CharPointer,S);return -1;}

  char GStringBase::GetAt(int x)const  { if(x<0)x=length()+x;if(x<length())return CharPointer[x];else return 0;}
  char GStringBase::GetAt (int x)       { if(x<0)x=length()+x;if(x<length())return CharPointer[x];else return 0;}
  void GStringBase::SetAt (int x,char c){ if(x<0)x=length()+x;if(x<length()){ CharPointer[x]=c;CalcSize();}}

  void GStringBase::Format(LPCSTR Format,...)
  {
   	va_list argList;
	va_start(argList, Format);
	FormatV(Format, argList);
	va_end(argList);

  }

  void GStringBase::FormatV(LPCTSTR Format, va_list &argList)
  {
	  if(!Format){ Empty();return;}
    int nMaxLen = 0;
	va_list argListSave = argList;
	for (LPCTSTR lpsz =Format; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%')
		{
			nMaxLen += _tclen(lpsz);
			continue;
		}
		int nItemLen = 0;
		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
		{
			// check for valid flags
			     if (*lpsz == '#')nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||*lpsz == ' ');
			else break;// hit non-flag character
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _ttoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz));
		}
		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _tcsinc(lpsz);
			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = _tcsinc(lpsz);
			}
			else
			{
				nPrecision = _ttoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz));
			}
		}
		// should be on type modifier or specifier
		int nModifier = 0;
		switch (*lpsz)
		{
		// modifiers that affect size
		case 'h':
			
			lpsz = _tcsinc(lpsz);
			break;
		case 'l':
			
			lpsz = _tcsinc(lpsz);
			break;

		// modifiers that do not affect size
		case 'F':
		case 'N':
		case 'L':
			lpsz = _tcsinc(lpsz);
			break;
		}

		// now should be on specifier
		switch (*lpsz)
		{
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, char);
			break;
		// strings
		case 's':
		case 'S':
		{
			LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
			if (pstrNextArg == NULL)
			   nItemLen = 6; // "(null)"
			else
			{
			   nItemLen = lstrlenA(pstrNextArg);
			   nItemLen = max(1, nItemLen);
			}
			break;
		}
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			nItemLen = max(nItemLen, nWidth);
			if (nPrecision != 0)nItemLen = min(nItemLen, nPrecision);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'f':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:break;
				//ASSERT(FALSE);  // unknown formatting option
			}
		}
		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}
   SetSize(nMaxLen,0);
   if(nMaxLen && CharPointer)
    //_vstprintf(CharPointer, Format, argListSave); //_s
     vsprintf_s(CharPointer,nMaxLen+1, Format, argListSave); //_s
    //_vstprintf(CharPointer,nMaxLen+1, Format, argListSave); //_s
    //_vsnprintf
   va_end(argListSave);
   CalcSize();
  }


 void GString::SetSize(int S,int CLen)
 {
  if(CLen==-1)CLen=length();
  if((DWORD)CharPointer==0xCDCDCDCD || (DWORD)CharPointer==0xFBFBFBF || (DWORD)CharPointer==0xFEEEFEEE) CharPointer=0;
  if(S==0)
      {
       if(CharPointer)
       {
         StringDelete(MemSlot);
         CharPointer=0;
         _Size=0;
		 _Hash=0;
       }
       return;
      }
  const int DL=length()-S;
  if(!CharPointer || DL<0 || DL>=32 )  
  { 
	  if(DL>=24)
	  {
	    LPCSTR OldS=CharPointer;
		if(CLen==0)
			 StringDelete(MemSlot);
		SlotPoint *NSlot=NULL;
        CharPointer=StringNew(S+1,NULL,0,NSlot);
        //ASSERT(OldS);
		if(CLen>0)
		{
			memcpy(CharPointer,OldS,CLen+1);
			StringDelete(MemSlot);
		}
		MemSlot=NSlot;
       
	  }
	  else
	    CharPointer=StringNew(S+1,CharPointer,CLen,MemSlot);
  }
 }


#define SetCharP(a)  { if(!a || a[0]==0){SetSize(0);return;}const int intslen=smartstrlen(a);\
SetSize(intslen,0);\
if(a)strcpy_s(CharPointer,intslen+1,a); CharPointer[smartstrlen(a)]=0;CalcSize();}

#define SetCharPin(a,_intslen)  { if(!a || a[0]==0){SetSize(0);return;}const int intslen=(_intslen);\
SetSize(intslen,0);\
if(a)strcpy_s(CharPointer,intslen+1,a); CharPointer[smartstrlen(a)]=0;CalcSize();}

#define SetChar(a)\
if(length()<2)\
SetSize(2);\
CharPointer[0]=a;\
CharPointer[1]=0;CalcSize();



 void GStringBase::operator=(LPCSTR  src)         
     { SetCharP(src); }
 void GStringBase::operator=(GStringBaseRef src) 
     { ASSERT(src.length()==smartstrlen(src));SetCharPin(src.c_str(),src.length()); }
 void GStringBase::operator=(char c)              
     { SetChar(c);}


 GStringBase operator+(LPCSTR a,GStringBase &b)
 {
  strcpy_s(MidBuf,sizeof(MidBuf),a);
  int sl=smartstrlen(a);
  MidBuf[sl]=0;
  strcat_s(MidBuf,sizeof(MidBuf),b.c_str());
  MidBuf[sl+b.length()]=0;
  return MidBuf;
 }

 GStringBase GStringBase::operator+(LPCSTR b)
 { 
  if(c_str() && b && *b)
  {                           
   strcpy_s(MidBuf,sizeof(MidBuf),c_str());   
   //MidBuf[length()]=0;         
   strcat_s(MidBuf,sizeof(MidBuf),b);                 
  }else if(c_str())return c_str();else return b;//MidBuf[0]=0;
  return MidBuf;
 }

 GStringBase GStringBase::operator+(GStringBaseRef b)
 { 
   if(b.c_str())
   {
    if(c_str())
    {                           
     strcpy_s(MidBuf,sizeof(MidBuf),c_str());    
     MidBuf[length()]=0;         
     strcat_s(MidBuf,sizeof(MidBuf),b.c_str());                 
    }
    else return b;//MidBuf[0]=0;
    return MidBuf;
   }
   else return c_str();
  }


 void GStringBase::operator+=(LPCSTR b)
 {
  int NLEN=strlen(b);
  int oldlen=length();
  int USED=length()+NLEN+1;
  SetSize(length()+NLEN+1);
  strcpy_s(CharPointer+oldlen,NLEN+1,b);
  CalcSize();
}

 void GStringBase::operator+=(char b)
 {
   int oldlen=length();
   SetSize(length()+2);
   CharPointer[length()]=b;
   CharPointer[length()+1]=0;
   CalcSize();
 }
 void GStringBase::operator+=(GStringBaseRef b) 
 {
  if(!b.c_str())return;
  int NLEN=(b.length());
  int oldlen=length();
  int USED=length()+NLEN+1;
  SetSize(USED);
  strncpy_s(CharPointer+oldlen,NLEN+1,b,NLEN);
  CharPointer[USED-1]=0;
  CalcSize();
}

 char GStringBase::operator[](int Index) const { return GetAt(Index);}
 char GStringBase::operator[](int Index)       { return GetAt(Index);}

 

 bool GStringBase::operator!=(LPCSTR  o2)const{ return Compare((char*)o2)!=0;}
 bool GStringBase::operator!=(GStringBaseRef o2)const{ return Compare((char*)o2.v_str())!=0;}


 bool GStringBase::operator <(GStringBaseRef o2)const { return Compare((char*)o2.v_str())<0;}


 void GStringBaseLoadString(DWORD id){}

// GStringBase::operator LPCSTR()const{return (LPCSTR)(CharPointer);}

 GStringBase operator+(LPCSTR a,GStringBase::GStringBaseRef b)
 {
	 GString Tmp=a;Tmp+=b;
	 return Tmp;
 }

 GString tmpBufStrFormat;

 LPCSTR StrFormat(LPCSTR Format,...)
 {
	  DOFORMAT(Err,Format);
	  tmpBufStrFormat=Err;
	  return tmpBufStrFormat;
 }

 int Fstrnccmp(LPCSTR A,LPCSTR B)
  {
   char a,b;
   for(DWORD x=0;;++x)
   {
    a=flocase(*A);
    b=flocase(*B);
         if(a<b) return -1;
    else if(a>b) return  1;
    else if(a==b && b==0) return 0;
    else if(a==0 && b!=0)  return -1;
    else if(a!=0 && b==0)  return 1;
    else   { ++A;++B; }
   }
  };

  inline int Fstrcmp(LPCSTR A,LPCSTR B)
  {
    return strcmp(A,B);
   /*for(DWORD x=0;;++x)
         if(*A<*B) return -1;
    else if(*A>*B) return  1;
    else if(*A==*B && *A==0) return 0;
    else if(*A==0 && *B!=0)  return -1;
    else if(*A!=0 && *B==0)  return 1;
    else   { ++A;++B; }
	*/
  }

  char flocase(char c)
  {
    if(c<='Z' && c>='A' )
     return c-'A'+'a';
    return c;
  }


  char fupcase(char c)
  {
    if(c<='z' && c>='a' )
     return c-'a'+'A';
    return c;
  }

  GString GetStringInfo(DWORD MODE)
  {
	  char Buf[0xFF];
	  sprintf_s(Buf,sizeof(Buf),"%d Total,%d Created, %d deleted, in %d bytes",StringInfo.NumCreated-StringInfo.NumDeleted,
	  StringInfo.NumCreated,
	  StringInfo.NumDeleted,
	  StringInfo.AllocCount);
	  return Buf;
  }

  void ConverSlashesToLeft(GString &Dest,const GString &Src)
  {
	  int L=Src.length()+1;
	  LPCSTR src=Src;
      Dest.resize(Src.length());
      LPCSTR c=src;
      char *d=Dest.v_str();
	  for(int i=0;i<L;++i,++c,++d)
	  if(*c!='/') 
		  *d=*c;
	  else 
		  *d='\\';
      Dest.CalcSize();
  }       


GString ExtractFileExt(const GString &Filename)
{
    if (Filename.size() == 0) return "";

    int i = Filename.size();
    GString buf;
	LPCSTR FN=Filename.c_str();
    while((FN[i] != '.') && (i > 0))
    {
        buf = &FN[i--];
    }
	buf.CalcSize();
	_strupr_s((char *)buf.c_str(),buf.length());
    return buf;
}

GString& ExtractFileExt(const GString &Filename,GString &Dest)
{
    if (Filename.size() == 0){ Dest=""; return Dest;}

    int i = Filename.size();
	LPCSTR FN=Filename.c_str();
    while((FN[i] != '.') && (i > 0))
    {
        Dest = &FN[i--];
    }
	Dest.CalcSize();
	_strupr_s((char *)Dest.c_str(),Dest.length());
    return Dest;
}