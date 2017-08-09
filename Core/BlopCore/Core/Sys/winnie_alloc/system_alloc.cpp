//#include "stdafx.h"
//system_alloc.cpp
//OS-specific memory allocation functions
//this is part of winnie_alloc library
//you may rewrite this functions if you like
#include "projectspace.h"//stdafx.h"
#include "system_alloc.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

void *SystemAlloc(size_t size)
{
  //char buf[100];
  //sprintf(buf, "SystemAlloc: %d \n", size);
  //OutputDebugString(buf);
  return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void SystemFree(void *p)
{
  VirtualFree(p, 0, MEM_RELEASE);
}

