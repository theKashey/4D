//---------------------------------------------------------------------------

#ifndef bStringsH
#define bStringsH



class CStringInfo
{
public:
 int NumCreated,NumDeleted,NumAlive;
 int BlockCount,AllocCount,MemErrCount;
 void Clear(){NumCreated=NumDeleted=BlockCount=AllocCount=MemErrCount=0;}
 CStringInfo(){Clear();}
};

#define GString GStringBase

#define FORMAT(a,b) {va_list argList;va_start(argList, b);a.FormatV(b,argList);va_end(argList);}
#define DOFORMAT(a,b) GString a;FORMAT(a,b);

class DLLTYPE GStringBase
{
  char      *CharPointer;
  DWORD     _Size;
  DWORD     _Hash;
  SlotPoint *MemSlot;
 public:
   inline void CalcSize()
   {
     _Hash=0;
     if(CharPointer) 
	  {
	   LPCSTR c=c_str();	   
	   _Size=0;
	   if(c)
		{
		  while(*(c++))
		   {
			_Hash = _Hash ^ ( *c << (_Size%24)); 
			++_Size;
		   }
		//   ++_Size;
		  //_Hash%=13;
		}
	  }
	 else 
	  _Size=0;
   }



  BINLINE void Cut(const DWORD i){if(i<_Size)v_str()[i]=0;CalcSize();}
  typedef const GStringBase& GStringBaseRef;
  typedef char* iterator;

  ~GStringBase() ;

  GStringBase();
  GStringBase(LPCSTR S);
  GStringBase(LPCSTR S,int nLen);
  GStringBase(GStringBaseRef S);

  void HardEmpty();


  void        TrimLeft();
  void        TrimRight();
  GStringBase Mid  (int nFirst,unsigned int nCount=65000);
  GStringBase Right(unsigned int nCount) const;
  GStringBase Left (unsigned int nCount ) const;
  LPSTR       PointAt(DWORD Index)const;

  void        MakeLower();

  int         Find(LPCSTR  s) const;
  int         find(CSTR    s) const;
  int         ReverseFind(LPCSTR s) const;

  int         GetLength() const{return _Size;}
  int         GetLength()      {return _Size;}
  int         size()      const{return _Size;};
  int         length()    const{return _Size;}

  void        resize   (int NewSize);

  LPSTR       v_str () const{ return CharPointer;}
  LPCSTR      c_str () const{ return CharPointer;}

  operator    LPCSTR() const{return c_str();}
  iterator    begin () const{ return v_str();}
  iterator    end   () const{return v_str()+length();}

  int Compare(LPCSTR S)const {if(CharPointer)return strcmp(CharPointer,S);else return -1;}
  int Compare4b(LPCSTR S)const
   {
    int I=0;
	DWORD *C1=(DWORD*)c_str();
	DWORD *C2=(DWORD*)S;
	for(;I<length()-4;I+=4)
	 {
	  if(*C1!=*C2) return *C1-*C2;
	  ++C1;
	  ++C2;
	 }
    if(I>=length())return 0;
	//I-=4;
	BYTE *B1=(BYTE*)C1;
	BYTE *B2=(BYTE*)C2;
    for(;I<length();++I)
	 {
	   if(*B1!=*B2) return *B1-*B2;
	   ++B1;
	   ++B2;
	 }
     return 0;
   }

  bool operator==(LPCSTR  o2)const{ return Compare(o2)==0;}
  bool operator==(const GStringBase  &o2)const
  { 
   if(_Hash!=o2._Hash)return false;
   else
   //if(_Size!=o2._Size)return false;
   //else 
   return Compare4b((char*)o2.v_str())==0;
  }

  void        LoadString(int id);


  void        insert(iterator begin,CSTR a);

  GStringBase substr(int from=0,int len=0)const;

  iterator    erase(iterator first,iterator last=0);
  iterator    erase(int from,int to);


  void        Empty();
  bool        IsEmpty()const;

//  int         Compare(LPCSTR S)const;
  int         CompareNoCase(LPCSTR S)const;

  char        GetAt(int i)const;
  char        GetAt(int i);
  void        SetAt(int x,char c);

  void        Format(LPCSTR Format,...);
  void        FormatV(LPCSTR Format,va_list &va);
  void        SetSize(int size,int CLen=-1);

  void operator=(LPCSTR src);
  void operator=(GStringBaseRef src);
  void operator=(char c);


  friend GStringBase DLLTYPE operator+(LPCSTR a,GStringBaseRef b);
  GStringBase operator+(LPCSTR b);
  GStringBase operator+(GStringBaseRef b);

  void operator+=(LPCSTR b);
  void operator+=(CSTR   b);
  void operator+=(GStringBaseRef b);

  char operator[](int Index) const;
  char operator[](int Index);



  bool operator!=(LPCSTR o2)const;
  bool operator!=(GStringBaseRef o2)const;

  bool operator <(GStringBaseRef o2)const;
  bool operator >(GStringBaseRef o2)const;
};

  char DLLTYPE flocase(char c);
  char DLLTYPE fupcase(char c);

  int DLLTYPE Fstrcmp(LPCSTR A,LPCSTR B);

  int DLLTYPE Fstrnccmp(LPCSTR A,LPCSTR B);

  LPCSTR DLLTYPE StrFormat(LPCSTR Format,...);

  GString GetStringInfo(DWORD MODE=0);

  void DLLTYPE ConverSlashesToLeft(GString &Dest,const GString &Src);

#define Istrlen(a) strlen(a)
typedef GString& GStringRef;
GString    DLLTYPE ExtractFileExt(const GString &Filename);
GStringRef DLLTYPE ExtractFileExt(const GString &Filename,GString &Dest);



//---------------------------------------------------------------------------
#endif
