/*===========================================================================
    Заголовок: MyLoaders.cpp
    Автор: Kashey
    Информация: FileSystem 3th interface 

===========================================================================*/

#include "projectspace.h"
#include "FileSys.h"  //FileSys.h"
#include "bFiles.h"

#ifndef bLINK

using namespace bFile;


GString DLLTYPE bGetDefalutPath()
 {
  return bGetRootPath();
 }

DWORD NumOpenFiles=0;
typedef SmartVector<PFileSys> CFileArray;

CFileArray MyFiles;

DWORD bOpenFile(LPCSTR FileName,LPCSTR Asses,bool MemMap)
{
  PFileSys F=new CFileSys();
  FileLoadType AS;
  AS=(strstr(Asses,"w")!=NULL)?FLT_FILE_WRITE:FLT_FILE_READ;
  if(!F->Open(FileName,AS,strstr(Asses,"b")!=NULL,FLT_STORAGETYPE_DEFAULT))
  {
	  delete F;
	  return 0;
  }
  if(MemMap)
	  F->ConvertToMemoryFile();
  NumOpenFiles++;
  MyFiles.push_back(F);
  return MyFiles.size();
}
void  bCloseFile(DWORD ID)
{
	if(!ID)return;
	if(ID>(DWORD)MyFiles.size())return;
	if(MyFiles[ID-1])
	{
	 NumOpenFiles--;
	 delete MyFiles[ID-1];
	 MyFiles[ID-1]=NULL;
	}
}
void  bSeekFile (DWORD ID,int Off,WORD MODE)
{
	if(!ID)return;
	if(ID>(DWORD)MyFiles.size())return;
	if(MyFiles[ID-1])
		MyFiles[ID-1]->Seek(Off,MODE);
}
int   bReadFile (void *Data,int Size,int N,DWORD ID)
{
	if(!ID)return -1;
	if(ID>(DWORD)MyFiles.size())return -1;
	if(MyFiles[ID-1])
		return MyFiles[ID-1]->ReadBuf(Data,N*Size);
	return -1;
}
template <class T>
void  bReadFile(DWORD ID,T &buf, int size)
{
	if(!ID)return;
	if(ID>MyFiles.size())return;
	if(size==-1)size=sizeof(T);
	if(MyFiles[ID-1])
		return MyFiles[ID-1]->Read(buf,size);
}

int   bWriteFile (const void *Data,int Size,int N,DWORD ID)
{
	if(!ID)return -1;
	if(ID>(DWORD)MyFiles.size())return -1;
	if(MyFiles[ID-1])
        {
          MyFiles[ID-1]->WriteBuf(Data,N*Size);
          return 0;
        }
	return -1;
}

int   DLLTYPE bWriteString(DWORD ID,LPCSTR S)
{
 return bWriteFile((void*)S,strlen(S),1,ID);
}
int   DLLTYPE bWriteVString(DWORD ID,LPCSTR S,...)
{
 DOFORMAT(T,S);
 return bWriteString(ID,T);
}

template <class T>
void  bWriteFile(DWORD ID,T &buf, int size)
{
	if(!ID)return;
	if(ID>MyFiles.size())return;
	if(size==-1)size=sizeof(T);
	if(MyFiles[ID-1])
        {
		  MyFiles[ID-1]->Write(buf,size);
          return 0;
        }
    return -1;
}

bool bIsFileExists(LPCSTR FileName)
{
	PFileSys F=new CFileSys();
	bool ret=F->Open(FileName)==TRUE;
	delete F;
	return ret;
}

long bTellFile(DWORD ID)
{
	if(!ID)return 0;
	if(ID>(DWORD)MyFiles.size())return 0;
	return MyFiles[ID-1]->Tell();
}

long bFileSize(DWORD ID)
{
	if(!ID)return 0;
	if(ID>(DWORD)MyFiles.size())return 0;
	return MyFiles[ID-1]->GetFileSize();
}

LPCVOID bGetFileData(DWORD ID)
{
	if(!ID)return 0;
	if(ID>(DWORD)MyFiles.size())return 0;
	return MyFiles[ID-1]->GetMemoryData();
}

SearchFile::SearchFile(LPCSTR _pattern,int SybDirLen,bool _AddDirs)
{
	pattern =_pattern;
	handle  =0;
	Level   =0;
	MaxLevel=SybDirLen;
    AddDirs =_AddDirs;
}

#include "IO.H"

SearchFile::~SearchFile()
{
   if(handle)
	_findclose(handle);
}

LPCSTR SearchFile::FirstFile(void)
{
	struct _finddata_t fileinfo;
	
	GString temp_path="Data\\";
	temp_path+=pattern;

	//f_UnixToDosName(temp_pat);

	handle = _findfirst (temp_path, &fileinfo);
	if (handle == -1){
		_findclose(handle);
		handle=0;
		return NULL;
	}
	// skip non-valid filenames
    if(!AddDirs)
	while ((fileinfo.attrib&_A_SUBDIR) || fileinfo.name[0]=='.'){
		if(_findnext(handle,&fileinfo)==-1){
			_findclose(handle);
			handle=0;
			return NULL;
		}
	}
	tmp_name=fileinfo.name;
	return tmp_name;
}

LPCSTR SearchFile::NextFile(void)
{
	struct _finddata_t fileinfo;
	while(_findnext(handle, &fileinfo)!=-1)
    {     
        if(!AddDirs)
		    if ((fileinfo.attrib & _A_SUBDIR))	continue;
		if (fileinfo.name[0] == '.') continue;

        tmp_name=fileinfo.name;
	    return tmp_name;
	}
	_findclose(handle);
	handle=0;
	return NULL;
}



#else

DWORD bOpenFile(LPCSTR FileName,LPCSTR Asses,bool MemMap){return NULL;}
void  bCloseFile(DWORD ID){}
void  bSeekFile (DWORD ID,int Off,WORD MODE){}
int   bReadFile (void *Data,int Size,int N,DWORD ID){return NULL;}
template <class T>
void  bReadFile(DWORD ID,T &buf, int size){}

bool bIsFileExists(LPCSTR FileName){return NULL;}

long bTellFile(DWORD ID){return NULL;}

long bFileSize(DWORD ID){return NULL;}

LPCVOID bGetFileData(DWORD ID)  {return NULL;}

#endif