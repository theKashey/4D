#pragma once

enum FileLoadType {
	FLT_NONE,									// Ничего

	FLT_STORAGETYPE_DEFAULT,
	FLT_STORAGETYPE_FILE,
	FLT_STORAGETYPE_MEMORY,
	FLT_STORAGETYPE_PAKFILE,

	FLT_FILE_READ,
	FLT_FILE_WRITE,
	FLT_FILE_APPEND,
	FLT_FILE_CREATE,
	FLT_FILE_REWRITE,
};


class DLLTYPE CFileSys
{
public:
	CFileSys(void);
	~CFileSys(void);
	GString Name;

	BOOL Open(const GString &FileName, FileLoadType op = FLT_FILE_READ, BOOL Binary = TRUE, FileLoadType SType = FLT_STORAGETYPE_DEFAULT);
	void Close();
	BOOL Feof();

	GString ReadString(UINT size=0);
	BOOL ReadTextString(GString &res);
	template <class T>
	int Read(T *buf, UINT size=0)
	{
		if (size)
		{
			if (StorageType == FLT_STORAGETYPE_FILE)
				fread(buf, size, 1, pFile);
			else if (!Feof())
			{
				memcpy(buf, (BYTE *)pMemBuf + CurPos, size);
				CurPos += size;
			}
		}
	}
	template <class T>
	int Read(T &buf, size_t size = 1)
	{
		UINT sizet = sizeof(T);
		return ReadBuf(&buf, sizet * size);
	}

	template <class T>
	void Write(const T &buf, size_t size = 1)
	{
		UINT sizet = sizeof(T);
		WriteBuf(&buf, sizet * size);
	}
	void WriteBuf(LPCVOID Buf, size_t size);
	int ReadBuf(void *buf, size_t size);
	int Seek(UINT Position, int From);

	void ConvertToMemoryFile();

	void *GetMemoryData();
	DWORD GetSizeInBytes();
	DWORD GetFileSize();
	FileLoadType GetStorageType();
	FILE * GetFile();

	long  Tell();

private:
	FileLoadType StorageType;
	FILE *pFile;
	void *pMemBuf;
	DWORD SizeInBytes;
	DWORD CurPos;
};

typedef CFileSys* PFileSys;
PFileSys DLLTYPE OpenFile(GString FileName);
/*
#ifdef NULL_FS

void DLLTYPE     InitFileSystem(GString Path);
PFileSys DLLTYPE OpenFile(GString FileName);

#define bOpenFile(a,b) 1
#define bCloseFile(a)
#define bSeekFile(a,b,c)
#define bReadFile(a,b,c,d) 1

#else

void DLLTYPE     InitFileSystem(GString Path);
PFileSys DLLTYPE OpenFile(GString FileName);

DWORD DLLTYPE bOpenFile(LPCSTR FileName,LPCSTR Asses,bool MemMap=false);
void  DLLTYPE bCloseFile(DWORD ID);
void  DLLTYPE bSeekFile (DWORD ID,int Off,WORD MODE);
int   DLLTYPE bReadFile (void *Data,int Size,int N,DWORD ID);
template <class T>
int   bReadFile(DWORD ID,T &buf, int size = -1);

int   DLLTYPE bWriteFile (void *Data,int Size,int N,DWORD ID);
template <class T>
int   bWriteFile(DWORD ID,T &buf, int size = -1);

bool DLLTYPE bIsFileExists(LPCSTR FileName);
long DLLTYPE bTellFile(DWORD ID);
long DLLTYPE bFileSize(DWORD ID);
LPCVOID DLLTYPE bGetFileData(DWORD ID);

#endif
*/
