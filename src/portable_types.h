#ifndef WSS_PORTABLE_TYPES_HEADER
#define WSS_PORTABLE_TYPES_HEADER

#include <stdio.h>
#include <time.h>
#include <string>

#define SET_NO_COPY(TypeName) TypeName(const TypeName&) = delete;void operator=(const TypeName&) = delete;

////////////////////////////////
//  COMPILER

#ifdef WSS_MSVC
#include <winsock2.h>
#include <windows.h>
#include <memory>
//
// Macros and Typedefs
//
#define STRING_IT(a) #a
#define TOKEN_IT(a) STRING_IT(##a)
#define MESSAGE(a) message (__FILE__ "(" TOKEN_IT(__LINE__) ") : " a)
typedef HINSTANCE LIBHANDLE;
#pragma once
#endif

#ifdef WSS_GCC
#define STRING_IT(a)
#define TOKEN_IT(a)
#define MESSAGE(a)
typedef void* LIBHANDLE;
#endif

/////////////////////////////////////
//  GENERAL OS
#ifdef WSS_MAC

#ifdef __cplusplus
	#define TRUE true
	#define FALSE false
#else
	#define TRUE 1
	#define FALSE 0
#endif

#elif WSS_LINUX

#ifdef __cplusplus
	#define TRUE true
	#define FALSE false
#else
	#define TRUE 1
	#define FALSE 0
#endif

#elif WSS_WIN

#endif

namespace wss {
	typedef time_t NativeTimeType;
	typedef std::basic_string<char*> wssString;
	typedef std::basic_string<wchar_t*> wssWString;
}

#endif
