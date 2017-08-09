#ifndef _VBO_ELEMENT_H
#define _VBO_ELEMENT_H
#pragma once

#define memo_BUFFER     1
#define memo_INDEX	    2
#define memo_PIXELDATA	3
#define memo_PIXELSTORE 4
#define memo_DATASTORE  5
#define memo_DATAREF    6

#define memo_STATIC     4
#define memo_DYNAMIC    8
#define memo_STREAM	    12

#define memo_RSTATIC    5
#define memo_RDYNAMIC   9
#define memo_RSTREAM	13

#define memo_CSTATIC    6
#define memo_CDYNAMIC   10
#define memo_CSTREAM	14

#define memo_READ       (BYTE)1
#define memo_WRITE      (BYTE)2

#define VBO_STATIC_GEOM	    memo_BUFFER,memo_STATIC
#define VBO_DYNAMIC_GEOM    memo_BUFFER,memo_DYNAMIC
#define VBO_STREAM_GEOM	    memo_BUFFER,memo_STREAM
						   
#define VBO_STATIC_INDEX    memo_INDEX ,memo_STATIC
#define VBO_DYNAMIC_INDEX   memo_INDEX ,memo_DYNAMIC
#define VBO_STREAM_INDEX    memo_INDEX ,memo_STREAM

#define VBO_STATIC_PIXELS	memo_PIXELDATA,memo_STATIC
#define VBO_DYNAMIC_PIXELS  memo_PIXELDATA,memo_DYNAMIC
#define VBO_STREAM_PIXELS   memo_PIXELDATA,memo_STREAM

#define VBO_STATIC_GRABER   memo_PIXELSTORE ,memo_RSTATIC
#define VBO_DYNAMIC_GRABER  memo_PIXELSTORE ,memo_RDYNAMIC
#define VBO_STREAM_GRABER   memo_PIXELSTORE ,memo_RSTREAM

#define VBO_DATA_STORE	    memo_DATASTORE,memo_STATIC
#define VBO_DATA_REF	    memo_DATAREF  ,memo_STATIC

#define VBO_RESET_VB        1
#define VBO_RESET_IB        2
#define VBO_RESET_PB        4
#define VBO_RESET_UB        16


class DLLTYPE CVBOElement
{
	DWORD VBOID;
	int   UseCount;
	BYTE  Type;
	DWORD vboType;
	DWORD vboBinded;
	BYTE  vboUsage;
	int   LockCount;
	int   Size;
	int   UseType;
	LPVOID WriteAddr;
	int   MySize;
	char  *Data;
    BYTE   LockCapable;
public:
	CVBOElement (BYTE Type=0,BYTE Usage=0);
	~CVBOElement(void);
	void SetType(BYTE Type,BYTE Usage);

	bool Bind(const bool SkipCheck=false);
	bool BindAs(const DWORD As,const bool SkipCheck=false);
	void Unbind();

	LPVOID Lock(BYTE TO=0);
	void   Unlock(bool Full=false);

	void   SetSize(int Size);
	void   SetBuffer(LPCVOID Buffer,int Size);
	void   SybData(LPVOID Buffer,int size,int offset);
	LPVOID GetData(LPVOID Buffer,int size,int offset);
	void   Resize(int NewSize);

	DWORD decodeVStype(BYTE TYPE)const;
	DWORD decodeVSmode(BYTE TYPE)const;

	static void ResetVBO(DWORD MASK=0xFFFFFFFF);

	bool IsValid();
	bool IsSet  ();

	LPVOID  BufferOffset(const DWORD Off);
	void    SetPointer(LPCVOID p);
};

DWORD DLLTYPE GetTotalVBOSize();

#endif
