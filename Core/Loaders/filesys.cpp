#include "StdAfx.h"
#include "Core\Loaders\FileSys.h"
#include "Core\Loaders\bFiles.h" 

DWORD SYS_FILES=0;

#include <io.h>

typedef SmartVector<GString> CFileList;

char DEFPATH[0xFF*2];

struct CPackFile
{
	GString Name;
	CFileList FileList;
};


typedef SmartVector<CPackFile> CPackList;

static SmartVector<CPackFile> PackSystem;

// UnIMP parts

extern "C" {
#include "unimp\unimp.h"
}
// File System Parts
void ParsePackFile(CPackFile &pfile)
{
	if (extract_file((char *)pfile.Name.c_str(), 1, NULL, NULL, NULL) != 0)
		pfile.Name = "";
}

void ParsePathForPackFiles(GString DirPath)
{
	_finddata_t fdata;
	long hFile;
	if (DirPath.size()>0)
	{
		if (DirPath[DirPath.size()-1] != '\\')
			DirPath += '\\';
	}// else DirPath = "";
	hFile = _findfirst((DirPath + "*.imp").c_str(), &fdata);
	if (hFile == -1L) return;
	do 
	{
		if (fdata.attrib & _A_SUBDIR)
		{
			if (GString(fdata.name) != GString("."))
				if (GString(fdata.name) != GString(".."))
					ParsePathForPackFiles(DirPath + fdata.name);
				continue;
		}
		LPCSTR ext = &fdata.name[strlen(fdata.name)-3];
		if(strcmp(ext,"pak")!=0 &&   strcmp(ext,"dat")!=0 && strcmp(ext,"imp")!=0  ) continue;
		CPackFile pfile;		
		pfile.Name = DirPath + fdata.name;
		// Parse pack file and fill data
		ParsePackFile(pfile);
		if (!pfile.Name.IsEmpty())
		{
			PackSystem.push_back(pfile);
			Print2Console(SYS_FILES, "Detected pack file: %s", pfile.Name.c_str());
		}
		
	} while (_findnext( hFile, &fdata ) == 0);
	_findclose(hFile);
}

#include <direct.h>

void InitFileSystem(GString Path)
{
	_getcwd(DEFPATH, sizeof(DEFPATH));	// get current path
	strcat_s(DEFPATH,sizeof(DEFPATH),"\\Data\\");
    //strcpy_s(DEFPATH,sizeof(DEFPATH),"Data\\");
	if(SYS_FILES==0)
		RegisterNewConsoleClass(SYS_FILES,"FILES",-1);

	bFile::AddSearchPath(DEFPATH);
	bFile::AddSearchPath("");

	PackSystem.clear();
	ParsePathForPackFiles(Path);
}

GString bGetRootPath()
{
  return DEFPATH;
}
GString bGetAppPath()
{
  char DP[0xFF*2];
  _getcwd(DP,sizeof(DP));
  return DP;
}


FILETIME DLLTYPE bFile::bGetFileTimeW(LPCSTR FileName)
{
	    HANDLE hFile;
		GString tFileName;
		bFileSeachIteration(FileName,
			                tFileName,
		{if((hFile=CreateFile(tFileName,           // open MYFILE.TXT 
                GENERIC_READ,              // open for reading 
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                OPEN_EXISTING,             // existing file only 
                FILE_ATTRIBUTE_NORMAL,     // normal file 
				NULL))!=INVALID_HANDLE_VALUE)break;})
		FILETIME FT;
		if(hFile == INVALID_HANDLE_VALUE)
		{
			memset(&FT,0,sizeof(FT));
			return FT;
		}
		GetFileTime(hFile,0,0,&FT);
        CloseHandle(hFile); 
		return FT;
}

DWORD DLLTYPE bFile::bGetFileTimeWL(LPCSTR FileName)
{    
		return bGetFileTimeW(FileName).dwLowDateTime;
}

DWORD DLLTYPE bFile::bGetFileTimeU(LPCSTR FileName)
{
	    FILETIME FT=bGetFileTimeW(FileName);
	     __int64  ff = *(__int64*)(&FT);
        return   (int)(ff/(__int64)10000000-(__int64)11644473600); 
}

CFileSys::CFileSys(void)
{
#ifdef BLOPFINALBUILD
	StorageType = FLT_STORAGETYPE_DEFAULT;
#else
	StorageType = FLT_STORAGETYPE_PAKFILE;
#endif
	pFile = NULL;
	pMemBuf = NULL;
}

