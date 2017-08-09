//define strcat_s(a,c,b) strcat(a,b)
//define strcpy_s(a,c,b) strcpy(a,b)
//define strncpy_s(a,c,d,b) strncpy(a,d,b)

#ifdef __cplusplus
#define DOTInline inline 
#else
#define DOTInline
#include <stdarg.h>
#endif

DOTInline int sprintf_s(char *Buff,int BuffSize,const char *Format,...)
{
	va_list args;
    va_start(args, Format);
	return _vsnprintf(Buff,BuffSize,Format,args);
}

DOTInline int fopen_s(FILE **f,const char* F,const char* Format)
{
	*f=fopen(F,Format);
	if(*f) return 0;
	else return 1;
}