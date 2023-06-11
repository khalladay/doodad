#pragma once
#include <stdint.h>
#include <Windows.h>
#include <vector>

// DirectX includes
#include <d3d11.h>
#include <DirectXColors.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "winmm.lib")

#define APP_NAME "Doodad"

#define DEFAULT_SCREEN_W 480 
#define DEFAULT_SCREEN_H 320

#define PATH_SEPARATOR "\\"

#include "doodad_app.h"
#include "input_handler.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

const float PI = 3.14159265f;
const float DEG2RAD = (PI * 2) / 360.0f;

#if _DEBUG
#include <debugapi.h>
#include <stdio.h>

#define NDEBUG 1

#define checkf(expr, format, ...) if (!(expr))																\
{																											\
    fprintf(stdout, "CHECK FAILED: %s:%ld:%s " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);	\
	MessageBoxA(NULL, "CHECK FAILED", NULL, MB_OK);\
	DebugBreak();																							\
}

#define check(expr) if (!(expr))																\
{																											\
    fprintf(stdout, "CHECK FAILED: %s:%ld:%s\n", __FILE__, __LINE__, __func__);	\
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