CFileSys::~CFileSys(void)
{
	Close();
}

BOOL CFileSys::Open(const GString &_FileName, FileLoadType op, BOOL Binary, FileLoadType SType)
{
    GString FileName;
    {GString F2=_FileName;
     ConverSlashesToLeft(FileName,F2);
    }

	GString tFileName;

	switch(SType) {
	case FLT_STORAGETYPE_FILE:
		switch(op) 
       {
		case FLT_FILE_READ:
			 bFileSeachIteration(FileName,tFileName,if(fopen_s(&pFile,tFileName.c_str(), Binary? "rb" : "rt"))pFile=NULL;else break;)
			 if(!pFile)
			 { Error2Console(SYS_FILES,"File not found %s", FileName.c_str());return false;}
             else FileName=tFileName;
			 break;
		case FLT_FILE_WRITE:
			 //bFileSeachIteration(FileName,tFileName,
			 if(fopen_s(&pFile,FileName.c_str(), Binary? "wb+" : "wt+"))pFile=NULL;//else break;)
			 if(!pFile)
			 { Error2Console(SYS_FILES,"File not found %s", FileName.c_str());return false;}
             else FileName=tFileName;
			 break;
		default:
			;
         }
		
		break;
	case FLT_STORAGETYPE_MEMORY:
		break;
	case FLT_STORAGETYPE_PAKFILE:
		break;
	default:
		// Default from File
		switch(op) {
		case FLT_FILE_READ:
             bFileSeachIteration(FileName,tFileName,if(fopen_s(&pFile,tFileName.c_str(), Binary? "rb" : "rt"))pFile=NULL;else break;)
			 if(!pFile)
			 { Error2Console(SYS_FILES,"File not found %s", FileName.c_str());return false;}
             else FileName=tFileName;
			 if (pFile) StorageType = FLT_STORAGETYPE_FILE;
			 break;
		case FLT_FILE_WRITE:
			 //bFileSeachIteration(FileName,tFileName,
				 if(fopen_s(&pFile,FileName.c_str(), Binary? "wb+" : "wt+"))pFile=NULL;
				 //else break;)
			 if(!pFile)
			 { Error2Console(SYS_FILES,"File not found %s", FileName.c_str());return false;}
             else FileName=tFileName;
			if (pFile) StorageType = FLT_STORAGETYPE_FILE;
			break;
		default:	;
		}
		if (!pFile) // Else from PackFile
		{
			pMemBuf = NULL;
			GString FN;
			ConverSlashesToLeft(FN,FileName);
			for (CPackList::iterator it=PackSystem.begin(); it!=PackSystem.end(); it++)
			{
                uint32 uSizeInBytes;
				int error = extract_file((char *)(*it).Name.c_str(), 0, &pMemBuf, (char *)FN.c_str(), &uSizeInBytes);
                SizeInBytes=uSizeInBytes;
				if (!error) break;
			}
			if (!pMemBuf){ Error2Console(SYS_FILES,"File not found %s", FileName.c_str());return false;}
			CurPos = 0;
			StorageType = FLT_STORAGETYPE_PAKFILE;
		}

	}
	Name = FileName;
	return true;
}
void CFileSys::Close()
{
	switch(StorageType) {
	case FLT_STORAGETYPE_FILE:
		if (pFile) fclose(pFile);
		pFile = NULL;
		break;
	case FLT_STORAGETYPE_MEMORY:
		break;
	case FLT_STORAGETYPE_PAKFILE:
		break;
	default:
		;
	}
	if (pMemBuf)
	{
		//delete []pMemBuf;
		free(pMemBuf);
		pMemBuf = NULL;
	}
	Name = "";

}

BOOL CFileSys::Feof()
{
	switch(StorageType) {
	case FLT_STORAGETYPE_FILE:
		if (pFile) return feof(pFile);
		break;
	case FLT_STORAGETYPE_MEMORY:
		break;
	case FLT_STORAGETYPE_PAKFILE:
		break;
	default:
		;
	}

	return (CurPos>=SizeInBytes);
}

