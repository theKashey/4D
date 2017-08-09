/***************************************************************************
                          HashLib.cpp  -  description
                             -------------------
    begin                : Sat Jun 21 2003
    copyright            : (C) 2003 by kashey
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "ProjectSpace.h"
#include "HashLib.h"
#include "stdio.h"



#define TEADelta 0x9E3779B9

#define xor ^

inline void Encrypt(DWORD K0,DWORD K1,DWORD K2,DWORD K3, DWORD sum,DWORD k,DWORD &y,DWORD &z)
{
sum=0;
for(k=0;k<=31;++k)
{
 sum+=TEADelta;
 y+=((z<<4)+K0) xor (z+sum) xor ((z>>5)+K1);
 z+=((y<<4)+K2) xor (y+sum) xor ((y>>5)+K3);
}
}


void __Hash1(char *Src,char *Dest)
{
 DWORD g1,g2,h1,h2,tmp1,tmp2,  y,z,k,sum;
 g1=g2=h1=h2=tmp1=tmp2=0;
 DWORD srclen=strlen(Src);
 k=sum=0;
 for(DWORD i=0;i<srclen;i++)
 {
  y=h1;z=h2;
  Encrypt(g1,g2,HIWORD(Src[i]),LOWORD(Src[i]),sum,k,y,z);
  tmp1=y;tmp2=z;h1=h1 ^ y; h2= h2 ^ z;
  y=g1;z=g2;
  Encrypt(HIWORD(Src[i]),LOWORD(Src[i]), tmp1, tmp2,sum,k,y,z);
  g1=g1 ^ y; g2= g2 ^ z;
 }
 sprintf_s(Dest,0xFF,"%4X%4X%4X%4X",h1,h2,g1,g2);
}

void __Hash1Prepare(LPCSTR In,LPSTR Out,DWORD *Key)
{
 sprintf_s(Out,0xFF,"%x%X%s%x%x%X",Key[0],Key[1],In,Key[2],Key[3],Key[1]*In[0]*In[1]);
}

//end

