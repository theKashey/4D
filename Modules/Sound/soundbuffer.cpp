#include "StdAfx.h"
#include "soundbuffer.h"
#include "al.h"
#include "alc.h"
#include "alu.h"
#include "alut.h"
#include "alinit.h"

#include "vorbis/codec.h"

#define def_psm_size DYNBUF_SIZE+10 //default ogg read block



_SOUNDBUFFERPACK  Buffers;
_SOUNDBUFFERARRAY BuffersPack;
BufferInfo SoundInfoDymmy;

BufferInfo &GetBufferByID(DWORD BufferID)
{   if(BufferID)	return BuffersPack[BufferID-1];
	else            return SoundInfoDymmy; 
}
DWORD FindBufferByName(LPCSTR Name,const DWORD Flags)
{
	return Buffers.GetIDFor(Name,(DWORD)Istrlen(Name));
}

void  BufferInfo::Check    ()
{
    if(CountUse<1)	Delete();
}

int  BufferInfo::Kill(int Count)
{
	CountUse-=Count;
	if(CountUse<1)	Delete();
	return CountUse;
}

int   BufferInfo::Use(int Count)
{
	CountUse+=Count;
	return CountUse;
}

void  BufferInfo::Create(LPCSTR _FileName)
{
	Clear();
	ALuint aID;
	alGenBuffers(1, &aID);
	bufferID=aID;
	currentbuffer=0;
	fIsStreamed=false;
    FileName=_FileName;
}

void    BufferInfo::SetStreamed(const bool Flag)
{
	if(fIsStreamed==Flag)return;
	if(fIsStreamed)
	{ //delete buffer 2
		ALuint aID=sbufferID;
		alDeleteBuffers(1,&aID);
		sbufferID=0;
	}
	else
	{ //create buffer 2
	 ALuint aID;
	 alGenBuffers(1, &aID);
	 sbufferID=aID;
	}
	fIsStreamed=Flag;
}

void    BufferInfo::Delete()
{
	if(CountUse<1)
	{
		ALuint aID=bufferID;
		alDeleteBuffers(1,&aID);
		bufferID=0;
	}
}

void    BufferInfo::Clear ()
{
	bufferID   =0;
	packID     =0;
	CountUse   =0;
	CanBeLooped=true;
}

void  BufferInfo::Add2Pack()
{
	BuffersPack.push_back(*this);
	packID=BuffersPack.size();
	Buffers.PushAs(packID,FileName,FileName.length());
}

int BufferInfo::UseMusic   (const DWORD SourceID)
{
	if(!IsStreamed())return 0;
	//is current buffer=1 - unqyery 0,query 1
	//else                - unqyery 1,query 2
	ALuint			BufOut,BufIn;
	BufOut=BufIn=0;
    /*if(!currentbuffer)
	{
		BufOut=sbufferID;
		BufIn = bufferID;
	}
	else
	{
		BufOut= bufferID;
		BufIn =sbufferID;
	} */ 
	alSourceUnqueueBuffers(SourceID, 1, &BufOut);
	//ALuint aID=BufOut;
	//alDeleteBuffers(1,&aID);
	//alGenBuffers(1, &aID);
	//BufOut=aID;
	if (!CheckALError()) return -1;
	if (ReadOggBlock(DYNBUF_SIZE,BufOut))
	{
		alSourceQueueBuffers(SourceID, 1, &BufOut);
		if (!CheckALError()) return 0;
	}
	else
	{
		ov_pcm_seek(mVF, 0);
		alSourceQueueBuffers(SourceID, 1, &BufOut);
		if (!CheckALError()) return 0;
		//if (!mLooped)
			return 1;
	}
	return 2;
}


DWORD  GetALBufferID(const int BufferID)
{
	if(BufferID==0) return 0;
	if(BufferID>BuffersPack.size())return 0;
    const BufferInfo &info=BuffersPack[BufferID-1];
	return info.GetALID();
}

