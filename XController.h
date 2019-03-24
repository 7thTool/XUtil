#pragma once
#ifndef _H_XCONTROLLER_H_
#define _H_XCONTROLLER_H_

#include "XUtil.h"

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif//

XUTIL_API void XController_SetHandler(const char* name, void* handler);
XUTIL_API void* XController_GetHandler(const char* name);

XUTIL_API void XController_SetThreadHandler(const char* name, void* handler);
XUTIL_API void* XController_GetThreadHandler(const char* name);

XUTIL_API void XController_SetData(const char* name, void* data);
XUTIL_API void* XController_GetData(const char* name);

XUTIL_API void XController_SetThreadData(const char* name, void* data);
XUTIL_API void* XController_GetThreadData(const char* name);

#if defined(__cplusplus)
}

#define theCtl XUtil::XController::instance()

namespace XUtil
{

class XUTIL_API XController
{
public:
    static XController& instance();

    virtual void start(char* xml, int xmlflag = 0) = 0;
    virtual void stop() = 0;    
    
	virtual const char* work_path() = 0;
	virtual const char* data_path() = 0;
    
    virtual void SetHandler(const char* name, void* handler) = 0;
    virtual void* GetHandler(const char* name) = 0;

    virtual void SetThreadHandler(const char* name, void* handler) = 0;
    virtual void* GetThreadHandler(const char* name) = 0;

    virtual void SetData(const char* name, void* data) = 0;
    virtual void* GetData(const char* name) = 0;

    virtual void SetThreadData(const char* name, void* data) = 0;
    virtual void* GetThreadData(const char* name) = 0;
};

}

#endif//

#endif//_H_XCONTROLLER_H_
