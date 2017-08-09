#include "ProjectSpace.h"
#include "VectorBase.h"
#include "Matrixs.h"


TMatrix::TMatrix( float m0, float m4, float  m8, float m12,
                      float m1, float m5, float  m9, float m13,
                      float m2, float m6, float m10, float m14,
                      float m3, float m7, float m11, float m15 )
{
    m[0]=m0; m[4]=m4; m[8] =m8;  m[12]=m12;
    m[1]=m1; m[5]=m5; m[9] =m9;  m[13]=m13;
    m[2]=m2; m[6]=m6; m[10]=m10; m[14]=m14;
    m[3]=m3; m[7]=m7; m[11]=m11; m[15]=m15;
}

void TMatrix::identity( void )
{
    m[0]=1.0f; m[4]=0.0f; m[8] =0.0f; m[12]=0.0f;
    m[1]=0.0f; m[5]=1.0f; m[9] =0.0f; m[13]=0.0f;
    m[2]=0.0f; m[6]=0.0f; m[10]=1.0f; m[14]=0.0f;
    m[3]=0.0f; m[7]=0.0f; m[11]=0.0f; m[15]=1.0f;
}

void TMatrix::translate( const bVector3f &trans )
{
    identity();

    m[12] = trans[0];
    m[13] = trans[1];
    m[14] = trans[2];
}

void TMatrix::translate_x( const float &dist )
{
    identity();

    m[12] = dist;
}

void TMatrix::translate_y( const float &dist )
{
    identity();

    m[13] = dist;
}

void TMatrix::translate_z( const float &dist )
{
    identity();

    m[14] = dist;
}

void TMatrix::rotate( const float &angle, bVector3f &axis )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    axis.normalize();

    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];
    
    m[0]  = c + (1-c) * ux;
    m[1]  = (1-c) * ux*uy + s*uz;
    m[2]  = (1-c) * ux*uz - s*uy;
    m[3]  = 0;
    
    m[4]  = (1-c) * uy*ux - s*uz;
    m[5]  = c + (1-c) * pow(uy,2);
    m[6]  = (1-c) * uy*uz + s*ux;
    m[7]  = 0;
    
    m[8]  = (1-c) * uz*ux + s*uy;
    m[9]  = (1-c) * uz*uz - s*ux;
    m[10] = c + (1-c) * pow(uz,2);
    m[11] = 0;
    
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void TMatrix::rotate_x( const float &angle )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    identity();

    m[5]  =  c;
    m[6]  =  s;
    m[9]  = -s;
    m[10] =  c;
}

void TMatrix::rotate_y( const float &angle )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    identity();

    m[0]  =  c;
    m[2]  = -s;
    m[8]  =  s;
    m[10] =  c;
}

void TMatrix::rotate_z( const float &angle )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    identity();

    m[0] =  c;
    m[1] =  s;
    m[4] = -s;
    m[5] =  c;
}

void TMatrix::scale( const bVector3f &scale )
{
    identity();

    m[0]  = scale[0];
    m[5]  = scale[1];
    m[10] = scale[2];
}

void TMatrix::transformPoint( bVector3f *vec )
{
    bVector3f &v = *vec;

    float x = v[0];
    float y = v[1];
    float z = v[2];

    v[0] = x * m[0] +
          y * m[4] +
          z * m[8] + m[12];

    v[1] = x * m[1] +
          y * m[5] +
          z * m[9] + m[13];

    v[2] = x * m[2] +
          y * m[6] +
          z * m[10]+ m[14];
}

void TMatrix::transformVector( bVector3f *vec )
{
    bVector3f &v = *vec;

    float x = v[0];
    float y = v[1];
    float z = v[2];

    v[0] = x * m[0] +
          y * m[4] +
          z * m[8];

    v[1] = x * m[1] +
          y * m[5] +
          z * m[9];

    v[2] = x * m[2] +
          y * m[6] +
          z * m[10];
}

