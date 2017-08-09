/*
unimp.c   IMP archive extraction source code version 1.13   June 26, 2001
Copyright (c) 1999-2001 Technelysium Pty Ltd

This file may be freely distributed provided that it is unmodified. This source
code may be incorporated into other programs only for the purpose of listing or
extracting the contents of IMP archives. Technelysium Pty Ltd makes no
guarantee that this source code is error-free or fit for a particular purpose.

Version 1.1 has been tested on Windows 95 and SunOS 5.6. For other platforms,
unimp.h may need minor modifications.

Modification history:
Date           Programmer        Notes
Feb   2, 2000  Conor McCarthy    Changed *dest++=dest[-dist] in LZ77 copying
                                 (strictly incorrect, doesn't work with lcc-win32)
May  23, 2001  Conor McCarthy    Fixed bug in BWT extraction
June 26, 2001  Conor McCarthy    Memory allocation in decomp() resulted in data
                                 being written one byte past the end of the block.
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "unimp.h"
//#include "ProjectDotNet.h"

FILE *g_archfile;
char g_nopath=0,g_list=0;
uchar g_exprev,g_exlast,g_exerror;
int g_e8t,g_ovlevel,g_bitpos,g_mmptr,g_eof,g_retcode,
  g_ncodes[3]={NDCODES,ND2CODES,ND3CODES},
  g_ncoffset[3]={NLLCODES,NLLCODES+NDCODES,NLLCODES+NDCODES+ND2CODES};
uint g_exesize=0,g_e8extra;
uint32 g_crc,g_crctbl[256],g_dirfpos,g_overlap,g_prevfpos,
  g_unitoffset,g_exmemory,g_blockstart,g_blocksize,g_bitbuf,
  g_nextestart,g_mmlist[MAX_MM_CODES],
  g_overlaps[3][8]=
   {{32768,65536,73728,98304,122880,147456,172032,196608}, /*normal*/
    {32768,65536,73728,98304,122880,147456,172032,196608}, /*normal*/
    {32768,131072,196608,262144,327680,393216,458752,MAXOVERLAP}}; /*best*/
const uchar g_dirsig[]={0x49,0x4D,0x50,0x44,0x45,0};
uchar g_e8buf[8];
char g_archpath[MAX_PATH]="",g_outpath[MAX_PATH],g_dirbuf[IMPDIRBLOCK];
const char g_months[13][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
  "Sep","Oct","Nov","Dec","???"};
char *g_archname=g_archpath,*g_dirbufsrc,*g_dirbufend;
uchar *g_blockbuffer,*g_dcsrc,*g_dcend;
uchar g_dcbuffer[256];
void show_error(int error);
int parse_args(int argc,char **argv);
int extract_files(int arg1, int argc, char **argv, int onlycheck, void *pMemBuf);
int e8ut_crc_wr16(FILE *outfile,uchar *block,int32 size,uint32 e8offset);
int e8ut_crc_wr32(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize);
int extract_dir();

void *pOutMemBuf = NULL;
uint FileSize = 0;
uint CurPos = 0;

void fmyprintf(struct _iobuf *iob, char *mess, ...)
{

}
void myprintf(char *mess, ...)
{
	
}

void myfputs(char *mess, ...)
{
}

#define myfputc(a,b) 
/*void myfputc(char c)
{
}  */

int myfwrite(const void *pMem, size_t size, size_t count, FILE *pMemBuf)
{
	if (!pOutMemBuf) return (int)fwrite(pMem, size, count, pMemBuf);

	memcpy((uchar *)pOutMemBuf+CurPos, pMem, size*count);
	CurPos += (uint)size*count;

	return count;
}

int UNIMPmain(int argc, char **argv)
{
 int i,arg;
 char path[MAX_PATH];
 char *p;
 
 if(argc<2 || !parse_args(argc,argv))
  {
   fmyprintf(stderr,"Usage: unimp [-[jl] -o<output_path>] <archive name>\n"
     " -j       Junk stored directory info\n"
     " -l       List archive contents\n"
     " -o<path> Set output path\n\n");
   return RET_USERERR;
  }
 for(arg=1;arg<argc && argv[arg][0]==SWITCH_CHAR;arg++);
 if(arg>=argc)
  {myfputs("No archive name specified\n",stderr);
   return RET_USERERR;}
 strcpy_s(path,sizeof(path),argv[arg]);
 p=strrchr(path,PATH_CHAR);
 if(!p)p=path;
 p=strrchr(p,'.');
 if(!p)strcat_s(path,sizeof(path),".imp");
 if(fopen_s(&g_archfile,path,"rb"))g_archfile=NULL;
 if(!g_archfile)
  {fmyprintf(stderr,"Cannot open \"%s\" for reading.\n",path);
   return RET_OPENERR;}
 for(i=0;i<256;i++)
  {uint32 crc=i;
   char c;
   for(c=8;c;c--)crc= crc&1 ? 0xEDB88320L^(crc>>1) : crc>>1;
   g_crctbl[i]=crc;}
 i=extract_files(arg,argc,argv, 0, NULL);
 if(i<0)show_error(i);
 fclose(g_archfile);
 return g_retcode;
}

char *strcpyrd(char *dest, char *src)
{
 do *dest++=*src;
 while(*src++);
 return dest-1;
}

int parse_args(int argc,char **argv)
{
 int arg;
 for(arg=1;arg<argc;arg++)if(argv[arg][0]==SWITCH_CHAR)
  {
   char *src=argv[arg]+1;
   while(*src)switch(*src++)
    {
     case 'j':
       g_nopath=1;
       break;

     case 'l':
       g_list=1;
       break;

     case 'o':
      {
       char *p=strcpyrd(g_outpath,src);
       if(p>g_outpath && p[-1]!=PATH_CHAR)
        {*p++=PATH_CHAR;
         *p++=0;}
       *src=0;
       break;
      }

     default:
       return 0;
    }
  }
 return 1;
}

uint32 block_crc(void *block,int32 n,uint32 crc)
{
 uchar *src,*end;
 for(src=(uchar *)block,end=src+n;src<end;src++)
   crc=g_crctbl[(crc^*src)&0xFF]^(crc>>8);
 return crc;
}

#ifndef convert_path
void convert_path(char *path)
{
 for(;*path;path++)if(*path=='\\')*path=PATH_CHAR;
}
#endif

uint16 get_uint16(void *src)
{
 return *(uchar*)src+(((uchar*)src)[1]<<8);
}

uint32 get_uint32(void *src)
{
 int i;
 uint32 u=0;
 for(i=3;i>=0;i--)u=(u<<8)|((uchar*)src)[i];
 return u;
}