DWORD  GetALBuffer2ID(const int BufferID)
{
	if(BufferID==0) return 0;
	if(BufferID>BuffersPack.size())return 0;
    const BufferInfo &info=BuffersPack[BufferID-1];
	return info.GetAL2ID();
}

int    IncBufferUsage(const int BufferID)
{
	if(BufferID==0) return 0;
	if(BufferID>BuffersPack.size())return 0;
	BufferInfo &info=BuffersPack[BufferID-1];
	return info.Use(1);

}
int    DecBufferUsage(const int BufferID)
{
	if(BufferID==0) return 0;
	if(BufferID>BuffersPack.size())return 0;
	BufferInfo &info=BuffersPack[BufferID-1];
	int use=info.Use(-1);
	info.Check();
	return use;
}

BufferInfo DummyBuffer;
const BufferInfo& GetInfoForBuffer(const int BufferID)
{
	if(BufferID==0) return DummyBuffer;
	else
	if(BufferID>BuffersPack.size())return DummyBuffer;
	else return BuffersPack[BufferID-1];
}

int   alUseMusicBuffer(const int SourceId,const int BufferID)
{
	if(BufferID==0) return 0;
	if(BufferID>BuffersPack.size())return 0;
	BufferInfo &info=BuffersPack[BufferID-1];
	return info.UseMusic(SourceId);
}



//********************************************************************************
//********************************************************************************
//********************************************************************************
//							OGG
//********************************************************************************
//********************************************************************************
//********************************************************************************

//-----------------------------------------------------------
//    ov_callback specific functions
//-----------------------------------------------------------

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	/*istream *File = reinterpret_cast<istream*>(datasource);
	File->read((char *)ptr, size * nmemb);
	return File->gcount();
	*/
	DWORD *ID=(DWORD*)datasource;
	return bReadFile(ptr,(int)size,(int)nmemb,*ID);
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
	DWORD *ID=(DWORD*)datasource;
	bSeekFile(*ID,offset,whence);
	return 0;
}

long TellOgg(void *datasource)
{
	DWORD *ID=(DWORD*)datasource;
	return bTellFile(*ID);
}

int CloseOgg(void *datasource)
{
	DWORD *ID=(DWORD*)datasource;
	bCloseFile(*ID);
	return 0;
}


//********************************************************************************
//********************************************************************************
//********************************************************************************

bool    BufferInfo::Load(LPCSTR _FileName,DWORD Flags)
{
    GString Ext = ExtractFileExt(_FileName);
	if (Ext == "WAV") return LoadWavFile(_FileName,Flags);
	if (Ext == "OGG") return LoadOggFile(_FileName,Flags); 
	return false;
}

bool BufferInfo::LoadWavFile(const GString &Filename,DWORD Flags)
{
	Create(Filename);
	ALenum		format;
	ALsizei		size;
	ALsizei		freq;
	ALboolean	loop;
	ALvoid     *WavData;
	// Load wav data	
	DWORD WavF=bOpenFile(Filename,"rb",true);
	if(!WavF)
		return false;
	alutLoadWAVMemory((ALbyte*)bGetFileData(WavF), &format, &WavData, &size, &freq, &loop);
	bCloseFile(WavF);
	if (!CheckALError())
		return false;
	Format			= format;
	Rate			= freq;
	// Fill buffer
	alBufferData(bufferID, format, WavData, size, freq);
	// Unload wav
	alutUnloadWAV(format, WavData, size, freq);
	if (!CheckALError()) return false;
	WavData=NULL;
	if (!CheckALError())
		return false;
	Add2Pack();
	return true;
}