TMatrix TMatrix::operator + ( const TMatrix &other )
{
    TMatrix result;

    result.m[0]  = m[0]  + other.m[0];
    result.m[1]  = m[1]  + other.m[1];
    result.m[2]  = m[2]  + other.m[2];
    result.m[3]  = m[3]  + other.m[3];

    result.m[4]  = m[4]  + other.m[4];
    result.m[5]  = m[5]  + other.m[5];
    result.m[6]  = m[6]  + other.m[6];
    result.m[7]  = m[7]  + other.m[7];

    result.m[8]  = m[8]  + other.m[8];
    result.m[9]  = m[9]  + other.m[9];
    result.m[10] = m[10] + other.m[10];
    result.m[11] = m[11] + other.m[11];

    result.m[12] = m[12] + other.m[12];
    result.m[13] = m[13] + other.m[13];
    result.m[14] = m[14] + other.m[14];
    result.m[15] = m[15] + other.m[15];

    return result;
}

TMatrix TMatrix::operator - ( const TMatrix &other )
{
    TMatrix result;

    result.m[0]  = m[0]  - other.m[0];
    result.m[1]  = m[1]  - other.m[1];
    result.m[2]  = m[2]  - other.m[2];
    result.m[3]  = m[3]  - other.m[3];

    result.m[4]  = m[4]  - other.m[4];
    result.m[5]  = m[5]  - other.m[5];
    result.m[6]  = m[6]  - other.m[6];
    result.m[7]  = m[7]  - other.m[7];

    result.m[8]  = m[8]  - other.m[8];
    result.m[9]  = m[9]  - other.m[9];
    result.m[10] = m[10] - other.m[10];
    result.m[11] = m[11] - other.m[11];

    result.m[12] = m[12] - other.m[12];
    result.m[13] = m[13] - other.m[13];
    result.m[14] = m[14] - other.m[14];
    result.m[15] = m[15] - other.m[15];

    return result;
}

TMatrix TMatrix::operator * ( const TMatrix &other )
{
    TMatrix result;

    result.m[0]  = (m[0]*other.m[0])+(m[4]*other.m[1])+(m[8]*other.m[2])+(m[12]*other.m[3]);
    result.m[1]  = (m[1]*other.m[0])+(m[5]*other.m[1])+(m[9]*other.m[2])+(m[13]*other.m[3]);
    result.m[2]  = (m[2]*other.m[0])+(m[6]*other.m[1])+(m[10]*other.m[2])+(m[14]*other.m[3]);
    result.m[3]  = (m[3]*other.m[0])+(m[7]*other.m[1])+(m[11]*other.m[2])+(m[15]*other.m[3]);

    result.m[4]  = (m[0]*other.m[4])+(m[4]*other.m[5])+(m[8]*other.m[6])+(m[12]*other.m[7]);
    result.m[5]  = (m[1]*other.m[4])+(m[5]*other.m[5])+(m[9]*other.m[6])+(m[13]*other.m[7]);
    result.m[6]  = (m[2]*other.m[4])+(m[6]*other.m[5])+(m[10]*other.m[6])+(m[14]*other.m[7]);
    result.m[7]  = (m[3]*other.m[4])+(m[7]*other.m[5])+(m[11]*other.m[6])+(m[15]*other.m[7]);

    result.m[8]  = (m[0]*other.m[8])+(m[4]*other.m[9])+(m[8]*other.m[10])+(m[12]*other.m[11]);
    result.m[9]  = (m[1]*other.m[8])+(m[5]*other.m[9])+(m[9]*other.m[10])+(m[13]*other.m[11]);
    result.m[10] = (m[2]*other.m[8])+(m[6]*other.m[9])+(m[10]*other.m[10])+(m[14]*other.m[11]);
    result.m[11] = (m[3]*other.m[8])+(m[7]*other.m[9])+(m[11]*other.m[10])+(m[15]*other.m[11]);

    result.m[12] = (m[0]*other.m[12])+(m[4]*other.m[13])+(m[8]*other.m[14])+(m[12]*other.m[15]);
    result.m[13] = (m[1]*other.m[12])+(m[5]*other.m[13])+(m[9]*other.m[14])+(m[13]*other.m[15]);
    result.m[14] = (m[2]*other.m[12])+(m[6]*other.m[13])+(m[10]*other.m[14])+(m[14]*other.m[15]);
    result.m[15] = (m[3]*other.m[12])+(m[7]*other.m[13])+(m[11]*other.m[14])+(m[15]*other.m[15]);

    return result;
}

