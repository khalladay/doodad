#pragma once

#if _DEBUG 
#include <debugapi.h>
#include <stdio.h>
#include "logging.h"

#define NDEBUG 1

#define checkf(expr, format, ...) if (!(expr))																\
{																											\
    LOG("CHECK FAILED: %s:%ld:%s " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);	\
	MessageBoxA(NULL, "CHECK FAILED", NULL, MB_OK);\
	DebugBreak();																							\
}

#define check(expr) if (!(expr))																\
{																											\
    LOG("CHECK FAILED: %s:%ld:%s\n", __FILE__, __LINE__, __func__);	\
	MessageBoxA(NULL, "CHECK FAILED", NULL, MB_OK);\
	DebugBreak();																							\
}

#define mustDeref(ptr) ( (ptr == nullptr ? DebugBreak() : (void)0), *ptr)

#else
#undef NDEBUG
#define checkf(expr, format, ...);
#define check(expr);
#define mustDeref(ptr) (*ptr)
#endif

