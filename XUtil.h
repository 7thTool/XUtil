#ifndef _H_XUTIL_H_
#define _H_XUTIL_H_

#pragma once

#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
    #ifdef _WIN64
        #warning "WIN64 os platform"
    #else
        #warning "WIN32 os platform"
    #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #warning "iOS Simulator os platform"
    #elif TARGET_OS_IPHONE
        #warning "iOS device os platform"
    #elif TARGET_OS_MAC
        #warning "other Mac OS os platform"
    #else
        #warning "unknown Apple os platform"
    #endif
#elif __ANDROID__
    #warning "android os platform"
#elif __linux__
    #warning "linux os platform"
#elif __unix__ // all unices not caught above
    #warning "unix os platform"
#elif defined(_POSIX_VERSION)
    #warning "POSIX os platform"
#else
    #error "unknown os platform"
#endif

#ifdef XUTIL_CONF
#include <XUtilConf.h>
#else
//#ifndef XUTIL_DLL
//#define XUTIL_DLL
//#endif//XUTIL_DLL
#endif//

#ifdef WIN32
#ifdef XUTIL_DLL
#ifdef XUtil_EXPORTS
#define XUTIL_API __declspec(dllexport)
#else
#define XUTIL_API __declspec(dllimport)
#endif//
#else
#define	XUTIL_API 
#endif//XUTIL_DLL
#else //LINUX
#define	XUTIL_API 
#endif//

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	
#endif//WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <memory.h>
#else //LINUX
#include <sys/types.h> 
#include <sys/syscall.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#ifdef __APPLE__
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h> 
#include <ctype.h>
#include <time.h>
#include <float.h>


typedef unsigned char byte;
typedef void* ptr_t;
typedef void* handle_t;
typedef ptrdiff_t int_t;
//ptrdiff_t
//ssize_t
//size_t

#ifdef WIN32
#define __STDCALL __stdcall
#else
#define __STDCALL __attribute__((stdcall))
#endif

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif//INFINITE

//#define _E				(2.7182818285)
//#define _e				_E
//#define _PI				(3.1415926536)
//#define _pi				_PI

#define	_zero_ctrl_			(1e-10)
#define	_over_ctrl_			(1e15)
#define _precision_			(1e-6)
#define _error_ctrl_		(1e16)
#define _4_5_ctrl_			(0.500001)
#define _decimal_digits_	(7)
#define _decimal_digitsex_	(15)

static bool IsZeroFloat(double v)	{
    return ((v)>-_zero_ctrl_ && (v)<_zero_ctrl_);
}	
static bool IsOverFloat(double v) {
    return ((v)<-_over_ctrl_ || (v)>_over_ctrl_);
}		
static bool IsEqualFloat(double x, double y) {
    return ((x)>(y)-_zero_ctrl_ && (x)<((y)+_zero_ctrl_));
}

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define _hasallof(s,f) (((s)&(f))==(f))
#define _hasoneof(s,f) ((s)&(f))

#define _reversepos(pos,cnt) ((cnt)-((pos)+1))

#define MAKEINT16(l, h)     ((uint16_t)(((uint8_t)(((uint16_t)(l)) & 0xff)) | ((uint16_t)((uint8_t)(((uint16_t)(h)) & 0xff))) << 8))
#define LOINT8(x)           ((uint8_t)(((uint16_t)(x)) & 0xff))
#define HIINT8(x)           ((uint8_t)((((uint16_t)(x)) >> 8) & 0xff))
#define MAKESHORT			MAKEINT16			
//#define MAKEWORD			MAKEINT16
//#define LOBYTE			LOINT8
//#define HIBYTE			HIINT8
#define MAKEINT32(l,h)		((uint32_t)(((uint16_t)(((uint32_t)(l)) & 0xffff)) | ((uint32_t)((uint16_t)(((uint32_t)(h)) & 0xffff))) << 16))
#define LOINT16(l)          ((uint16_t)(((uint32_t)(l)) & 0xffff))
#define HIINT16(l)          ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))
//#define MAKEDWORD			MAKEINT32
#define MAKEINT				MAKEINT32
//#define MAKELONG			MAKEINT32
//#define LOWORD			LOINT16
//#define HIWORD			HIINT16
#define LOSHORT				LOINT16
#define HISHORT				HIINT16
#define MAKEINT64(l,h)		((uint64_t)(((uint32_t)((uint64_t)(l) & 0xffffffff)) | ((uint64_t)((uint32_t)(((uint64_t)(h)) & 0xffffffff))) << 32))
#define LOINT32(x)          ((uint32_t)(((uint64_t)(x)) & 0xffffffff))
#define HIINT32(x)          ((uint32_t)((((uint64_t)(x)) >> 32) & 0xffffffff))
//#define MAKEDWORDLONG		MAKEINT64
#define MAKELONGLONG		MAKEINT64
//#define LODWORD			LOINT32
//#define HIDWORD			HIINT32
#define LOINT				LOINT32
#define HIINT				HIINT32
#define LOLONG				LOINT32
#define HILONG				HIINT32

#define rgb(r,g,b) ((uint32_t)(((byte)(r)|((uint16_t)((byte)(g))<<8))|(((uint16_t)(byte)(b))<<16)))
#define rgbr(rgb) (LOINT8(rgb))
#define rgbg(rgb) (LOINT8(((uint16_t)(rgb)) >> 8))
#define rgbb(rgb) (LOINT8((rgb)>>16))

#ifndef PRINTF
#if defined(_DEBUG) || 1
#define PRINTF(format,...) printf(format, ##__VA_ARGS__)
#define WPRINTF(format,...) wprintf(format, ##__VA_ARGS__)
#define PRINTFL(format,...) printf(format, ##__VA_ARGS__);printf("\n")
#define WPRINTFL(format,...) wprintf(format, ##__VA_ARGS__);wprintf(L"\n")
#else
#define PRINTF(format,...) 
#define WPRINTF(format,...) 
#define PRINTFL(format,...)
#define WPRINTFL(format,...)
#endif//
#endif//

int GetDecimalDigits(double v);

#endif//_H_XUTIL_H_