void init_getbits()
{
 g_dcsrc=g_dcend=g_dcbuffer;
 g_bitpos=0;
 g_bitbuf=0;
 g_eof=0;
}

uint32 getbits(int nbits)
{
 if(nbits>g_bitpos)while(g_bitpos<25)
  {
   if(g_dcsrc>=g_dcend)
    {
     g_dcsrc=g_dcbuffer;
     if(!g_eof)
      {g_dcend=g_dcbuffer+fread(g_dcbuffer,1,sizeof(g_dcbuffer),g_archfile);
       g_eof=feof(g_archfile);}
    }
   g_bitbuf+=(*g_dcsrc++)<<g_bitpos;
   g_bitpos+=8;
  }
 return g_bitbuf;
}

int32 unstore(uchar *output,int32 size)
{
 fseek(g_archfile,g_dcsrc-g_dcend-(g_bitpos>>3),SEEK_CUR);
 return fread(output,1,size,g_archfile);
}

void make_codes(uint *huffcodes,uchar *hclengths,int ncodes)
{
 uint i,j,k,l,count[MAXCODELEN+1],nextcode[MAXCODELEN+1];

 memset(count,0,sizeof(count));
 for(i=0;i<(uint)ncodes;i++)count[hclengths[i]]++;
 count[0]=0;
 for(i=1,k=0;i<=MAXCODELEN;i++)
  {
   k=(k+count[i-1])<<1;
   nextcode[i]=k;
  }
 for(i=0;i<(uint)ncodes;i++)
  {
   j=hclengths[i];
   if(j)
    {
     l=0;
     k=nextcode[j]++;
     for(;j>0;j--)
      {
       l<<=1;
       l+=k&1;
       k>>=1;
      }
     huffcodes[i]=l;
    }
  }
}

int make_decode_tables(int *ttable, int hdchain[][2], uint *huffcodes,
  uchar *hclengths, int ttbits, int ncodes)
{
 int hdnode,hdndest,i,ttsize;
 uint base,n,hcode;

 ttsize=1<<ttbits;
 if(hdchain)for(i=0;i<ncodes;i++)hdchain[i][0]=hdchain[i][1]=-MAX_NCODES;
 for(i=0;i<ttsize;i++)ttable[i]=-MAX_NCODES;
 hdndest=1;
 for(i=0;i<ncodes;i++)if(hclengths[i])
  {
   base=1<<hclengths[i];
   n=huffcodes[i];
   if(base<=(uint)ttsize)
    {
     while(n<(uint)ttsize) /*n should be less than ttsize but it might be invalid*/
      {
       ttable[n]=i;
       n+=base;
      }
    }
   else if(hdchain)
    {
     base=hclengths[i]-ttbits-1;
     n&=ttsize-1;
     hcode=huffcodes[i]>>ttbits;
     if(ttable[n]==-MAX_NCODES)
      {
       ttable[n]=-hdndest;
       hdnode=hdndest++;
      }
     else
      {
       hdnode=-ttable[n];
       while(hdchain[hdnode][hcode&1]>=0)
        {
         hdnode=hdchain[hdnode][hcode&1];
         hcode>>=1;
         base--;
        }
      }
     while(base--)
      {
       if(hdndest==ncodes)return EIMP_DAMAGED;
       hdchain[hdnode][hcode&1]=hdndest;
       hdnode=hdndest++;
       hcode>>=1;
      }
     hdchain[hdnode][hcode&1]=-i;
    }
  }
 return 0;
}

void load_fixed_codes(uchar *hclens,int32 exsize)
{
 memset(hclens,8,244);
 memset(hclens+244,9,17);
 hclens[256]=0;
 memset(hclens+261,10,5);
 memset(hclens+266,11,NLLCODES-266);
 if(exsize<=8192)
  {memset(hclens+NLLCODES,4,4);
   memset(hclens+NLLCODES+4,5,24);
   memset(hclens+NLLCODES+28,0,NDCODES-28);}
 else
  {memset(hclens+NLLCODES,5,22);
   memset(hclens+NLLCODES+22,6,NDCODES-22);}
 memset(hclens+NLLCODES+NDCODES,4,ND2CODES);
 hclens[NLLCODES+NDCODES]=3;
 hclens[NLLCODES+NDCODES+1]=3;
}

int load_hcodes(uchar *hclengths, int ncodes, int ntrees, int32 exsize)
{
 int i,j,dzrpt,zrpt,len,hcode,code;
 uchar hlclens[NLENCODES];
 uchar *hcldest,*hclend,*hclrpt;
 uint huffcodes[NLENCODES];
 int ttable[HCTTSIZE];
 int hdchain[NLENCODES][2];

 if(!ntrees){load_fixed_codes(hclengths,exsize);return 0;}
 memset(hlclens,0,NLENCODES);
 for(i=0,j=1;i<NLENCODES;)
  {
   code=getbits(5)&31;
   USEDBITS(5);
   if(code<2)
    {i+=j<<code;
     j<<=1;}
   else
    {if(code>MAXCODELEN+1)return EIMP_DAMAGED;
     hlclens[i++]=code-1;
     j=1;}
  }
 make_codes(huffcodes,hlclens,NLENCODES);
 make_decode_tables(ttable,hdchain,huffcodes,hlclens,HCTTBITS,NLENCODES);
 hcldest=hclengths;
 hclend=hclengths+ncodes*ntrees;
 hclrpt=hclengths+ncodes;
 len=0;dzrpt=1;zrpt=1;
 while(hcldest<hclend)
  {
   code=ttable[getbits(HCTTBITS)&(HCTTSIZE-1)];
   if(code<0)
    {
     if(code==-MAX_NCODES)return EIMP_DAMAGED;
     hcode=getbits(MAXCODELEN)>>HCTTBITS;
     code=-code;
     do
      {
       code=hdchain[code][hcode&1];
       hcode>>=1;
      }
     while(code>0);
     if(code==-MAX_NCODES)return EIMP_DAMAGED;
     code=-code;
    }
   USEDBITS(hlclens[code]);
   if(code>=DZRUN)
    {
     code=dzrpt<<(code-DZRUN);
     while(code-- && hcldest<hclend)
      {
       if(hcldest>=hclrpt && hcldest[-ncodes])len=hcldest[-ncodes];
       *hcldest++=len;
      }
     dzrpt<<=1;
     zrpt=1;
    }
   else if(code>=ZRUN)
    {
     code=zrpt<<(code-ZRUN);
     while(code-- && hcldest<hclend)*hcldest++=0;
     zrpt<<=1;
     dzrpt=1;
    }
   else
    {
     if(hcldest>=hclrpt && hcldest[-ncodes])len=code+hcldest[-ncodes];
     else len+=code;
     if(len>MAXCODELEN)len-=MAXCODELEN+1;
     *hcldest++=len;
     dzrpt=1;zrpt=1;
    }
  }
 for(hcldest=hclengths;hcldest<hclend;hcldest++)if(*hcldest>MAXCODELEN)return EIMP_DAMAGED;
 return 0;
}

