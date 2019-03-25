#pragma once
#ifndef __H_XLOGGER_H_
#define __H_XLOGGER_H_

#include "XUtil.h"

#include <stddef.h>
#include <stdint.h>

enum XLoggerLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

#if defined(__cplusplus)
extern "C" {
#endif//

XUTIL_API void XLogger_Init(char const* const filename, XLoggerLevel lv, int max_size);
XUTIL_API void XLogger_Logout(XLoggerLevel lv, char const* const fmt, ...);
#ifndef LOG4T
#define LOG4T(format, ...) XLogger_Logout(XLoggerLevel::Trace, format, ##__VA_ARGS__)
#endif//
#ifndef LOG4D
#define LOG4D(format, ...) XLogger_Logout(XLoggerLevel::Debug, format, ##__VA_ARGS__)
#endif//
#ifndef LOG4I
#define LOG4I(format, ...) XLogger_Logout(XLoggerLevel::Info, format, ##__VA_ARGS__)
#endif//LOG4I
#ifndef LOG4W
#define LOG4W(format, ...) XLogger_Logout(XLoggerLevel::Warning, format, ##__VA_ARGS__)
#endif//LOG4W
#ifndef LOG4E
#define LOG4E(format, ...) XLogger_Logout(XLoggerLevel::Error, format, ##__VA_ARGS__)
#endif//LOG4E
#ifndef LOG4F
#define LOG4F(format, ...) XLogger_Logout(XLoggerLevel::Fatal, format, ## __VA_ARGS__)
#endif//LOG4F

#if defined(__cplusplus)
}

// #define theLogger XUtil::XLogger::instance()

namespace XUtil
{

// class XUTIL_API ILogger
// {
// public:
//     static ILogger& instance();

//     virtual void init(const char* filename, XLoggerLevel lv = XLoggerLevel::Debug, int max_size = 100 * 1000 * 1000) = 0;
// 	virtual void logout(XLoggerLevel lv, const char *fmt, ...) = 0;
// };

}

#endif//

#endif//__H_XLOGGER_HPP_


