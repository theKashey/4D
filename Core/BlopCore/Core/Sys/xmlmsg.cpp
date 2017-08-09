/***************************************************************************
                          XMLmsg.cpp  -  description
                             -------------------
    begin                : Thu Jul 3 2003
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
#define _CORE_
#include "ProjectSpace.h"
#include "XMLmsg.h"
#include "stdio.h"

bool isASCII(char c)
{
 //printable charasters is A-z,0-9
 if((c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9')) return true;
 return false;
}

byte _MAPHEX(char c)
{
 if(c<='9') return c-'0';
 if(c>='A' && c<='Z')return 10+c-'A';
 if(c>='a' && c<='z')return 10+c-'a';
 return 0;
}

unsigned char _CHARTOBIN(const char* S)
{
 return _MAPHEX(S[1])+_MAPHEX(S[0])*16;
}

void Escape(const char *v1,char *v2,DWORD v1len)
{
 DWORD x,y;
 for( x=0, y=0;x<v1len;++x)
 {
  if(!isASCII(v1[x])) {sprintf_s(&v2[y],0xFF,"%%%2X",v1[x]);y+=3;}
  else v2[y++]=v1[x];
 }
 v2[y]=0;
 for(x=0;x<strlen(v2);x++) if(v2[x]==' ')v2[x]='0';
}

WORD UnEscape(const char *v1,char *v2,DWORD v1len)
{
 WORD x,y;
 char c;

 for( x=0, y=0;x<v1len;y++)
 {
  if(v1[x]!='%') v2[y]=v1[x++];
  else
  {
   c=_CHARTOBIN(&v1[++x]);
   x+=2;
   v2[y]=c;
  }
 }
 return y;
}

  WORD  Cmd, Len,Command;
  char  Data[1024];

WORD MSG2XML(LPSTR BUFFER,const BlopMessage &msg)
{
 sprintf_s(BUFFER,0xFF,"<msg5 Cmd=\"%d\" Data=\"",msg.Cmd);
 Escape(msg.Data,BUFFER+strlen(BUFFER),msg.Size);
 strcat_s(BUFFER,0xFF,"\" />");
 return strlen(BUFFER);
}

#define TRACKER  for(x=0;x<strlen(caret);x++) if(caret[x]=='"') break; if(caret[x]!='"') return false

#define SET(a)  {strncpy_s(MBUF,sizeof(MBUF),caret,x); MBUF[x]=0; a=atoi(MBUF);}
#define MOVE(a) caret+=x+2+strlen(a);

#define GetParamName for(x=0;x<strlen(caret);x++) if(caret[x]=='=') { strncpy_s(VName,sizeof(VName),caret,x);VName[x]=0; break;} if(caret[x]!='=') break;

#ifdef WINDOWS
#define strcasecmp _stricmp
#endif


bool XML2MSG(LPSTR BUFFER,BlopMessage &msg)
{
 if(strlen(BUFFER)<40) return false; //not all in packet
 char Buffer[0xFF*3],MBUF[0xFF*2],VName[0xFF];
 bzero(&msg,sizeof(msg)+2);
 strcpy_s(Buffer,sizeof(Buffer),BUFFER);
 char *caret;
 caret=BUFFER+strlen("<msg5 ");
 DWORD x;
 bool LenSet,DataSet;
 LenSet=DataSet=0;
 while(1)
 {
   GetParamName;
   //x points to =;
   caret+=x+2;
   TRACKER;
   if(strcmp(VName,"Cmd")==0) SET(msg.Cmd) else
   if(strcmp(VName,"Data")==0)
   {
    DataSet=true;
    strncpy_s(MBUF,sizeof(MBUF),caret,x); MBUF[x]=0;
    msg.Size=UnEscape(MBUF,msg.Data,strlen(MBUF));
   }
  caret+=3;
 }
 return true;
}