void reverse_mm(uchar *block,int32 bsize)
{
 int m,d;
 uchar *src,*end;
 
 for(m=0;m<g_mmptr;m++)
  {
   d=-(int)(g_mmlist[m]>>24);
   if(!d)continue;
   src=block+(g_mmlist[m]&0xFFFFFF);
   end=block+bsize;
   if(m<g_mmptr-1)end=block+(g_mmlist[m+1]&0xFFFFFF);
   while(src<end)*src+=src[d],src++;
  }
}

void redo_mm(uint32 overlap)
{
 int mmptr,d;
 uchar *start,*src,*end;
 
 start=g_blockbuffer+g_blockstart-overlap;
 if(start<g_blockbuffer)start=g_blockbuffer;
 for(mmptr=g_mmptr-1;mmptr>=0;mmptr--)
  {
   d=-(int)(g_mmlist[mmptr]>>24);
   if(!d)continue;
   end=g_blockbuffer+(g_mmlist[mmptr]&0xFFFFFF);
   if(end<start)end=start;
   src=g_blockbuffer+g_blockstart;
   if(mmptr<g_mmptr-1)src=g_blockbuffer+(g_mmlist[mmptr+1]&0xFFFFFF);
   for(src--;src>=end;src--)*src-=src[d];
   if(end<=start)break;
  }
}

int32 expand_lz(uchar *output, int32 start, int32 exsize, int mm)
{
 uint huffcodes[MAX_NCODES];
 int llcode_table[LLTTSIZE],distcode_table[3][DTTSIZE];
 int llcode_chain[NLLCODES][2],distcode_chain[3][NDCODES][2];
 uint32 dist_base[NDCODES-6],dist_mask[NDCODES-6];
 uchar dist_extra[NDCODES-6],len_base[NLLCODES-268],len_mask[NLLCODES-268],
   len_extra[NLLCODES-268];
 uchar *dest,*output_end;
 int i,j,treenum,code,num_dist_trees,dist_split,numcodes,numtrees;
 uint len;
 uint32 hcode,dist,last_dist,last_dist2;
 uchar *code_lengths,*codelen_src;

 if(mm)g_mmptr=0;
 num_dist_trees=getbits(2)&3;
 USEDBITS(2);
 if(!num_dist_trees)return EIMP_DAMAGED;
 dist_split=num_dist_trees>2;
 numcodes=NLLCODES+NDCODES+ND2CODES;
 if(dist_split)numcodes+=ND3CODES;
 dest=output+start;
 output_end=dest+exsize;
 numtrees=getbits(6);
 USEDBITS(6);
 numtrees&=63;
 if(numtrees)code_lengths=(uchar *)malloc(numcodes*numtrees);
 else code_lengths=(uchar *)malloc(MIN_LZ_CODES);
 if(!code_lengths)return EIMP_OUTOFMEMORY;
 i=load_hcodes(code_lengths,numcodes,numtrees,start+exsize);
 if(i<0){free(code_lengths);return i;}
 for(i=2,len=11,code=0; code<NLLCODES-268; i<<=1)
   for(j=code+4; code<j; len+=i)len_base[code++]=len;
 for(i=1,code=0; code<NLLCODES-268; i++)
   for(j=code+4; code<j; code++)len_extra[code]=i;
 for(code=0,len=1; code<NLLCODES-268; len=(len<<1)+1)
   for(j=code+4; code<j; code++)len_mask[code]=len;
 for(hcode=2,dist=5,code=0; code<NDCODES-6; hcode<<=1)
  {
   dist_base[code]=dist; dist+=hcode;
   dist_base[code+1]=dist; dist+=hcode;
   code+=2;
  }
 for(i=1,code=0; code<NDCODES-6; i++,code+=2)
   dist_extra[code]=dist_extra[code+1]=i;
 for(code=0,dist=1; code<NDCODES-6; dist=(dist<<1)+1,code+=2)
   dist_mask[code]=dist_mask[code+1]=dist;
 codelen_src=code_lengths-numcodes;
 last_dist=0;last_dist2=0;
 code=256;
 numtrees+=!numtrees;
 while(dest<output_end)
  {
   if(code==256)
    {
     if(!numtrees)break;
     numtrees--;
     codelen_src+=numcodes;
     make_codes(huffcodes,codelen_src,NLLCODES);
     make_decode_tables(llcode_table,llcode_chain,huffcodes,codelen_src,LLTTBITS,NLLCODES);
     codelen_src+=NLLCODES;
     for(i=0;i<num_dist_trees;i++)
      {
       make_codes(huffcodes,codelen_src,g_ncodes[i]);
       make_decode_tables(distcode_table[i],distcode_chain[i],huffcodes,codelen_src,DTTBITS,g_ncodes[i]);
       codelen_src+=g_ncodes[i];
      }
     codelen_src-=numcodes;
     code=0;
    }
   code=llcode_table[getbits(LLTTBITS)&(LLTTSIZE-1)];
   if(code<0)
    {
     if(code==-MAX_NCODES)break;
     hcode=getbits(MAXCODELEN)>>LLTTBITS;
     code=-code;
     do
      {
       code=llcode_chain[code][hcode&1];
       hcode>>=1;
      }
     while(code>0);
     if(code==-MAX_NCODES)break;
     code=-code;
    }
   USEDBITS(codelen_src[code]);
   if(code<256)*dest++=code;
   else if(code>256)
    {
     if(code<266)len=code-255;
     else if(code<LONGMATCHCODE)
      {
       len=getbits(5);
       USEDBITS(len_extra[code-266]);
       len=(len&len_mask[code-266])+len_base[code-266];
      }
     else if(code==LONGMATCHCODE)
      {
       len=(getbits(LONGMATCHBITS)&LONGMATCHMASK)+259;
       USEDBITS(LONGMATCHBITS);
      }
     else
      {
       len=getbits(3)&7;
       USEDBITS(3);
       g_mmlist[g_mmptr++]=(dest-output)|(len<<24);
       continue;
      }
     treenum=(len==2)+((dist_split&(len==3))<<1);
     code=distcode_table[treenum][getbits(DTTBITS)&(DTTSIZE-1)];
     if(code<0)
      {
       if(code==-MAX_NCODES)break;
       hcode=getbits(MAXCODELEN)>>DTTBITS;
       code=-code;
       do
        {
         code=distcode_chain[treenum][code][hcode&1];
         hcode>>=1;
        }
       while(code>0);
       if(code==-MAX_NCODES)break;
       code=-code;
      }
     USEDBITS(codelen_src[code+g_ncoffset[treenum]]);
     if(!code)dist=last_dist;
     else if(code==1)
      {
       dist=last_dist2;
       last_dist2=last_dist;
       last_dist=dist;
      }
     else
      {
       if(code<6)dist=code-1;
       else
        {
         dist=getbits(18);
         USEDBITS(dist_extra[code-6]);
         dist=(dist&dist_mask[code-6])+dist_base[code-6];
         last_dist2=last_dist;
         last_dist=dist;
        }
      }
     if(dist<=(uint32)(dest-output))
       while(len-- && dest<output_end)*dest=dest[-(int32)dist],dest++;
    }
  }
 free(code_lengths);
 if(mm)reverse_mm(output,start+exsize);
 return dest-output-start;
}

