#pragma once
#ifndef _H_XCONTROLLER_H_
#define _H_XCONTROLLER_H_

#include <stddef.h>
#include <stdint.h>

#ifdef WIN32
#if defined(XCONTROLLER_EXPORTS) || defined(XController_EXPORTS)
#define XCONTROLLER_API __declspec(dllexport)
#else
#define XCONTROLLER_API __declspec(dllimport)
#endif
#else
#define XCONTROLLER_API
#endif//

#if defined(__cplusplus)
extern "C" {
#endif//

XCONTROLLER_API void XController_Start(const char* xml, size_t xmlflag);
XCONTROLLER_API void XController_Stop();
    
XCONTROLLER_API void XController_SetHandler(const char* name, void* handler);
XCONTROLLER_API void* XController_GetHandler(const char* name);

XCONTROLLER_API void XController_SetThreadHandler(const char* name, void* handler);
XCONTROLLER_API void* XController_GetThreadHandler(const char* name);

XCONTROLLER_API void XController_SetData(const char* name, void* data);
XCONTROLLER_API void* XController_GetData(const char* name);

XCONTROLLER_API void XController_SetThreadData(const char* name, void* data);
XCONTROLLER_API void* XController_GetThreadData(const char* name);

#if defined(__cplusplus)
}

class XCONTROLLER_API XController
{
public:
    static XController& Instance();
    
    virtual void Start(const char* xml, size_t xmlflag = 0) = 0;
    virtual void Stop() = 0;
    
    virtual void SetHandler(const char* name, void* handler) = 0;
    virtual void* GetHandler(const char* name) = 0;

    virtual void SetThreadHandler(const char* name, void* handler) = 0;
    virtual void* GetThreadHandler(const char* name) = 0;

    virtual void SetData(const char* name, void* data) = 0;
    virtual void* GetData(const char* name) = 0;

    virtual void SetThreadData(const char* name, void* data) = 0;
    virtual void* GetThreadData(const char* name) = 0;
};

#endif//

#endif//_H_XCONTROLLER_H_