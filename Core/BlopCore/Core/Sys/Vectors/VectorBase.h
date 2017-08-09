//---------------------------------------------------------------------------

#ifndef VectorBaseH
#define VectorBaseH
//---------------------------------------------------------------------------
#ifndef	NEINLINE
#define NEINLINE BINLINE  
#endif
#include "ne/ne_math.h"

#define bVector3f neV3

#define VectorSet3(a,b) (b).SetTo(a)

#define x data[0]
#define y data[1]
#define z data[2]

#define DegToRad(Deg) (Deg*PI_f_180)

template< class A>
class bBasePoint2D
{
    public:
        A data[2];
        bBasePoint2D(){};
		bBasePoint2D(A _x,A _y){x=_x;y=_y;};
		void Set(A _x,A _y)
		{
			x=_x;
			y=_y;
		};
		void SetA(BYTE i,A& V)
		{
         data[i]=V;
		}
		A& operator[](int Index){return data[Index];}
};


#undef x
#undef y
#undef z

//#ifndef BILDER_TEMPLATE
template< class A>
class bBasePoint4D
{
	typedef bBasePoint4D<A> thisClass;
        typedef A cA;
        typedef A& rA;
    public:
        A x,y,z,w;
        bBasePoint4D(){};
	bBasePoint4D(const bBasePoint4D<A> &Src)
	{ x=Src.x;
	  y=Src.y;
	  z=Src.z;
	  w=Src.w;
	};
        bBasePoint4D(const A _x,const A _y,const A _z,const A _w){x=_x;y=_y;z=_z;w=_w;};
	void Set(const A _x=0,const A _y=0,const A _z=0,const A _w=0){x=_x;y=_y;z=_z;w=_w;};
	void Set(const A *v3,const A _w=0){x=v3[0];y=v3[1];z=v3[2];w=_w;};
        BINLINE rA operator[](const int Index)
	{
		switch(Index)
		{
		case 0:return x;
		case 1:return y;
		case 2:return z;
		case 3:return w;
		}
		return x;
	}
	BINLINE cA  operator[](const int Index)const
	{
		switch(Index)
		{
		case 0:return x;
		case 1:return y;
		case 2:return z;
		case 3:return w;
		}
		return x;
	}

	BINLINE operator float*(){return &x;}
	BINLINE void operator+=(const float *c){x+=c[0];y+=c[1];z+=c[2];}
	BINLINE thisClass operator *(const float c)
	{
		return thisClass(x*c,y*c,z*c,w*c);
	}

        BINLINE void operator = (const float* c)
	{
		x=c[0];y=c[1];z=c[2];w=c[3];
	}
};






typedef bBasePoint2D<char > bVector2c;
typedef bBasePoint2D<BYTE > bVector2b;
typedef bBasePoint2D<short> bVector2s;
typedef bBasePoint2D<WORD > bVector2w;
typedef bBasePoint2D<DWORD> bVector2d;
typedef bBasePoint2D<int  > bVector2i;
typedef bBasePoint2D<float> bVector2f;


typedef bBasePoint4D<char > bVector4c;
typedef bBasePoint4D<BYTE > bVector4b;
typedef bBasePoint4D<short> bVector4s;
typedef bBasePoint4D<DWORD> bVector4d;
typedef bBasePoint4D<int  > bVector4i;
typedef bBasePoint4D<float> bVector4f;

//#endif

//typedef neV4 bVector4f;

//extern bVector3f v3ONE;
//extern bVector4f v4ONE;
typedef const bVector3f &v3refo;
typedef const bVector4f &v4refo;
v3refo DLLTYPE v3ONE_();
v4refo DLLTYPE v4ONE_();

#define v3ONE v3ONE_()
#define v4ONE v4ONE_()

/*
#include "Matrixs.h"

*/

// convert X,Y,Z or X Y Z string to vector
#define V3CPY(a,b) {a[0]=b[0];a[1]=b[1];a[2]=b[2];}
#define V4CPY(a,b) {a[0]=b[0];a[1]=b[1];a[2]=b[2];a[3]=b[3];}

#define nMatrix neM4

struct floatVertexV3 {float Data[3];};

namespace Math
{

bVector3f DLLTYPE VectorFromString(LPCSTR String);

float DLLTYPE Str2Float(LPCSTR Str);
int   DLLTYPE Str2Int  (LPCSTR Str);

float DLLTYPE AngleDif(const float A,const float B);
bVector3f DLLTYPE GetAngleDirrection(bVector3f &src,bVector3f &Target);

};

#endif