int expand_bwt(uchar *output, int32 exsize)
{
 int32 count[257];
 uchar hclengths[BWTABLES][NCODES];
 uint huffcodes[NCODES];
 int ttable[BWTABLES][BWTTSIZE];
 int hdchain[BWTABLES][NCODES][2];
 uchar mtf[256],treemtf[BWTABLES];
 uint32 *links;
 uchar *dest,*output_end;
 int i,treenum,code,codes,numtrees;
 int32 rptbase,rpt,link,start;
 uint hcode;
 uchar rc;
 
 rc=getbits(8)&0xFF;
 USEDBITS(8);
 numtrees=getbits(3)&7;
 USEDBITS(3);
 if(numtrees<1)return EIMP_DAMAGED;
 for(treenum=0;treenum<numtrees;treenum++)
  {
   i=load_hcodes(hclengths[treenum],NCODES,1,0);
   if(i<0)return i;
   make_codes(huffcodes,hclengths[treenum],NCODES);
   make_decode_tables(ttable[treenum],hdchain[treenum],huffcodes,hclengths[treenum],BWTTBITS,NCODES);
  }
 g_bitbuf>>=g_bitpos&7;
 g_bitpos&=~7;
 for(i=0;i<256;i++)mtf[i]=i;
 for(i=0;i<BWTABLES;i++)treemtf[i]=i;
 memset(count,0,sizeof(count));
 rptbase=1;codes=WINDOW;
 dest=output;
 output_end=output+exsize+1;
 start=0; /*prevent crash from invalid data*/
 while(dest<output_end)
  {
   if(codes==WINDOW)
    {
     hcode=getbits(numtrees);
     code=0;
     while(hcode&1){hcode>>=1;code++;}
     if(code>=numtrees)return EIMP_DAMAGED;
     USEDBITS(code+1);
     treenum=treemtf[code];
     if(code)
      {
       for(;code;code--)treemtf[code]=treemtf[code-1];
       treemtf[0]=treenum;
      }
     codes=0;
     continue;
    }
   codes++;
   hcode=getbits(BWTTBITS)&(BWTTSIZE-1);
   code=ttable[treenum][hcode];
   if(code<0)
    {
     if(code==-MAX_NCODES)return EIMP_DAMAGED;
     hcode=getbits(MAXCODELEN)>>BWTTBITS;
     code=-code;
     do
      {
       code=hdchain[treenum][code][hcode&1];
       hcode>>=1;
      }
     while(code>0);
     if(code==-MAX_NCODES)return EIMP_DAMAGED;
     code=-code;
    }
   USEDBITS(hclengths[treenum][code]);
   if(code<2)
    {
     rpt=rptbase<<code;
     if(dest+rpt>output_end)rpt=output_end-dest;
     i=mtf[0];
     count[i+1]+=rpt;
     while(rpt--)*dest++=i;
     rptbase<<=1;
    }
   else if(code<257)
    {
     i=mtf[--code];
     *dest++=i;
     for(;code;code--)mtf[code]=mtf[code-1];
     mtf[0]=i;
     count[i+1]++;
     rptbase=1;
    }
   else
    {
     start=dest-output;
     dest++;
     rptbase=1;
    }
  }
 count[0]=1;
 for(i=1;i<256;i++)count[i]+=count[i-1];
 links=(uint32 *)malloc((exsize+1)*sizeof(int32));
 if(!links)return EIMP_OUTOFMEMORY;
 for(link=0;link<=exsize;link++)
   if(link!=start)links[link]=(output[link]<<24)|count[output[link]]++;
 links[start]=0;
 for(link=0,dest=output_end-2;dest>=output;dest--)
  {
   link=links[link&0xFFFFFF];
   *dest=(link>>24)+rc;
  }
 free(links);
 return 0;
}

int e8ut_crc_write(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize,int last)
{
 int32 i,w;
 uchar *src,*src2,*end;

 if(g_e8extra)
  {
   i=sizeof(g_e8buf)-g_e8extra;
   memcpy(g_e8buf+g_e8extra,block,i>size ? size : i);
   if(i>size)block=g_e8buf,size+=g_e8extra,e8offset-=g_e8extra;
   else
    {
     if(g_e8t==2)i=e8ut_crc_wr16(outfile,g_e8buf,g_e8extra,e8offset-g_e8extra);
     else i=e8ut_crc_wr32(outfile,g_e8buf,g_e8extra,e8offset-g_e8extra,srcsize);
     if(i<0)return i;
     block+=i;
     e8offset+=i;
     size-=i;
    }
  }
 if(g_e8t==2)i=e8ut_crc_wr16(outfile,block,size-g_e8t,e8offset);
 else i=e8ut_crc_wr32(outfile,block,size-g_e8t,e8offset,srcsize);
 if(i<0)return i;
 end=block+size;
 src=end-g_e8t+i;
 src2=src;
 while(src2<end && (*src2!=0xE8 || last))g_crc=g_crctbl[(uchar)g_crc^*src2++]^(g_crc>>8);
 if(src2>src)
  {
   w=myfwrite(src,1,src2-src,outfile);
   if(w<src2-src)return EIMP_WRITE;
  }
 g_e8extra=end-src2;
 if(g_e8extra)memmove(g_e8buf,src2,g_e8extra);
 return 0;
}

