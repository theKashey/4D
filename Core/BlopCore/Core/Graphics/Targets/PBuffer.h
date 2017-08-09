// PBuffer.h: interface for the CPBuffer class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __PBUFFER_H_
#define __PBUFFER_H_

class CDepthParams
{
};

#define PBUFFER_1D       (1	  )
#define PBUFFER_2D       (2	  )
#define PBUFFER_CUBE     (3	  )

#define PBUFFER_RGB      (1<<2)
#define PBUFFER_RGBA     (2<<2)
#define PBUFFER_DEPTH    (3<<2)

#define PBUFFER_16	     (1<<4)
#define PBUFFER_24	     (2<<4)
#define PBUFFER_32	     (3<<4)

#define PBUFFER_A0		 (0<<8)
#define PBUFFER_A1		 (1<<8)
#define PBUFFER_A8		 (2<<8)
#define PBUFFER_A16		 (3<<8)


#define PBUFFER_DEPTH0   (0<<10) 
#define PBUFFER_DEPTH16	 (1<<10)
#define PBUFFER_DEPTH24	 (2<<10)
#define PBUFFER_DEPTH32	 (3<<10)

#define PBUFFER_DBUFFER	 (1<<12) 
#define PBUFFER_MIPMAPED (1<<13)
#define PBUFFER_STENSIL  (1<<14)

#define PBUFFER_32D24    PBUFFER_32|PBUFFER_DEPTH24


#define BUFFER_FRONT_LEFT  1


class DLLTYPE CPBuffer  
{
protected:
	DWORD hDC;
	DWORD hGLRC;
	DWORD pBuffer;
	DWORD TexID;

	int   RezX;
	int   RezY;

	WORD  Mode;
	bool  Locked;

	DWORD saveHdc;
	DWORD saveHglrc;
	BYTE  Init;
public:
	CPBuffer();
	CPBuffer(WORD x,WORD y,DWORD Mode=PBUFFER_2D|PBUFFER_32D24);
	virtual ~CPBuffer();

	void DeleteContex();

	bool Create(WORD x,WORD y,DWORD Mode=PBUFFER_2D|PBUFFER_32D24);
	bool Bind(DWORD TextureID=0,DWORD buf = BUFFER_FRONT_LEFT);
	bool Unbind(int buf = BUFFER_FRONT_LEFT);
	bool SelectPlane(BYTE PlaneNum);
	bool Enable ();
	bool Disable();

	bool isLost () const;

	bool IsTexture1D  (){return (Mode&0x03)==PBUFFER_1D;}
	bool IsTexture2D  (){return (Mode&0x03)==PBUFFER_2D;}
	bool IsTextureCUBE(){return (Mode&0x03)==PBUFFER_CUBE;}

	bool IsAlphaHaved    (){return (Mode & PBUFFER_A16     )==PBUFFER_A16;}
	bool IsMipMapHaved   (){return (Mode & PBUFFER_MIPMAPED)==PBUFFER_MIPMAPED;}
	bool IsStensilHaved  (){return (Mode & PBUFFER_STENSIL )==PBUFFER_STENSIL;}
	bool IsDoubleBuffered(){return (Mode & PBUFFER_DBUFFER )==PBUFFER_DBUFFER;}

	bool IsLocked        (){return Locked;}

	bool	setAttr ( int attr, int value );

};

typedef CPBuffer *PPBuffer;

//PPBuffer CreatePBuffer(DWORD RezX,DWORD RezY,DWORD Type,DWORD Flags);
//void     ReleasePBuffer(PPBuffer Target);

#endif 
