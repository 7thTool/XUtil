#include "XLogger.h"
#include "XLogger.hpp"

#if defined(__cplusplus)
extern "C" {
#endif//

void XLogger_Init(char const* const filename, XLoggerLevel lv, int max_size)
{
    XUtil::XLogger::instance().init(filename, (boost::log::trivial::severity_level)lv, max_size);
}

void XLogger_Logout(XLoggerLevel lv, char const* const fmt, ...)
{
	if(XUtil::XLogger::instance().level() > lv) {
		return;
	}
    char buffer[2048] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
	va_end(args);
    BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), (boost::log::trivial::severity_level)lv) << buffer;
}

#if defined(__cplusplus)
}

//

#endif//