int e8ut_crc_wr16(FILE *outfile,uchar *block,int32 size,uint32 e8offset)
{
 uchar buffer[16384];
 int32 w;
 uint u;
 uchar *src,*end,*dest;
 uchar c;

 src=block;
 end=block+size;
 dest=buffer;
 while(src<end)
  {
   if(end-src>sizeof(buffer)-3)end=src+sizeof(buffer)-3;
   while(src<end)
    {
     c=*dest++=*src++;
     g_crc=g_crctbl[(uchar)g_crc^c]^(g_crc>>8);
     if(c==0xE8)
      {
       u=get_uint16(src)-(uint16)(e8offset+(src-block));
       *dest=(uchar)u;
       dest[1]=(uchar)(u>>8);
       src+=2;
       g_crc=g_crctbl[(uchar)g_crc^*dest++]^(g_crc>>8);
       g_crc=g_crctbl[(uchar)g_crc^*dest++]^(g_crc>>8);
      }
    }
   w=myfwrite(buffer,1,dest-buffer,outfile);
   if(w<dest-buffer)return EIMP_WRITE;
   dest=buffer;
   end=block+size;
  }
 return src-end;
}

int e8ut_crc_wr32(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize)
{
 uchar buffer[16384];
 int32 i,j,w;
 uchar *src,*end,*end2,*dest;
 uchar c;

 src=block;
 end=block+size;
 dest=buffer;
 if(end-src>sizeof(buffer)-5)end=src+sizeof(buffer)-5;
 while(src<end)
  {
   if(end-src>sizeof(buffer)-5)end=src+sizeof(buffer)-5;
   while(src<end)
    {
     c=*dest++=*src++;
     g_crc=g_crctbl[(uchar)g_crc^c]^(g_crc>>8);
     if(c==0xE8)
      {
       i=get_uint32(src);
       j=e8offset+(src-block);
       if(i<0 && i>=-j)i=srcsize-i-j-1;
       else if((uint32)i<(uint32)srcsize)i-=j;
       src+=4;
       for(end2=dest+4;dest<end2;dest++,i>>=8)
        {
         *dest=(uchar)i;
         g_crc=g_crctbl[(uchar)g_crc^*dest]^(g_crc>>8);
        }
      }
    }
   w=myfwrite(buffer,1,dest-buffer,outfile);
   if(w<dest-buffer)return EIMP_WRITE;
   dest=buffer;
   end=block+size;
  }
 return src-end;
}

int decomp(FILE *outfile,uint32 fpos,uint32 unitpos,uint32 togo,uchar flags)
{
 uchar lhbuf[LOCAL_HEADER_SIZE];
 uint16 namelen;
 int i,lhptr,nameptr;
 uint method,level,ovlevel;
 int32 nread,written;
 uint32 overlap,e8offset,srcsize,block_csize;
 uchar *blocksrc;
 
 if(!togo){g_crc=0;return 0;}
 g_crc=0xFFFFFFFF;
 g_e8extra=0;
 srcsize=togo;
 e8offset=0;
 g_exlast=0;
 lhptr=0;nameptr=0;
 g_e8t=0;
 if(flags&DFLAG_E8SHORT)g_e8t=2;
 else if(flags&DFLAG_E8LONG)g_e8t=4;
 fpos+=6+g_exesize;
 if(fpos!=g_prevfpos || unitpos<g_unitoffset)
  {
   g_blockstart=0;
   g_blocksize=0;
   g_exlast=0;
   g_exprev=0;
   g_unitoffset=0;
   g_exerror=0;
   g_mmptr=0;
   g_prevfpos=fpos;
   fseek(g_archfile,fpos,SEEK_SET);
   if(g_blockbuffer){free(g_blockbuffer);g_blockbuffer=NULL;}
  }
 while(togo)
  {
   if(g_exerror)return EIMP_DAMAGED;
   if(unitpos<g_unitoffset+g_blocksize)
    {
     blocksrc=g_blockbuffer+g_blockstart+unitpos-g_unitoffset;
     nread=g_blocksize-(unitpos-g_unitoffset);
     if(lhptr<LOCAL_HEADER_SIZE)
      {
       i=LOCAL_HEADER_SIZE-lhptr;
       if(i>nread)i=nread;
       memcpy(lhbuf+lhptr,blocksrc,i);
       lhptr+=i;
       unitpos+=i;
       nread-=i;
       if(lhptr<LOCAL_HEADER_SIZE)continue;
      }
     namelen=get_uint16(lhbuf+4);
     if(nameptr<namelen)
      {
       i=namelen-nameptr;
       if(i>nread)i=nread;
       nameptr+=i;
       unitpos+=i;
       continue;
      }
     if(nread>(int32)togo)nread=togo;
     if(!g_e8t)
      {
       g_crc=block_crc(blocksrc,nread,g_crc);
       written=myfwrite(blocksrc,1,nread,outfile);
       if(written<nread)return EIMP_WRITE;
      }
     else
      {
       i=e8ut_crc_write(outfile,blocksrc,nread,e8offset,srcsize,nread==(int32)togo);
       if(i<0)return i;
       written=nread;
       e8offset+=nread;
      }
     togo-=nread;
     unitpos+=nread;
    }
   if(togo)
    {
     g_exerror=1;
     if(g_exlast)return 0;
     g_blockstart+=g_blocksize;
     g_unitoffset+=g_blocksize;
     fpos=ftell(g_archfile);
     init_getbits();
     method=getbits(4)&15;
     USEDBITS(4);
     if(method>MAXMETHOD)return EIMP_DAMAGED;
     g_exlast=getbits(1)&1;
     USEDBITS(1);
     g_blocksize=getbits(20)&((1L<<20)-1);
     USEDBITS(20);
     block_csize=getbits(20)&((1L<<20)-1);
     USEDBITS(20);
     if(!g_blockbuffer)
      {
       g_exmemory=g_blocksize;
       g_blockbuffer=(uchar *)malloc(g_exmemory+512);
       if(!g_blockbuffer)return EIMP_OUTOFMEMORY;
      }
     if(method==METHOD_LZ77 || method==METHOD_LZ77_MM)
      {
       if(g_exprev)
        {
         level=getbits(2)&3;
         USEDBITS(2);
         if(level>2)return EIMP_DAMAGED;
         ovlevel=getbits(3)&7;
         USEDBITS(3);
         overlap=g_overlaps[level][ovlevel];
         redo_mm(overlap);
         if(g_blockstart>overlap)
          {
           memmove(g_blockbuffer,g_blockbuffer+g_blockstart-overlap,overlap);
           g_blockstart=overlap;
          }
         if(g_blocksize+g_blockstart>g_exmemory)
          {
           uchar *t;
           g_exmemory=g_blocksize+g_blockstart;
           t=(uchar *)realloc(g_blockbuffer,g_exmemory+512);
           if(!t){free(g_blockbuffer);g_blockbuffer=0;return EIMP_OUTOFMEMORY;}
           g_blockbuffer=t;
          }
        }
       else USEDBITS(5);
       written=expand_lz(g_blockbuffer,g_blockstart,g_blocksize,1);
       if(written<0)return written;
       if((uint32)written<g_blocksize){g_blocksize=(uint32)written;g_exlast=1;}
      }
     else
      {
       if(g_blocksize>g_exmemory)
        {
         uchar *t;
         g_exmemory=g_blocksize;
         t=(uchar *)realloc(g_blockbuffer,g_exmemory+512);
         if(!t){free(g_blockbuffer);return EIMP_OUTOFMEMORY;}
         g_blockbuffer=t;
        }
       if(!g_exlast)
        {
         overlap=MAXOVERLAP-g_blocksize;
         if((int32)overlap<0)overlap=0;
         else if(overlap>g_exmemory-g_blocksize)overlap=g_exmemory-g_blocksize;
         redo_mm(overlap);
         if(g_blockstart>overlap)
          {
           memmove(g_blockbuffer,g_blockbuffer+g_blockstart-overlap,overlap);
           g_blockstart=overlap;
          }
        }
       else g_blockstart=0;
       g_mmptr=0;
       if(method==METHOD_BWT)
        {
         i=expand_bwt(g_blockbuffer+g_blockstart,g_blocksize);
         if(i<0)return i;
        }
       else
        {
         i=unstore(g_blockbuffer+g_blockstart,g_blocksize);
         if(i<0)return i;
         if((uint32)i<g_blocksize){g_blocksize=(uint32)i;g_exlast=1;}
        }
      }
     fseek(g_archfile,fpos+block_csize,SEEK_SET);
     g_exerror=0;
     g_exprev=1;
    }
  }
 g_crc=~g_crc;
 return 0;
}

