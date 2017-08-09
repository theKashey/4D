#include "projectspace.h"
#include "DepthInit.h"

static void __mglMultMatrixVecd(const float matrix[16], const float in[4],
		      float out[4])
{
    int i;

    for (i=0; i<4; i++) {
	out[i] =
	    in[0] * matrix[0*4+i] +
	    in[1] * matrix[1*4+i] +
	    in[2] * matrix[2*4+i] +
	    in[3] * matrix[3*4+i];
    }
}

static void __mglMultMatricesd(const float a[16], const float b[16],
				float r[16])
{
    int i, j;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    r[i*4+j] =
		a[i*4+0]*b[0*4+j] +
		a[i*4+1]*b[1*4+j] +
		a[i*4+2]*b[2*4+j] +
		a[i*4+3]*b[3*4+j];
	}
    }
}

static void __mglMakeIdentityd(float m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

static int __mglInvertMatrixd(const float src[16], float inverse[16])
{
    int i, j, k, swap;
    float t;
    float temp[4][4];

    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    temp[i][j] = src[i*4+j];
	}
    }
    __mglMakeIdentityd(inverse);

    for (i = 0; i < 4; i++) {
	/*
	** Look for largest element in column
	*/
	swap = i;
	for (j = i + 1; j < 4; j++) {
	    if (fabs(temp[j][i]) > fabs(temp[i][i])) {
		swap = j;
	    }
	}

	if (swap != i) {
	    /*
	    ** Swap rows.
	    */
	    for (k = 0; k < 4; k++) {
		t = temp[i][k];
		temp[i][k] = temp[swap][k];
		temp[swap][k] = t;

		t = inverse[i*4+k];
		inverse[i*4+k] = inverse[swap*4+k];
		inverse[swap*4+k] = t;
	    }
	}

	if (temp[i][i] == 0) {
	    /*
	    ** No non-zero pivot.  The matrix is singular, which shouldn't
	    ** happen.  This means the user gave us a bad matrix.
	    */
	    return 0;
	}

	t = temp[i][i];
	for (k = 0; k < 4; k++) {
	    temp[i][k] /= t;
	    inverse[i*4+k] /= t;
	}
	for (j = 0; j < 4; j++) {
	    if (j != i) {
		t = temp[j][i];
		for (k = 0; k < 4; k++) {
		    temp[j][k] -= temp[i][k]*t;
		    inverse[j*4+k] -= inverse[i*4+k]*t;
		}
	    }
	}
    }
    return 1;
}

float finalMatrix[16];

void di_matrixprep(float _finalMatrix[16])
{
	__mglMultMatricesd(World()->Camera()->GetModelView(),World()->Camera()->GetProjection(), finalMatrix);
    __mglInvertMatrixd(finalMatrix, finalMatrix);
}

void di_ProcessVector(neV4 &pos,neV3 &_out)
{
	pos[0]=pos[0]*2-1;
	pos[1]=pos[1]*2-1;
	pos[2]=pos[2]*2-1;
	pos[3]=1;
	neV4 out;
	__mglMultMatrixVecd(finalMatrix, pos, out);
	const float on3=1.0f/out[3];
    _out[0]=out[0]*on3;
	_out[1]=out[1]*on3;
	_out[2]=out[2]*on3;
}

//#include "QuestScene.h"

extern float ReadZ(const int x,const int y);
extern ZGridData *ZDataGrid;

void di_ProcessPack(float realX,float realY,float deltainx,float deltainy,float fzdRezX,float fzdRezY)
{
     float _y=0;
	 float _x=0;
	 DWORD UsedX=0;
	 DWORD UsedY=0;
     float inx,iny;
     iny=0;
	 for(float y=0;y<realY;++y,iny+=deltainy,_y+=fzdRezY)
	 {
	   const DWORD ddy=y*realX;
	   DWORD x=0;
	   inx=0;
	   _x=0;
	   UsedX=0;
	  for(;x<realX;++x,inx+=deltainx,_x+=fzdRezX)
	  {
	     float z=ReadZ(inx,iny);//if(z==0)z=0.00000000010f;
		 {
         neV4 pos(_x,_y,z,1);
		 di_ProcessVector(pos,ZDataGrid[x+ddy].V);
         ZDataGrid[x+ddy].T[0]=_x;
		 ZDataGrid[x+ddy].T[1]=0.75-_y*0.75f;
		 }
	  }
	 }
}