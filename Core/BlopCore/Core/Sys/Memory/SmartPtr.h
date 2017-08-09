#pragma once

#if _MSC_VER <= 1300
#pragma warning (disable: 4786)
#endif
#ifdef _DEBUG
	#define safe_cast dynamic_cast
#else
	#define safe_cast static_cast
#endif

#include <map>

namespace SmartPtr{
/*using std;

#include <set>
*/
//using namespace AUTOLOCK_NAMESPACE;

template<class T>
struct Destructors{
	static inline void DeleteArray(T * ptr){
		delete [] ptr;
	}
	static inline void DeletePtr(T * ptr){
		delete ptr;
	}
	static inline void DeleteFree(T * ptr){
		free( ptr );
	}
	static inline void GlobalFree(T * ptr){
		HGLOBAL hGlobal=GlobalHandle(ptr);
		byte lockcount=GMEM_LOCKCOUNT & GlobalFlags(hGlobal);
		while (lockcount--)
			GlobalUnlock(hGlobal);
		::GlobalFree(hGlobal);
	}
	static inline void LocalFree(T * ptr){
		HLOCAL hLocal=LocalHandle(ptr);
		byte lockcount=LMEM_LOCKCOUNT & LocalFlags(hLocal);
		while (lockcount--)
			LocalUnlock(hLocal);
		::LocalFree(hLocal);
	}

	static inline void ComFree(T * ptr){
		ptr->Release();
	}
};

struct ICounterType{
	virtual void Create(LPVOID)=NULL;
	virtual void Destroy(LPVOID)=NULL;
	virtual LONG Increase(LPVOID)=NULL;
	virtual LONG Decrease(LPVOID)=NULL;
	virtual void SetValue(ICounterType *)=NULL;
	virtual LPCVOID GetCounter() const=NULL;
};

template<class T>
class CMapCounterType: public ICounterType{
	typedef std::map<T*, LONG> TMap;
	static TMap & GetMap(){
		static TMap ptrmap;
		return ptrmap;
	}
protected:
	CMapCounterType()
	{}
public:
	void Create(LPVOID _ptr){
		T* ptr=(T*)_ptr;
		if (ptr){
			if (GetMap().find(ptr)!=GetMap().end())
				++(GetMap()[ptr]);
			else
				GetMap()[ptr]=1;
		}
	}
	void Destroy(LPVOID _ptr){
		T* ptr=(T*)_ptr;
		GetMap().erase(ptr);
	}
	LONG Increase(LPVOID _ptr) {
		T* ptr=(T*)_ptr;
		return ++GetMap()[ptr];
	}
	LONG Decrease(LPVOID _ptr){
		T* ptr=(T*)_ptr;
		return --GetMap()[ptr];
	}

	void SetValue(ICounterType*){}
	LPCVOID GetCounter() const{return NULL;}

	static DWORD GetCount(){
		return (DWORD)GetMap().size();
	}
};


template<class T>
class CFastCounterType: public ICounterType{
	struct SFastCounter{
		LONG nCounter;
	};
	mutable SFastCounter * m_pCounter;
protected:
	CFastCounterType()
		:m_pCounter(NULL)
	{}

public:
	void Create(LPVOID _ptr){
		T* ptr=(T*)_ptr;
		m_pCounter=new SFastCounter;
		m_pCounter->nCounter=ptr?1:0;
	}
	void Destroy(LPVOID _ptr){//you must keep critical section over deletion and then terminate it!
		//T* ptr=(T*)_ptr;
		delete m_pCounter;
		m_pCounter=NULL;
	}
	LONG Increase(LPVOID _ptr){
		_ASSERTE(m_pCounter);
		return ++m_pCounter->nCounter;
	}
	LONG Decrease(LPVOID _ptr){
		_ASSERTE(m_pCounter);
		return --m_pCounter->nCounter;
	}
	void SetValue(ICounterType * pFastCounter){
		m_pCounter=const_cast<SFastCounter*>((const SFastCounter*)(pFastCounter->GetCounter()));
	}
	LPCVOID GetCounter() const{return m_pCounter;}
};

template<class T>
class CDereferencer{
protected:
	T * ptr;
public:
	CDereferencer(const T * _ptr=NULL)
		:ptr(const_cast<T *>(_ptr))
	{}
	T & operator *(){
		return *ptr;
	}
	const T & operator *() const{
		return *ptr;
	}

	T* operator ->(){
		_ASSERTE(ptr);
		return ptr;
	}

	const T* operator ->() const{
		_ASSERTE(ptr);
		return ptr;
	}

#if _MSC_VER > 1300
	typedef T * PT;
	typedef const T * PCT;
	__if_exists(T::~T){ //foolprof
		class CProxy: public T{
			void operator delete (LPVOID){}
			void operator delete [](LPVOID){}
		};

		operator CProxy*(){
			return (CProxy*)ptr;
		}

		operator const CProxy*() const{
			return (CProxy*)ptr;
		}
	}
	__if_not_exists(T::~T){
		operator PT(){
			return ptr;
		}
		operator PCT() const{
			return ptr;
		}
	}
#else
	operator T*(){
		return ptr;
	}
	operator const T*() const{
		return ptr;
	}
#endif

};
template<>
class CDereferencer<void>{
protected:
	void * ptr;
public:
	CDereferencer(const void * _ptr=NULL)
		:ptr(const_cast<void *>(_ptr))
	{}
	operator void*(){
		return ptr;
	}
	operator const void*() const{
		return ptr;
	}
};

template<class T, void (*Delete)(T * ptr)=Destructors<T>::DeletePtr, class CounterType=CMapCounterType<T> >
class CSmartPtr: public CDereferencer<T>, public CounterType{
#if _MSC_VER > 1200
	template<class TT, class T>
		friend TT * As(T & smartptr);
#endif
public:
#if _MSC_VER > 1200
	template<class TAliasFor>
	struct SGetAliasFor{
		class CSmartPtrAliase: public CSmartPtr{
		public:
			CSmartPtrAliase(){}
			CSmartPtrAliase(const TAliasFor * pAF)
				:CSmartPtr(pAF)
			{}
			CSmartPtr & operator = (const TAliasFor * pAF){
				return CSmartPtr::operator =(safe_cast<const T*>(pAF));
			}
			operator TAliasFor*(){
				return safe_cast<TAliasFor*>(GetPtr());
			}
			TAliasFor * operator ->(){
				return safe_cast<TAliasFor*>(GetPtr());
			}
			operator LPVOID(){
				return GetPtr();
			}
		};
		typedef CSmartPtrAliase Type;
	};
#endif
	CSmartPtr()	{}