int find_start()
{
 char *buffer,*src,*end;
 int32 nread;
 
 fseek(g_archfile,0,SEEK_SET);
 buffer=(char *)malloc(32768);
 if(!buffer)return EIMP_OUTOFMEMORY;
 nread=fread(buffer,1,32768,g_archfile);
 if(nread<IMP_HEADER_SIZE)return EIMP_NOT_IMP;
 src=buffer;
 end=buffer+nread-IMP_HEADER_SIZE+1;
 g_exesize=0;
 do
  {
   src=(char *)memchr(src,IMP_SIG_0,end-src);
   if(!src)return EIMP_NOT_IMP;
   if(src[1]==IMP_SIG_1 && src[2]==IMP_SIG_2 && src[3]==IMP_SIG_3)
    {
     g_exesize=src-buffer;
     free(buffer);
     return 0;
    }
   src++;
  }
 while (src<end);
 return EIMP_NOT_IMP;
}

int get_dir_entry(IMP_DIR_ENTRY *pdirent,char *name,char *comment)
{
 int i;
 uint32 crc;
 
 if(g_dirbufsrc>=g_dirbufend)
  {
   int err=extract_dir();
   if(err<0)return err;
  }
 if(g_dirbufend-g_dirbufsrc<IMP_DIR_ENTRY_SIZE)return EIMP_BAD_DIR;
 pdirent->ver=get_uint16(g_dirbufsrc);
 pdirent->fpos=get_uint32(g_dirbufsrc+4);
 pdirent->commlen=g_dirbufsrc[10];
 pdirent->flags=g_dirbufsrc[11];
 pdirent->unitpos=get_uint32(g_dirbufsrc+12);
 pdirent->esize=get_uint32(g_dirbufsrc+16);
 pdirent->crc=get_uint32(g_dirbufsrc+20);
 pdirent->extra=get_uint16(g_dirbufsrc+24);
 pdirent->namelen=get_uint16(g_dirbufsrc+26);
 pdirent->mtime=get_uint16(g_dirbufsrc+32);
 pdirent->mdate=get_uint16(g_dirbufsrc+34);
 pdirent->entcrc=get_uint16(g_dirbufsrc+36);
 if((pdirent->ver&0xFFF)>MAX_VERSION)
  {
   fmyprintf(stderr,"This archive requires UnIMP version %d.%d or higher",
     (pdirent->ver&0xF00)>>8,pdirent->ver&0xFF);
   g_retcode=RET_OLDVERSION;
   return -1;
  }
 i=IMP_DIR_ENTRY_SIZE+pdirent->namelen+pdirent->commlen+pdirent->extra;
 if(g_dirbufend-g_dirbufsrc<i)return EIMP_BAD_DIR;
 g_dirbufsrc[36]=0;
 g_dirbufsrc[37]=0;
 crc=block_crc(g_dirbufsrc,i,0xFFFFFFFF);
 if((~crc&0xFFFF)!=pdirent->entcrc)return EIMP_BAD_DIR;
 memcpy(name,g_dirbufsrc+IMP_DIR_ENTRY_SIZE,pdirent->namelen);
 name[pdirent->namelen]=0;
 convert_path(name);
 memcpy(comment,g_dirbufsrc+IMP_DIR_ENTRY_SIZE+pdirent->namelen,pdirent->commlen);
 comment[pdirent->commlen]=0;
 g_dirbufsrc+=i;
 return 0;
}

int extract_dir()
{
 int i,method;
 uint32 fpos,csize,exsize;
 char sig[8];
 
 fpos=ftell(g_archfile);
 fseek(g_archfile,g_dirfpos,SEEK_SET);
 g_dirbufsrc=g_dirbuf;
 g_dirbufend=g_dirbuf;
 if(!fread(sig,6,1,g_archfile))return EIMP_EOF;
 if(memcmp(sig,&g_dirsig,6)!=0)return EIMP_BAD_DIR;
 init_getbits();
 method=getbits(5)&15;
 USEDBITS(5);
 if(method>METHOD_LZ77)return EIMP_BAD_DIR;
 exsize=getbits(20)&((1L<<20)-1);
 USEDBITS(20);
 if(exsize>IMPDIRBLOCK)return EIMP_BAD_DIR;
 csize=getbits(20)&((1L<<20)-1);
 USEDBITS(20);
 if(method==METHOD_LZ77)
  {
   USEDBITS(5);
   i=expand_lz((uchar*)g_dirbuf,0,exsize,0);
  }
 else i=unstore((uchar*)g_dirbuf,exsize);
 g_dirfpos+=6+csize;
 fseek(g_archfile,fpos,SEEK_SET);
 if(i<0)return i;
 g_dirbufend+=i;
 return 0;
}

