//---------------------------------------------------------------------------

#ifndef SmartVectorH
#define SmartVectorH
//---------------------------------------------------------------------------

#include <stdlib.h>

class CMEMA
{

};



//CMEMA ma;

class DLLTYPE CBaseSmartVector
{
protected:
  int _size,_allocated;
  WORD GROW_SIZE;
 public:
  int &_Size     (){return _size;}
  int &_Allocated(){return _allocated;}
  int size()const {return _size;}
  virtual DWORD elem_size()=0;

  void SetGrowSize(int S){GROW_SIZE=S;}

  CBaseSmartVector();

  LPVOID PoolMem(LPVOID From,const DWORD Size);

  static DWORD _SmartReadyStart;
  bool SmartReadyStart();

  void parentKillBlock()
  {

  };

  bool ASSERTSIZE(const bool Ind)const
  {
	  if(!Ind)
	  {
		  ASSERT("SVector:ind>=size!" && FALSE);
		  return false;
	  }
	  else
	  {
		  return true;
	  }
  }
};

template <class T>
class /*DLL_TYPE*/ SmartVector:public CBaseSmartVector
{  
 private:
  void KILLBLOCK(T &In)
  {
	  parentKillBlock();
	  In.~T();
  }

 public:
  T *data;
  typedef       T*  iterator;
  typedef const T* Citerator;

  DWORD elem_size(){return sizeof(T);}

  iterator begin() {return &data[0];}
  iterator end  () {return &data[_size];}

  Citerator begin()const {return &data[0];}
  Citerator end  ()const {return &data[_size];}

  SmartVector(int I=0):CBaseSmartVector()
  {
   _allocated=0;
   _size=0;
   data=NULL;
   if(SmartReadyStart() && I)
     DropData(I);
  }

  ~SmartVector()
  {
   if(data) _resize(0);//PoolMem(data,0);
   data=NULL;
   _size=_allocated=0;
  }

  void DropData(int I=10)
  {
   _allocated=I;
   data=(T*)PoolMem(NULL,_allocated*sizeof(T));
   if(data)
     ZeroMemory(data,_allocated*sizeof(T));
   _size=0;
  }

  void _resize(int Nsize)
  {
   const int nsize=Nsize;
   if(Nsize<0) Nsize=_allocated-Nsize;
   	_allocated=Nsize;
   if(!data)
   {
	   data=(T*)PoolMem(data,_allocated*sizeof(T));
       ZeroMemory(data,_allocated*sizeof(T));
   }
   else
   {
	if(Nsize<size())
	{
		for(int i=Nsize;i<size();++i)
			KILLBLOCK(data[i]);
	}
    data=(T*)PoolMem(data,Nsize*sizeof(T));
    
    if(nsize>0)
    {   if(nsize>_size) ZeroMemory(data+_size,(nsize-_size)*sizeof(T));}
    else
	    ZeroMemory(data+_size,(-nsize)*sizeof(T));
   }
   
  }

  void SetSize(int Nsize,const bool resize=true)
  {
	  if(resize || Nsize>_allocated-2)_resize(Nsize+GROW_SIZE);
	  _size=Nsize;
  }

  void resize (int i){ SetSize(i,true);}
  void reserve(int i){int oldsize=_size;SetSize(i,false);_size=oldsize;}

  BINLINE T& push_back(const T& Item)
  {
   if(_allocated-_size<2) _resize(-GROW_SIZE);
   ASSERT(_size<_allocated);
   data[_size]=Item;
   return data[_size++];
  }

  template<class T2>
  BINLINE T& push_ref(T2& Item)
  {
   if(_allocated-_size<2) _resize(-GROW_SIZE);
   ASSERT(_size<_allocated);
   data[_size]=Item;
   return data[_size++];
  }


  void eraseItem(const T& Item)
  {
   for(int x=0;x<_size;++x)
    if(data[x]==Item){ erase(x);return;}
  }
  void erasePopItem(const T& Item)
  {
   for(int x=0;x<_size;++x)
    if(data[x]==Item){ erasePop(x);return;}
  }
  void erasePop(const T* Item)
  {
   for(int x=0;x<_size;++x)
    if(data+x==Item){ erasePop(x);return;}
  }

  void checkbound()
  {
	  if(data[_size-1]==NULL){ _size--;checkbound();}
  }

  iterator erase    (const DWORD itemIndex)
  {
   if(_size==0)return NULL;
   ASSERT((int)itemIndex<_size);
   if((int)itemIndex==_size-1){_size--;return &data[itemIndex];}
   KILLBLOCK(data[itemIndex]);
   data[itemIndex]=data[_size-1];
   _size--;
   return &data[itemIndex];
  }

  void erasePop(const DWORD itemIndex)
  {
   if(_size==0)return;
   ASSERT((int)itemIndex<_size);
   KILLBLOCK(data[itemIndex]);
   for(int x=itemIndex;x<=_size-1;++x)
      data[x]=data[x+1];
   ZeroMemory(&data[_size-1],sizeof(T));
   _size--;
  } 

  void clear()
  {
   _size=0;
  }

  BINLINE T& operator [](int index)     
  {
#ifdef _DEBUG
	  ASSERTSIZE(index<_size);
#endif
	  return data[index];
  }
  BINLINE T  operator [](int index)const
  {
#ifdef _DEBUG
	  ASSERTSIZE(index<_size);
#endif
	  return data[index];
  }

  BINLINE T& ref (int index){ASSERT(index<_size);return  data[index];}
  BINLINE T* addr(int index){ASSERT(index<_size);return &data[index];}

  void CopyFrom(const SmartVector& Src)
  {
	  _size=Src._size;
	  if(_allocated-_size<1) _resize(_size+7);
	  memcpy(data,Src.data,_size*sizeof(T));
  }

};

template <class T,class A>
class GList
{
 struct CKey
 {
	 T Key;
	 A Value;
	CKey(T &_Key,A _Value)
	{
		Key  =_Key;
		Value=_Value;
	}
 };
 SmartVector<CKey> Values;
public:
 void Push(T &K,A &V){ Values.push_back(CKey(K,V));}
 bool Lookup(A &a,T &K)
 { 
	 for(SmartVector<CKey>::iterator it=Values.begin(); it!=Values.end();++it)
		 if((*it).Key==K) {a=(*it).Value;return true;}
	 return false;
 };
};

template <class T>
class SmartCasheBase
{
 
};



template <class T>
class SmartCashe: public SmartCasheBase<T>,public SmartVector<T>
{
};
#endif