	CSmartPtr(const CSmartPtr & smartptr) {
		*this=smartptr;
	}

	CSmartPtr(const T * _ptr)
		:CDereferencer<T>(_ptr)
	{
		if (ptr)
			CounterType::Create(ptr);
	}

#if _MSC_VER > 1200
	template<class T2, void (*Delete)(T2 * ptr), class CounterType2>
	CSmartPtr & Set(const CSmartPtr<T2, Delete, CounterType2> & _ptr)
#else	
	CSmartPtr & Set(const CSmartPtr & _ptr)
#endif
	{
		CSmartPtr & smartptr=const_cast<CSmartPtr &>(_ptr);
		if(smartptr.GetPtr())
			smartptr.Increase(smartptr.GetPtr());
		Release();
		ptr=smartptr.GetPtr();
		CounterType::SetValue(&smartptr);
		return *this;
	}

	CSmartPtr & operator =(const CSmartPtr & _ptr){
		return Set(_ptr);
	}

#if _MSC_VER > 1200
	template<class T2, void (*Delete)(T2 * ptr), class CounterType2>
	CSmartPtr & operator << (const CSmartPtr<T2, Delete, CounterType2> & _ptr){
		return Set(_ptr);
	}
#endif

	void AddRef(){
		if (ptr)
			CounterType::Increase(ptr);
	}

	bool Release(){
		bool bRes=false;
		if (ptr){
			if (CounterType::Decrease(ptr) <= 0){
				try{
					CounterType::Destroy(ptr);
					Delete(ptr);
					ptr=NULL;
					bRes=true;
				} catch(...){
					_ASSERTE(!("cannot delete ptr\n"));
		}	}	}
		return bRes;
	}	

	~CSmartPtr(){
		Release();
	}

	T* GetPtr() const{
		return ptr;
	}
};

template<class T, void (*Delete)(T * ptr)=Destructors<T>::DeletePtr>
class CFastPtr: public CSmartPtr<T, Delete, CFastCounterType<T> >{
public:
	CFastPtr(){}
	CFastPtr(const T * ptr)
		:CSmartPtr<T, Delete, CFastCounterType<T> >(ptr)
	{}
	CFastPtr(const CFastPtr & ptr)
		:CSmartPtr<T, Delete, CFastCounterType<T> >(ptr)
	{}
};


template<class T, class CounterType=CMapCounterType<T> >
class CComPtr: public CSmartPtr<T, Destructors<T>::ComFree, CounterType>{
public:
	CComPtr(){}
	CComPtr(T * ptr)
		:CSmartPtr<T, Destructors<T>::ComFree, CounterType>(ptr)
	{
		if (ptr) ptr->AddRef();
	}
	CComPtr(const CComPtr & comptr)
		:CSmartPtr<T, Destructors<T>::ComFree, CounterType>(comptr)
	{
		if (ptr) ptr->AddRef();
	}
};


/*
template<class T, class CounterType=CMapCounterType<T> >
struct SGetComPtr{
	typedef CSmartPtr<T, Destructors<T>::ComFree, CounterType > Type;
};
*/


inline void DeleteHandle(HANDLE hHandle){
	::CloseHandle(hHandle);
}
typedef CSmartPtr<void, DeleteHandle > CSmartHandle;

#ifdef _DEBUG
	#define castto(T, val) dynamic_cast<T>(val)
#else
	#define castto(T, val) static_cast<T>(val)
#endif


#if _MSC_VER > 1300

template<class TT, class T>
inline TT * As(T & smartptr){
	return castto(TT*, smartptr.ptr);
}

#else

template<class TT>
class As{
	TT * ptr;
public:
	template<class T>
	As(T & sp){
		ptr = safe_cast<TT*>(sp.GetPtr());
	}
	TT * operator ->(){
		return ptr;
	}

	operator TT & (){
		return *ptr;
	}
};
#endif
};
using SmartPtr::CSmartPtr;

using SmartPtr::CFastPtr;
using SmartPtr::CSmartHandle;
using SmartPtr::As;
using SmartPtr::Destructors;
using SmartPtr::CFastCounterType;

#define SMARTOBJ(a)    SmartPtr::CSmartPtr<a, SmartPtr:: Destructors<a>::DeletePtr  >
#define SMARTARRAY(a)  SmartPtr::CSmartPtr<a, SmartPtr:: Destructors<a>::DeleteArray>
#define SMARTMALLOC(a) SmartPtr::CSmartPtr<a, SmartPtr:: Destructors<a>::DeleteFree >