int show_impcomment(uint32 cmoffset)
{
 char impcomment[4176],chbuf[4];
 int len;

 fseek(g_archfile,g_exesize+cmoffset,SEEK_SET);
 if(!fread(chbuf,4,1,g_archfile))return EIMP_EOF;
 len=get_uint16(chbuf);
 if(len>sizeof(impcomment)-1)len=sizeof(impcomment)-1;
 if(!fread(impcomment,1,len,g_archfile))return EIMP_EOF;
 if((uint16)~block_crc(impcomment,len,0xFFFFFFFF)!=get_uint16(chbuf+2))return EIMP_DAMAGED;
 impcomment[len]=0;
 myfputc('\n',stdout);
 myfputs(impcomment,stdout);
 myfputc('\n',stdout);
 return 0;
}

void list_dirent(IMP_DIR_ENTRY *pdirent,char *name)
{
 uint month=((pdirent->mdate>>5)&15)-1;
 if(month>12)month=12;
 myprintf("%10u %s %02d %04d %2d:%02d %s\n",pdirent->esize,
   g_months[month], pdirent->mdate&31, ((pdirent->mdate>>9)&127)+1980,
   (pdirent->mtime>>11)&31, (pdirent->mtime>>5)&63, name);
}

int extract_files(int arg1, int argc, char **argv, int onlycheck, void *pMemBuf)
{
 char name[MAX_PATH],comment[MAXFILECOMMENT+1],path[MAX_PATH];
 uchar ihbuf[IMP_HEADER_SIZE];
 IMP_HEADER ih;
 IMP_DIR_ENTRY dirent;
 int err;
 int32 errcount,ent;
 uint32 archsize;
 char *dest;
 
 errcount=0;
 fseek(g_archfile,0,SEEK_END);
 archsize=ftell(g_archfile);
 if(archsize<sizeof(IMP_HEADER))return EIMP_NOT_IMP;
 fseek(g_archfile,0,SEEK_SET);
 if(!fread(ihbuf,4,1,g_archfile))return EIMP_EOF;
 fseek(g_archfile,0,SEEK_SET);
 ih.sig=get_uint32(ihbuf);
 if(ih.sig!=IMP_SIG)
  {
   err=find_start();
   if(err<0)return err;
   if(g_exesize+IMP_HEADER_SIZE>archsize)return EIMP_EOF;
   fseek(g_archfile,g_exesize,SEEK_SET);
  }
 if(!fread(ihbuf,IMP_HEADER_SIZE,1,g_archfile))return EIMP_EOF;

 if (onlycheck) return 0;

 ih.diroffset=get_uint32(ihbuf+4);
 ih.direntries=get_uint32(ihbuf+8);
 ih.cmoffset=get_uint32(ihbuf+16);
 ih.flags=get_uint16(ihbuf+38);
 ih.crc=get_uint16(ihbuf+40);
 ihbuf[40]=0;
 ihbuf[41]=0;
 if((~block_crc(ihbuf,IMP_HEADER_SIZE,0xFFFFFFFF)&0xFFFF)!=ih.crc)return EIMP_DAMAGED;
 if(!ih.direntries)return 0;
 if(ih.flags&HFLAG_MULTIVOL)return EIMP_NOMULTIVOL;
 if(ih.flags&HFLAG_BLOWFISH)return EIMP_NOCIPHER;
 if(ih.cmoffset)
  {
   if(g_exesize+ih.cmoffset>archsize)return EIMP_EOF;
   err=show_impcomment(ih.cmoffset);
   if(err<0)return err;
  }
 g_blockbuffer=0;g_prevfpos=0xFFFFFFFF;
 g_dirbufsrc=g_dirbufend=g_dirbuf;
 g_dirfpos=ih.diroffset+g_exesize;
 if(g_dirfpos>archsize)return EIMP_EOF;
 fseek(g_archfile,g_dirfpos,SEEK_SET);
 if(g_list && ih.direntries)myfputs(
    "      Size   Last Modified   Name\n"
    "---------- ----------------- ------------------------------------------------\n",stdout);
 for(ent=0;ent<(int32)ih.direntries;ent++)
  {
   FILE *outfile;
   char *p=0;
   err=get_dir_entry(&dirent,name,comment);
   if(err<0)return err;
   dest=path;
   if(g_outpath)dest=strcpyrd(path,g_outpath);
   if(g_nopath)
    {p=strrchr(name,PATH_CHAR);
     if(p)p++;}
   strcpy_s(dest,0xFF*4,p ? p : name);
   if(dirent.flags&DFLAG_DIR)
    {
     //if(!g_list && !g_nopath)mkdir(path);
     continue;
    }
   if(g_list)list_dirent(&dirent,name);
   else
    {
     if(fopen_s(&outfile,path,"rb"))outfile=NULL;
     if(outfile)
      {fclose(outfile);
       fmyprintf(stderr,"File \"%s\" exists, skipping.\n",path);
       continue;}
     if(fopen_s(&outfile,path,"wb"))outfile=NULL;
     if(!outfile)
      {fmyprintf(stderr,"Cannot create \"%s\"\n",path);
       g_retcode=RET_IO;
       continue;}
     myprintf("Extracting \"%s\"",path);
     if(*comment)myprintf(" - %s\n",comment);
     else myfputc('\n',stdout);
     err=decomp(outfile,dirent.fpos,dirent.unitpos,dirent.esize,dirent.flags);
     fclose(outfile);
     if(err<0)
      {
       if(err==EIMP_WRITE)
        {fmyprintf(stderr,"Cannot write \"%s\".\n",path);
         g_retcode=RET_IO;
         return 0;}
       return err;
      }
     if(g_crc!=dirent.crc)
      {myfputs("File fails CRC error check\n",stderr);
       g_retcode=RET_DAMAGED;
       errcount++;}
    }
  }
 if(errcount)fmyprintf(stderr,"%ld error%s found.\n",errcount,errcount==1 ? "" : "s");
 return 0;
}

void show_error(int error)
{
 switch(error)
  {
   case EIMP_OUTOFMEMORY:
     myfputs("Out of memory.\n",stderr);
     g_retcode=RET_OUTOFMEMORY;
     break;
   case EIMP_DAMAGED:
     myfputs("Archive is damaged.\n",stderr);
     g_retcode=RET_DAMAGED;
     break;
   case EIMP_BAD_DIR:
     myfputs("Archive directory is damaged.\n",stderr);
     g_retcode=RET_DAMAGED;
     break;
   case EIMP_NOT_IMP:
     myfputs("Not an IMP archive.\n",stderr);
     g_retcode=RET_NOT_IMP;
     break;
   case EIMP_EOF:
     myfputs("Unexpected end of file.\n",stderr);
     g_retcode=RET_DAMAGED;
     break;
   case EIMP_NOMULTIVOL:
     myfputs("Multiple-volume IMP archives not supported.\n",stderr);
     break;
   case EIMP_NOCIPHER:
     myfputs("Deciphering not supported.\n",stderr);
     break;
  }
}

