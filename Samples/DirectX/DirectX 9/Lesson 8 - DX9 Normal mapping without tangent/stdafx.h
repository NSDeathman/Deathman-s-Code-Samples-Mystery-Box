///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
// Windows includes (You need to add $(WindowsSDK_IncludePath)
// to VS include paths of your project)
#include <iostream>
#include <ctime>
#include <windows.h>
#include <ppl.h>
///////////////////////////////////////////////////////////////
// Windows libraries
#pragma comment( lib, "winmm.lib")   
///////////////////////////////////////////////////////////////
// Includes (You need to add $(DXSDK_DIR)Include\ to VS 
// include paths of your project) 
#include <d3dx9.h>
///////////////////////////////////////////////////////////////
// Libraries (You need to add 
// $(DXSDK_DIR)Lib\$(LibrariesArchitecture)\ to VS include 
// paths of your project) 
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
///////////////////////////////////////////////////////////////
#define SAFE_RELEASE(x)		\
	{						\
		if (x)				\
		{					\
			(x)->Release();	\
			(x) = NULL;		\
		}					\
	}

#define SAFE_DELETE(p)       \
	{                        \
		if (p)               \
		{                    \
			delete (p);      \
			(p) = NULL;      \
		}                    \
	}

#define SAFE_DELETE_ARRAY(p)\
	{                       \
		if (p)              \
		{                   \
			delete[](p);    \
			(p) = NULL;     \
		}                   \
	}
///////////////////////////////////////////////////////////////
typedef char string1024[1024];
void __cdecl Print(LPCSTR format, ...)
{
	va_list mark;
	string1024 buf;
	va_start(mark, format);
	int sz = _vsnprintf_s(buf, sizeof(buf) - 1, format, mark);
	buf[sizeof(buf) - 1] = 0;
	va_end(mark);
	if (sz)
	{
		std::cout << (buf);
		std::cout << "\n";
	}
}

extern bool g_bNeedCloseApplication;
#define ERROR_MESSAGE_NAME "DX9 Sample.exe"
#define ERROR_MESSAGE(error_message, ...)											\
	{																				\
			MessageBox(NULL, error_message, ERROR_MESSAGE_NAME, MB_OK);				\
			g_bNeedCloseApplication = true;											\
	}																				\

#define ASSERT(expession, error_message, ...)										\
{																					\
	if (expession == false)															\
	{																				\
		Print(error_message);														\
		MessageBox(NULL, error_message, ERROR_MESSAGE_NAME, MB_OK);					\
		g_bNeedCloseApplication = true;												\
	}																				\
}																					\
///////////////////////////////////////////////////////////////