#include "stdafx.h"

using namespace bFile;



typedef SmartVector<ISearchPath> VSearchPath;

VSearchPath SeachPaths;

IncludePath::IncludePath(LPCSTR Path)
{
	Name=Path;
    AddSearchPath(Name);
}
IncludePath::~IncludePath()
{
    RemoveSearchPath(Name);
}

int bFile::AddSearchPath(LPCSTR Path)
{
	GString gPath=Path;
	if(gPath.size()==0)gPath="";
	else
	{
		if(*(gPath.end()-1)!='\\') gPath+='\\';
		{GString F2=gPath;
        ConverSlashesToLeft(gPath,F2);
       }
	}
	for(VSearchPath::iterator it=SeachPaths.begin();it!=SeachPaths.end();++it)
	{
		if(it->Name==gPath)
			return it->Inc();
	}
	SeachPaths.push_back(ISearchPath(gPath));
	return 1;
}

int bFile::RemoveSearchPath(LPCSTR Path)
{
	GString gPath=Path;
	if(gPath.size()==0)gPath="";
	else
	{
		if(*(gPath.end()-1)!='\\') gPath+='\\';
		{GString F2=gPath;
        ConverSlashesToLeft(gPath,F2);
       }
	}
	for(VSearchPath::iterator it=SeachPaths.begin();it!=SeachPaths.end();++it)
	{
		if(it->Name==gPath)
		{
			if(it->Dec()<1)
				SeachPaths.erasePop(it);
			return 1;
		}
	}
	return 0;
}

PSearchPath bFile::FirstSearchPath()
{
 return SeachPaths.end();
}
PSearchPath bFile::NextSearchPath(PSearchPath  P)
{
	PSearchPath ret=P-1;
	if(ret<SeachPaths.begin()) return NULL;
	else return ret;
}