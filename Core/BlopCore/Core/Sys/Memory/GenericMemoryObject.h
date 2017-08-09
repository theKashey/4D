#ifndef _CGenericMemoryObject__H_
#define _CGenericMemoryObject__H_
#pragma once

struct DLLTYPE MemoryStruct
{
public:
/*	 PVOID operator new     (size_t Size);
	 PVOID operator new[]   (size_t Size);

	 void  operator delete  (PVOID Src);
	 void  operator delete[](PVOID Src);
*/
};

struct DLLTYPE MemoryClass
{
public:
	/* PVOID operator new     (size_t Size);
	 PVOID operator new[]   (size_t Size);

	 void  operator delete  (PVOID Src);
	 void  operator delete[](PVOID Src);
	 */
};

#define mS public MemoryStruct
#define mC public MemoryClass

#endif
