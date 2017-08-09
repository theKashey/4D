/*===========================================================================
	Заголовок: cgShader_mng.h
    Автор: Kulib
	Информация: сг game manager

===========================================================================*/

#ifndef cgShaderMng_h
#define cgShaderMng_h

#include "cgshader.h"
//#include "../../shared/include/geom.h"

struct cgShaderRef
{
	int num;
	GString filename;
	int count;
	void ResetCount(){ count=0;}
};


class DLLTYPE cgShaderMng
{
public:
	cgShaderMng();
	cgShaderRef* AddShader(LPCSTR n);
    int AddShader(cgShader *SH);
    cgShaderRef* AddRef(LPCSTR Name,int Ref);
	void ResetAll();
	void Update();
	void LoadShaders();

	typedef SmartVector<cgShaderRef> ShaderRefList;
	typedef SmartVector<cgShader*> ShaderList;
	ShaderRefList   refs;
	ShaderList      shaders;
	
	int    LoadShaderFromBasepath(LPCSTR );
	int    GetShaderIdFromName(LPCSTR sname);
	LPCSTR GetShaderNameFromId(const int id);
	cgShader *GetShader(const int id);
	cgShader *GetShader(LPCSTR name);
	cgShaderRef *GetShaderRef(LPCSTR name);

	cgShader *SwapShaders(cgShader *In,cgShader *snew);

	template<class T>
	void GetT(T* &Ret,LPCSTR Name){Ret=(T*)(this->GetShader(Name));}
};

#endif