TMatrix TMatrix::operator * ( const float scalar )
{
    TMatrix result;

    result.m[0]  = m[0]  * scalar;
    result.m[1]  = m[1]  * scalar;
    result.m[2]  = m[2]  * scalar;
    result.m[3]  = m[3]  * scalar;

    result.m[4]  = m[4]  * scalar;
    result.m[5]  = m[5]  * scalar;
    result.m[6]  = m[6]  * scalar;
    result.m[7]  = m[7]  * scalar;

    result.m[8]  = m[8]  * scalar;
    result.m[9]  = m[9]  * scalar;
    result.m[10] = m[10] * scalar;
    result.m[11] = m[11] * scalar;

    result.m[12] = m[12] * scalar;
    result.m[13] = m[13] * scalar;
    result.m[14] = m[14] * scalar;
    result.m[15] = m[15] * scalar;

    return result;
}

/*
void VectorTransform      (float *V, CTMatrix &M,float *Out);
void CreateRotationMatrixX(TMatrix &out,float Sine,float Cosine);
void CreateRotationMatrixY(TMatrix &out,float Sine,float Cosine);
void CreateRotationMatrixZ(TMatrix &out,float Sine,float Cosine);

#define X  0
#define Y  1
#define Z  2
#define W  3

void MatrixMultiply       (CTMatrix &M1,CTMatrix &M2, TMatrix &out)
{
  for(BYTE I = 0;I<4;I++)
    for(BYTE J = 0;J<4;J++)
      out.M[I][J] = M1.M[I][X] * M2.M[X][J] +
                    M1.M[I][Y] * M2.M[Y][J] +
                    M1.M[I][Z] * M2.M[Z][J] +
                    M1.M[I][W] * M2.M[W][J];
}

void VectorTransform      (float *V, CTMatrix &M,float *Out)
{
  Out[X] = V[X] * M.M[X][X] + V[Y] * M.M[Y][X] + V[Z] * M.M[Z][X] + M.M[W][X];
  Out[Y] = V[X] * M.M[X][Y] + V[Y] * M.M[Y][Y] + V[Z] * M.M[Z][Y] + M.M[W][Y];
  Out[Z] = V[X] * M.M[X][Z] + V[Y] * M.M[Y][Z] + V[Z] * M.M[Z][Z] + M.M[W][Z];
}

void CreateRotationMatrixX(TMatrix &Result,float Sine,float Cosine)
{
  Result.M[X][X] = 1;
  Result.M[X][Y] = 0;
  Result.M[X][Z] = 0;
  Result.M[X][W] = 0;

  Result.M[Y][X] = 0;
  Result.M[Y][Y] = Cosine;
  Result.M[Y][Z] = -Sine;
  Result.M[Y][W] = 0;

  Result.M[Z][X] = 0;
  Result.M[Z][Y] = Sine;
  Result.M[Z][Z] = Cosine;
  Result.M[Z][W] = 0;

  Result.M[W][X] = 0;
  Result.M[W][Y] = 0;
  Result.M[W][Z] = 0;
  Result.M[W][W] = 1;
}

void CreateRotationMatrixY(TMatrix &Result,float Sine,float Cosine)
// creates matrix for rotation about y-axis
{
  Result.M[X][X] = Cosine;
  Result.M[X][Y] = 0;
  Result.M[X][Z] = Sine;
  Result.M[X][W] = 0;

  Result.M[Y][X] = 0;
  Result.M[Y][Y] = 1;
  Result.M[Y][Z] = 0;
  Result.M[Y][W] = 0;

  Result.M[Z][X] = -Sine;
  Result.M[Z][Y] = 0;
  Result.M[Z][Z] = Cosine;
  Result.M[Z][W] = 0;

  Result.M[W][X] = 0;
  Result.M[W][Y] = 0;
  Result.M[W][Z] = 0;
  Result.M[W][W] = 1;
}

void CreateRotationMatrixZ(TMatrix &Result,float Sine,float Cosine)
// creates matrix for rotation about z-axis
{
  Result.M[X][X] = Cosine;
  Result.M[X][Y] = -Sine;
  Result.M[X][Z] = 0;
  Result.M[X][W] = 0;

  Result.M[Y][X] = Sine;
  Result.M[Y][Y] = Cosine;
  Result.M[Y][Z] = 0;
  Result.M[Y][W] = 0;

  Result.M[Z][X] = 0;
  Result.M[Z][Y] = 0;
  Result.M[Z][Z] = 1;
  Result.M[Z][W] = 0;

  Result.M[W][X] = 0;
  Result.M[W][Y] = 0;
  Result.M[W][Z] = 0;
  Result.M[W][W] = 1;
}
*/