int extract_file(char *packfilename, int onlycheck, void **pMemBuf, char *filename, uint32 *SizeInBytes)
{
 char name[MAX_PATH],comment[MAXFILECOMMENT+1],path[MAX_PATH];
 uchar ihbuf[IMP_HEADER_SIZE];
 IMP_HEADER ih;
 IMP_DIR_ENTRY dirent;
 int err;
 int32 errcount,ent;
 uint32 archsize;
 char *dest;
 uint32 i = 0;

 if(fopen_s(&g_archfile,packfilename,"rb"))g_archfile=NULL;
 if(!g_archfile)
	 return -1;
 
 for(i=0;i<256;i++)
 {uint32 crc=i;
 char c;
 for(c=8;c;c--)crc= crc&1 ? 0xEDB88320L^(crc>>1) : crc>>1;
 g_crctbl[i]=crc;}

 errcount=0;
 fseek(g_archfile,0,SEEK_END);
 archsize=ftell(g_archfile);
 if(archsize<sizeof(IMP_HEADER)) {fclose(g_archfile);return EIMP_NOT_IMP;}
 fseek(g_archfile,0,SEEK_SET);
 if(!fread(ihbuf,4,1,g_archfile)){fclose(g_archfile);return EIMP_EOF;}
 fseek(g_archfile,0,SEEK_SET);
 ih.sig=get_uint32(ihbuf);
 g_exesize = 0;
 if(ih.sig!=IMP_SIG)
  {
   err=find_start();
   if(err<0) {fclose(g_archfile);return err;}
   if(g_exesize+IMP_HEADER_SIZE>archsize){fclose(g_archfile);return EIMP_EOF;}
   fseek(g_archfile,g_exesize,SEEK_SET);
  }
 if(!fread(ihbuf,IMP_HEADER_SIZE,1,g_archfile)){fclose(g_archfile);return EIMP_EOF;}

 if (onlycheck)
 {
	 fclose(g_archfile);
	 return 0;
 }

 ih.diroffset=get_uint32(ihbuf+4);
 ih.direntries=get_uint32(ihbuf+8);
 ih.cmoffset=get_uint32(ihbuf+16);
 ih.flags=get_uint16(ihbuf+38);
 ih.crc=get_uint16(ihbuf+40);
 ihbuf[40]=0;
 ihbuf[41]=0;
 if((~block_crc(ihbuf,IMP_HEADER_SIZE,0xFFFFFFFF)&0xFFFF)!=ih.crc)return EIMP_DAMAGED;
 if(!ih.direntries)return 0;
 if(ih.flags&HFLAG_MULTIVOL)return EIMP_NOMULTIVOL;
 if(ih.flags&HFLAG_BLOWFISH)return EIMP_NOCIPHER;
 if(ih.cmoffset)
  {
   if(g_exesize+ih.cmoffset>archsize)return EIMP_EOF;
   err=show_impcomment(ih.cmoffset);
   if(err<0)return err;
  }
 g_blockbuffer=0;g_prevfpos=0xFFFFFFFF;
 g_dirbufsrc=g_dirbufend=g_dirbuf;
 g_dirfpos=ih.diroffset+g_exesize;
 if(g_dirfpos>archsize)return EIMP_EOF;
 fseek(g_archfile,g_dirfpos,SEEK_SET);
 if(g_list && ih.direntries)myfputs(
    "      Size   Last Modified   Name\n"
    "---------- ----------------- ------------------------------------------------\n",stdout);
 for(ent=0;ent<(int32)ih.direntries;ent++)
  {
   FILE *outfile;
   char *p=0;
   err=get_dir_entry(&dirent,name,comment);
   if (filename)
   {
	   int res;
	   if (res = _stricmp(name, filename)) continue;

   }
   if(err<0)return err;
   dest=path;
   if(g_outpath)dest=strcpyrd(path,g_outpath);
   if(g_nopath)
    {p=strrchr(name,PATH_CHAR);
     if(p)p++;}
   strcpy_s(dest,0xFF*4,p ? p : name);
   if(dirent.flags&DFLAG_DIR)
    {
     //if(!g_list && !g_nopath)mkdir(path);
     continue;
    }
   if(g_list)list_dirent(&dirent,name);
   else
    {
	   outfile = NULL;
	   if (*pMemBuf)
	   {
		   if(fopen_s(&outfile,path,"rb"))outfile=NULL;
		   if(outfile)
		   {fclose(outfile);
		   fmyprintf(stderr,"File \"%s\" exists, skipping.\n",path);
		   continue;}
		   if(fopen_s(&outfile,path,"wb"))outfile=NULL;
		   if(!outfile)
		   {fmyprintf(stderr,"Cannot create \"%s\"\n",path);
		   g_retcode=RET_IO;
		   continue;}
	   }
     myprintf("Extracting \"%s\"",path);
     if(*comment)myprintf(" - %s\n",comment);
     else myfputc('\n',stdout);
	 *pMemBuf = malloc(dirent.esize);
	 pOutMemBuf = *pMemBuf;
	 *SizeInBytes = FileSize = dirent.esize;
	 CurPos = 0;
     err=decomp(outfile,dirent.fpos,dirent.unitpos,dirent.esize,dirent.flags);
     if (outfile) fclose(outfile);
     if(err<0)
      {
       if(err==EIMP_WRITE)
        {fmyprintf(stderr,"Cannot write \"%s\".\n",path);
         g_retcode=RET_IO;
         return 0;}
       return err;
      }
     if(g_crc!=dirent.crc)
      {myfputs("File fails CRC error check\n",stderr);
       g_retcode=RET_DAMAGED;
       errcount++;}
    }
    if (!err) break;
  }
 fclose(g_archfile);
 if (!*pMemBuf) return -1;
 if(errcount)fmyprintf(stderr,"%ld error%s found.\n",errcount,errcount==1 ? "" : "s");
 return 0;
}


/*struct UniPack
{
	DWORD ID;
	char PackName[0xFF];
	DWORD NumFiles;
	LPCSTR Files;
};

DWORD GetPackID(LPCSTR PackName)
{
}

void MapFiles(DWORD id)
{
};

bool IsFileExists(DWORD id,LPCSTR Name)
{
	return false;
}
*/
