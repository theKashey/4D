#ifndef _SOUND_BUFFER_H
#define _SOUND_BUFFER_H

#define NUM_OF_DYNBUF	2		    // num buffers in queue
#define DYNBUF_SIZE		3*512000	// Buffer size
#define ERRMSG(Msg) Error2Console(SYS_SOUND,Msg)

#define SND_STREAM 1

#include "vorbis/vorbisfile.h"  

struct BufferInfo
{
private:
	bool LoadWavFile(const GString &Filename,DWORD Flags);
	bool LoadOggFile(const GString &Filename,DWORD Flags);
	bool ReadOggBlock(const int Size,DWORD Buf=0);
	DWORD   packID;
	DWORD   bufferID;  // primary sound buffer
	DWORD   sbufferID; // secondary sound buffer
	int     currentbuffer;// current buffer indicator
	GString FileName;
	DWORD   Params;
	DWORD   Len;
	DWORD   Rate;
	DWORD   Format;
	LPBYTE  Data;
	int     CountUse;
	bool    CanBeLooped;
	bool    fIsStreamed;

	OggVorbis_File	*mVF;
	vorbis_comment	*mComment;
	vorbis_info		*mInfo;
	DWORD            OggFile;
public:
	bool    IsStreamed()const{return fIsStreamed;}
	int     Kill    (int Count=1);
	int     Use     (int Count=1);
	void    Create  (LPCSTR FileName);
	void    Delete  ();
	void    Clear   ();
	void    Add2Pack();
	bool    Load(LPCSTR FileName,DWORD Flags);
    DWORD   GetPackID()const {return packID;}
	DWORD   GetALID  ()const {return  bufferID;}
	DWORD   GetAL2ID ()const {return sbufferID;}
	void    Check    ();
	int     GetCountUse(){return CountUse;}
	void    SetStreamed(const bool Flag);
	int    UseMusic   (const DWORD SourceID);
};

typedef CBSPDividorLookUpArray<char,int> _SOUNDBUFFERPACK;
typedef SmartVector<BufferInfo>          _SOUNDBUFFERARRAY;

// Vars
extern _SOUNDBUFFERPACK  Buffers;
extern _SOUNDBUFFERARRAY BuffersPack;

DWORD  GetBufferFor  (const GString &Name,const DWORD Flags);
DWORD  GetALBufferID (const int BufferID);
DWORD  GetALBuffer2ID(const int BufferID);
int    IncBufferUsage(const int BufferID);
int    DecBufferUsage(const int BufferID);
const  BufferInfo& GetInfoForBuffer(const int BufferID);
int   alUseMusicBuffer(const int SourceId,const int BufferID);

#endif