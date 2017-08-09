#pragma once

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

class DLLTYPE TMatrix
{
public:

     float m[16];


    void identity(void);
    void translate(const bVector3f &trans);
    void translate_x(const float &dist);
    void translate_y(const float &dist);
    void translate_z(const float &dist);
    void rotate(const float &angle, bVector3f &axis);
    void rotate_x(const float &angle);
    void rotate_y(const float &angle);
    void rotate_z(const float &angle);
    void scale(const bVector3f &scale);
    void transformPoint( bVector3f *vec );
    void transformVector( bVector3f *vec );

    // Operators...
    TMatrix operator + (const TMatrix &other);
    TMatrix operator - (const TMatrix &other);
    TMatrix operator * (const TMatrix &other);

    TMatrix operator * (const float scalar);

	 TMatrix()  { identity(); }

    TMatrix( float m0, float m4, float  m8, float m12,
               float m1, float m5, float  m9, float m13,
               float m2, float m6, float m10, float m14,
               float m3, float m7, float m11, float m15 );
};


typedef const TMatrix CTMatrix;

/*void MatrixMultiply       (CTMatrix &M1,CTMatrix &M2, TMatrix &out);
void VectorTransform      (float *V, CTMatrix &M,float *Out);
void CreateRotationMatrixX(TMatrix &out,float Sine,float Cosine);
void CreateRotationMatrixY(TMatrix &out,float Sine,float Cosine);
void CreateRotationMatrixZ(TMatrix &out,float Sine,float Cosine);
*/