void CFileSys::ConvertToMemoryFile()
{
	if (StorageType != FLT_STORAGETYPE_FILE) return;
	CurPos = ftell(pFile);
	SizeInBytes = fseek(pFile,0, SEEK_END);
	SizeInBytes = ftell(pFile);
	fseek(pFile,0, SEEK_SET);

	pMemBuf = (BYTE*)malloc(SizeInBytes+1);//new BYTE[SizeInBytes];
	SizeInBytes=fread(pMemBuf, 1, SizeInBytes, pFile);
    ((BYTE*)pMemBuf)[SizeInBytes]=0;
	fclose(pFile);
	pFile = NULL;

	StorageType = FLT_STORAGETYPE_MEMORY;
}

GString CFileSys::ReadString(UINT size)
	{
		GString temp;
		if (size)
		{
			
			temp.resize(size);
			ReadBuf((char *)temp.c_str(),size);
			return temp;
		}
		else
		{
			char b;
			Read(b);
			while (b!=0)
			{
				temp += b;
				Read(b);
			}
			return temp;
		}
		return "";
	}

	BOOL CFileSys::ReadTextString(GString &res)
	{
		if (Feof()) return FALSE;
		res="";
		unsigned char h;
		Read(h);
		while (((h==9)||(h==' '))&&!Feof()) Read(h);
		while ((h!='\n')&&!Feof())
		{
			if (h!=13)
				res+=h;
			Read(h);
			if (h==9) h=' ';
		}
		return TRUE;
	}

	/*template <class T>
	void CFileSys::Read(T *buf, UINT)
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
	void CFileSys::Read(T &buf, size_t size)
	{
		UINT sizet = sizeof(T);
		ReadBuf(&buf, sizet * size);
	}	 */

	//template <class T>
	//void CFileSys::Write(const T &buf, size_t size)
	void CFileSys::WriteBuf(LPCVOID Buf, size_t size)
	{
		if (size)
		{
			if (StorageType == FLT_STORAGETYPE_FILE)
				fwrite(Buf, 1,size, pFile);
			else if (!Feof())
			{
				memcpy((BYTE *)pMemBuf + CurPos, Buf, size);
				CurPos += (DWORD)size;
			}
		}
	}

	int CFileSys::ReadBuf(void *buf, size_t size)
	{
		int readed=0;
		if (size)
		{
			if (StorageType == FLT_STORAGETYPE_FILE)
				{
				 size_t RLEN=size;
				 while(size>0)
			     {
				   int OFF=fread(buf, 1,size, pFile);
				   buf=(char*)buf+OFF;
				   if(!OFF)
					   return readed;
				   readed+=OFF;
				   size-=OFF;
				 }
			     return RLEN;
				}
			else //if (!Feof())
			{
				int RLen=(int)size;
				if (CurPos+size>SizeInBytes)
					RLen=SizeInBytes-CurPos;
				{
					memcpy(buf, (BYTE *)pMemBuf + CurPos, RLen);
					CurPos += (DWORD)RLen;
				}
				return RLen;
			}
		}
		return -1;
	}
	int CFileSys::Seek(UINT Position, int From)
	{
		if (StorageType == FLT_STORAGETYPE_FILE)
			return fseek(pFile, Position, From);
		else
		{
			switch(From) {
			case SEEK_SET:
				CurPos = Position;
				break;
			case SEEK_CUR:
				CurPos += Position;
				break;
			case SEEK_END:
				CurPos = SizeInBytes - Position;
				break;
			}
			return 0;
		}

	}

	long CFileSys::Tell()
	{
		if (StorageType == FLT_STORAGETYPE_FILE)
		 return ftell(pFile);
		return CurPos;
	}

	DWORD CFileSys::GetFileSize()
	{
		if (StorageType == FLT_STORAGETYPE_FILE)
		{
		  	DWORD iSizeInBytes = fseek(pFile,0, SEEK_END);
        	iSizeInBytes = ftell(pFile);
	        fseek(pFile,0, SEEK_SET);
		  return iSizeInBytes;
		}
		else return GetSizeInBytes();
	}

	void *CFileSys::GetMemoryData() { return pMemBuf;}
	DWORD CFileSys::GetSizeInBytes() { return SizeInBytes;}
	FileLoadType CFileSys::GetStorageType() { return StorageType; }


	FILE * CFileSys::GetFile() {return pFile;}

	PFileSys DLLTYPE OpenFile(GString FileName)
	{
       PFileSys FS=new CFileSys();
	   if(FS->Open(FileName)) return FS;
       delete FS;
	   return NULL;
	}



	