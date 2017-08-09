#define MAX_VERSION 0x10A
#define MAXMETHOD METHOD_LZ77_MM

#ifdef _WIN32
#define PATH_CHAR '\\'
#define convert_path(p)
#endif

#ifndef PATH_CHAR
#define PATH_CHAR '/'
#endif
#define SWITCH_CHAR '-'

#define MAX_PATH 260

#define IMP_SIG 0xA504D49
#define IMP_SIG_0 0x49
#define IMP_SIG_1 0x4D
#define IMP_SIG_2 0x50
#define IMP_SIG_3 0xA

#define IMPDIRBLOCK 8192
#define MAXOVERLAP 524288
#define MAXFILECOMMENT 78

#if INT_MAX>65535
typedef int int32;
typedef unsigned int uint32;
#else
typedef long int32;
typedef unsigned long uint32;
#endif
typedef short int16;
typedef unsigned int uint;
typedef unsigned short uint16;
typedef unsigned char uchar;

typedef struct
{
 uint32 sig;
 uint32 diroffset;
 uint32 direntries;
 uint32 ecoffset;
 uint32 cmoffset;
 uint32 length;
 uint32 csize;
 uint16 mvsig;
 uint16 disk;
 uint16 dirdisk;
 uint16 cmdisk;
 uint16 ecdisk;
 uint16 flags;
 uint16 crc;
} IMP_HEADER;

typedef struct
{
 uint16 ver;
 uint16 disk;
 uint32 fpos;
 uint16 attrib;
 uchar commlen;
 uchar flags;
 uint32 unitpos;
 uint32 esize;
 uint32 crc;
 uint16 extra;
 uint16 namelen;
 uint16 ctime;
 uint16 cdate;
 uint16 mtime;
 uint16 mdate;
 uint16 entcrc;
} IMP_DIR_ENTRY;

typedef struct
{
 uint32 esize;
 uint16 namelen;
 uint16 ver;
 uint16 entcrc;
 uchar flags;
} LOCAL_HEADER;

#define IMP_HEADER_SIZE 42   /*sizeof these structs probably wrong due to padding*/
#define IMP_DIR_ENTRY_SIZE 38
#define LOCAL_HEADER_SIZE 11

/*IMP_DIR_ENTRY flags*/
#define DFLAG_DIR 1
#define DFLAG_E8SHORT 2
#define DFLAG_E8LONG 4

/*IMP_HEADER flags*/
#define HFLAG_MULTIVOL 1
#define HFLAG_LASTVOL 2
#define HFLAG_BLOWFISH 4

/*methods*/
#define METHOD_STORE 0
#define METHOD_LZ77 1
#define METHOD_BWT 2
#define METHOD_LZ77_MM 3

/*LZ77*/
#define NLLCODES 288 /* Number of literal-length codes */
#define NDCODES 42   /* Number of distance codes */
#define ND2CODES 14  /* Number of distance codes for length==2 */
#define ND3CODES 28  /* Number of distance codes for length==3 */
#define MIN_LZ_CODES (NLLCODES+NDCODES+ND2CODES)
#define LONGMATCHBITS 15  /*extra bits for long match*/
#define LONGMATCHMASK ((1<<LONGMATCHBITS)-1)
#define LONGMATCHCODE (NLLCODES-2)
#define MMCODE (NLLCODES-1) /*multimedia code*/
#define LLTTBITS 9
#define DTTBITS 7
#define LLTTSIZE (1<<LLTTBITS)
#define DTTSIZE (1<<DTTBITS)
#define MAX_MM_CODES 1024

/*BWT*/
#define NCODES 258
#define WINDOW 50
#define BWTABLES 6
#define BWTTBITS 9
#define BWTTSIZE (1<<BWTTBITS)

#define MAX_NCODES NLLCODES
#define MAXCODELEN 16
#define NLENCODES (MAXCODELEN+5) /* no. of code length codes */
#define ZRUN (MAXCODELEN+1) /* run of zeros */
#define DZRUN (MAXCODELEN+3)/* run of delta-zeros*/
#define HC_MCL 14    /* max code length for storing Huffman codes */
#define HCTTBITS 6
#define HCTTSIZE (1<<HCTTBITS)

#define USEDBITS(n) {g_bitpos-=n;g_bitbuf>>=n;}

/*errors*/
#define EIMP_NOT_IMP -2
#define EIMP_DAMAGED -3
#define EIMP_BAD_DIR -4
#define EIMP_EOF -5
#define EIMP_WRITE -6
#define EIMP_NOCIPHER -7
#define EIMP_NOMULTIVOL -8
#define EIMP_BAD_CRC -9
#define EIMP_OUTOFMEMORY -10

/*return codes*/
#define RET_USERERR 1
#define RET_OLDVERSION 2
#define RET_DAMAGED 3
#define RET_NOT_IMP 4
#define RET_OPENERR 5
#define RET_OUTOFMEMORY 6
#define RET_IO 7

int extract_file(char *packfilename, int onlycheck, void **pMemBuf, char *filename, uint32 *SizeInBytes);
