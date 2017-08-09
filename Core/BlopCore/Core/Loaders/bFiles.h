#ifndef __BFILES_H
#define __BFILES_H

#ifdef NULL_FS

void DLLTYPE     InitFileSystem(GString Path);
PFileSys DLLTYPE OpenFile(GString FileName);

#define bOpenFile(Name,Type) 1
#define bCloseFile(fID)
#define bSeekFile(fID,Off,Mode)
#define bReadFile(Data,Size,N,fID) 1

#else

void     DLLTYPE InitFileSystem(GString Path);

DWORD DLLTYPE bOpenFile(LPCSTR FileName,LPCSTR Asses,bool MemMap=false);
void  DLLTYPE bCloseFile(DWORD ID);
void  DLLTYPE bSeekFile (DWORD ID,int Off,WORD MODE);
int   DLLTYPE bReadFile (void *Data,int Size,int N,DWORD ID);
template <class T>
int   bReadFile(DWORD ID,T &buf, int size = -1);

int   DLLTYPE bWriteFile (const void *Data,int Size,int N,DWORD ID);
template <class T>
int   bWriteFile(DWORD ID,T &buf, int size = -1);
int   DLLTYPE bWriteString(DWORD ID,LPCSTR S);
int   DLLTYPE bWriteVString(DWORD ID,LPCSTR S,...);

GString DLLTYPE bGetDefalutPath();

bool DLLTYPE bIsFileExists(LPCSTR FileName);
long DLLTYPE bTellFile(DWORD ID);
long DLLTYPE bFileSize(DWORD ID);
LPCVOID DLLTYPE bGetFileData(DWORD ID);

GString DLLTYPE bGetRootPath();
GString DLLTYPE bGetAppPath();

namespace bFile
{

class DLLTYPE SearchFile {
public:
	SearchFile(LPCSTR pattern="*",int SybDirLen=0,bool AddDirs=false);
	~SearchFile(void);
	LPCSTR FirstFile();
    LPCSTR NextFile();
private:
	DWORD   handle;						// io handle
	GString pattern;
	DWORD   Level,MaxLevel;
	GString tmp_name;
    bool    AddDirs;
};

class DLLTYPE IncludePath
{
	GString Name;
public:
    IncludePath(LPCSTR Path);
   ~IncludePath();
};
typedef IncludePath* PIncludePath;

int DLLTYPE AddSearchPath(LPCSTR Path);
int DLLTYPE RemoveSearchPath(LPCSTR Path);

DWORD    DLLTYPE bGetFileTimeU(LPCSTR FileName);
FILETIME DLLTYPE bGetFileTimeW(LPCSTR FileName);
DWORD    DLLTYPE bGetFileTimeWL(LPCSTR FileName);


struct ISearchPath
{
	GString Name;
	int     Counter;
	ISearchPath(LPCSTR N){Name=N;Counter=1;}
	int Inc(){Counter++;return Counter;}
	int Dec(){Counter--;return Counter;}
};

typedef ISearchPath* PSearchPath;

PSearchPath DLLTYPE FirstSearchPath();
PSearchPath DLLTYPE NextSearchPath(PSearchPath  P);

#define bFileSeachIteration(a,b,c)                  \
{													\
	bFile::PSearchPath sit=bFile::FirstSearchPath();\
	bFile::PSearchPath bsit=sit;					\
	b=a;											\
	while(sit=bFile::NextSearchPath(sit))			\
	{												\
        b=sit->Name+a;								\
		c											\
	}												\
}


}
#endif
#endif