bool BufferInfo::LoadOggFile(const GString &Filename, DWORD Flags)
{
	Create(Filename);
	int				DynBuffs = 1, BlockSize;
	ov_callbacks	cb;
	// Fill cb struct
	cb.close_func	= CloseOgg;
	cb.read_func	= ReadOgg;
	cb.seek_func	= SeekOgg;
	cb.tell_func	= TellOgg;

	// Create OggVorbis_File struct
	mVF = new OggVorbis_File;
	// Open Ogg file
	OggFile=bOpenFile(Filename,"rb",true);//.open(Filename.c_str(), ios_base::in | ios_base::binary);
	// Generate local buffers
	if (ov_open_callbacks(&OggFile, mVF, NULL, -1, cb) < 0)
	{
		bCloseFile(OggFile);
		return false;
	}
	bool Streamed=Flags & SND_STREAM;
	SetStreamed(false);
	// Check for existance of sound
	if (!Streamed)
		BlockSize = ov_pcm_total(mVF, -1) * 4;
    if(Streamed || BlockSize> def_psm_size*3)
	{
		if(!Streamed)
			Print2Console(SYS_SOUND,"Warning:Due to huge size sound %s threaded as streamed",Filename.c_str());
		BlockSize	= DYNBUF_SIZE;
		DynBuffs	= NUM_OF_DYNBUF;
		CanBeLooped = false;
		SetStreamed(true);
	}
	
	// Return vorbis_comment and vorbis_info structures
	mComment		= ov_comment(mVF, -1);
	mInfo			= ov_info(mVF, -1);

	// Fill buffer infos
	Rate		= mInfo->rate;
	Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	if(ReadOggBlock(BlockSize))
	Add2Pack();
	if(IsStreamed())
	 if(ReadOggBlock(BlockSize)) return true;
	 else return false;
	return true;
}


char tmpPCM_DATA[def_psm_size];

bool BufferInfo::ReadOggBlock(const int Size,DWORD _BufID)
{
	// vars
	char		eof = 0;
	int			current_section;
	int		    TotalRet = 0, ret;
	if (Size < 1) return false;
	char *PCM_DATA=NULL;
	if(Size>def_psm_size)   PCM_DATA=new char[Size];
	else                    PCM_DATA=tmpPCM_DATA;
	// Read loop
	while (TotalRet < Size) 
	{
		mVF->datasource=&OggFile;
		try
		{
		ret = ov_read(mVF, PCM_DATA + (int)TotalRet, (int)(Size - TotalRet), 0, 2, 1, &current_section);
		// if end of file or read limit exceeded
		if (ret == 0) break;
		else if (ret < 0)
		{
			ASSERT("ERROR IN OGG BIT STREAM" && 0);
		} 		// Error in bitstream
		else TotalRet += ret;
		}
		catch(...)
		{
			break;
		}
	}
	if (TotalRet > 0)
	{
		if(!IsStreamed())
		 alBufferData(bufferID, Format, (void *)PCM_DATA,(ALsizei)TotalRet, Rate);
		else
		{
			DWORD WritingBuf=_BufID;
			
			if(!currentbuffer)
			{	if(!_BufID)
				WritingBuf=bufferID;
                currentbuffer=1;
			}
			else
			{	if(!_BufID)
				WritingBuf=sbufferID;
				currentbuffer=0;
			}
			alBufferData( WritingBuf, Format, (void *)PCM_DATA,(ALsizei)TotalRet, Rate);
            if (!CheckALError()) return false;
		}
	}
	if(PCM_DATA!=tmpPCM_DATA) free(PCM_DATA);
	return (TotalRet > 0);
}


DWORD  FindBufferFor(const GString &Name,const DWORD Flags)
{
    return FindBufferByName(Name,Flags);
}
DWORD  LoadBufferFor(const GString &Name,const DWORD Flags)
{
  BufferInfo Info;
  if(!Info.Load(Name,Flags))return 0;
  return Info.GetPackID();
}

DWORD  GetBufferFor(const GString &Name,const DWORD Flags)
{
    DWORD ret=FindBufferFor(Name,Flags);
    if(!ret)
    {
        ret=LoadBufferFor(Name,Flags);
        if(ret)Print2Console(SYS_SOUND,"Loaded %s",Name.c_str());
    }
    return ret;
}