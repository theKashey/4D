/*===========================================================================
	Заголовок: cgShader_mng.cpp
    Автор: Kashey
	Информация: сг game manager

===========================================================================*/
#include "projectspace.h"
#include "projectgraphic.h"
#include "cgShader.h"
#include "cgShader_mng.h"

using namespace Draw;

cgShaderMng::cgShaderMng()
{
lua::dostring("shaders={}");
}

cgShaderRef* cgShaderMng::AddShader(LPCSTR _name)
{
	GString name=_name;
	// search for name
	ShaderRefList::iterator it=refs.begin();
    const ShaderRefList::iterator endit=refs.end();
	for(;it!=endit;++it)
	{
		if(name==it->filename)
		{
			// allready referenced, increase reference counter
			it->count++;
			return it;
		}
	}
    return AddRef(name,LoadShaderFromBasepath(name));
}

LPCSTR CUTCG(LPCSTR S)
{
 static GString Str;
 Str=S;
 if(Str.c_str()[Str.GetLength()-2]=='.')
 {
   Str.SetAt(Str.GetLength()-2,0);
   Str.CalcSize();
 }
 else
 {
  for(int i=0;i<Str.GetLength();++i)
      if(Str.c_str()[i]=='.')
          {
           Str.SetAt(i,0);
           Str.CalcSize();
           break;
          }
 }
 return Str;
}

cgShaderRef* cgShaderMng::AddRef(LPCSTR Name,int Ref)
{
    cgShaderRef ref;
	ref.count = 1;
	ref.filename=Name;
	ref.num=Ref;
	refs.push_back(ref);
    lua::doVstring("shaders.%s=nil",CUTCG(Name));
    lua::doVstring("shaders.%s={}",CUTCG(Name));
    lua::doVstring("function tmp_sm_refresh() sm_retouch(\"%s\");end;",Name);
    lua::doVstring("shaders.%s.retouch=tmp_sm_refresh",CUTCG(Name));
    lua::doVstring("tmp_sm_refresh=nil");

	return refs.end()-1;
}

void cgShaderMng::ResetAll(void){
	ShaderRefList::iterator it=refs.begin();
    const ShaderRefList::iterator endit=refs.end();
	for(;it!=endit;++it) it->ResetCount();
}

void SManRetouch(LPCSTR F)
{
	cgShader *S=RENDER->Shaders()->GetShader(F);
	if(S)
	{
		S=RENDER->Shaders()->SwapShaders(S,S->Retouch());
        if(S)
		 delete S;
	}
}

cgShader *cgShaderMng::SwapShaders(cgShader *In,cgShader *snew)
{
    if(!snew)
    {
      Error2Console(SYS_GRAPHIC,"cgShaderMng: shader reused");
      return snew;
    }
    ShaderList::iterator it=shaders.begin();
    const ShaderList::iterator endit=shaders.end();
	for(int i=0;it!=endit;++it,++i) 
		if(*it==In)
		{
			*it=snew;
			return In;
		}
    Print2Console(SYS_GRAPHIC,"Adding new shader...");
    AddRef(snew->GetName(),AddShader(snew));
    //Error2Console(SYS_GRAPHIC,"cgShaderMng:
	return snew;
}

void cgShaderMng::LoadShaders()
{
	bFile::SearchFile search("Shaders/*.cg");
	LPCSTR name;
	GString n;
	for(name=search.FirstFile(); name; name=search.NextFile())
	{
		//n="Shaders/";
		//n+=name;
		AddShader(name);
	}
	Update();
	lua::createLPCSTRfunction("sm_retouch",SManRetouch);
}

//
// loads unloaded  deletes unreferenced
//
void cgShaderMng::Update(void)
{
	ShaderList::iterator it=shaders.begin();
    const ShaderList::iterator endit=shaders.end();
	for(;it!=endit;++it) if((*it)->IsValid()) Print2Console(SYS_GRAPHIC,"--- %s",(*it)->GetName());
	Print2Console(SYS_GRAPHIC,"cgShaderMng: %d programs in stock", shaders.size());
}

int cgShaderMng::AddShader(cgShader *SH)
{
 if(SH)
 {
    shaders.push_back(SH);
	return shaders.size()-1;
 }
 else
  return -1;
}

int cgShaderMng::LoadShaderFromBasepath(LPCSTR name)
{
	static cgShader m;//=new cgShader();
	cgShader *Ret;
	if((Ret=m.LoadBasePath(name,0)))
	{
		shaders.push_back(Ret);
		return shaders.size()-1;
	}
	return -1;
}

int cgShaderMng::GetShaderIdFromName(LPCSTR sname)
{
	if(!sname) return shaders.size()-1;
	// search for name
	ShaderList::iterator it=shaders.begin();
    const ShaderList::iterator endit=shaders.end();

	GString name=sname;
	for(int i=0;it!=endit;++it,++i) 
		if((*it)->GName()==name) return i;

	Error2Console(SYS_GRAPHIC,"*** GetShaderIdFromName: Shader %s not found!\n",sname);
	return -1;
}

cgShader* cgShaderMng::GetShader(const int id)
{
	return shaders[id];
}

cgShader *cgShaderMng::GetShader(LPCSTR id)
{
	if(!id) return *(shaders.end()-1);
	// search for name
	ShaderList::iterator it=shaders.begin();
    const ShaderList::iterator endit=shaders.end();

	GString name=id;
	for(int i=0;it!=endit;++it,++i) 
		if((*it)->GName()==name) return *it;

	Print2Console(SYS_GRAPHIC,"*** cgShaderMng::GetShader: program <%s> not loaded\n",id);
	return NULL;
}

cgShaderRef *cgShaderMng::GetShaderRef(LPCSTR id)
{
	if(!id) return (refs.end()-1);
	// search for name
	ShaderRefList::iterator it=refs.begin();
    const ShaderRefList::iterator endit=refs.end();

	GString name=id;
	for(int i=0;it!=endit;++it,++i) 
		if((*it).filename==name) return it;

	Print2Console(SYS_GRAPHIC,"*** cgShaderMng::GetShader: program <%s> not loaded\n",id);
	return NULL;
}

