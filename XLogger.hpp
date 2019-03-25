#pragma once
#ifndef __H_XLOGGER_HPP_
#define __H_XLOGGER_HPP_

#include "XUtil.hpp"
#include "XLogger.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace XUtil {

	// template<typename T0>
	// void printf(T0 val) {
	//     std::cout << val;
	// }
	// template<typename T, typename... Args>
	// void printf(T val, Args... args) {
	//     std::cout << val;
	//     printf(args...);
	// }
	// template<typename T0, typename... T>
	// void printf(T0 t0, T... t) {
	//     std::cout << t0;
	//     if constexpr (sizeof...(t) > 0) printf(t...);
	// }

	class XLogger : private boost::noncopyable
	{
	public:
		static XLogger& instance() {
			static XLogger _inst;
			return _inst;
		}

		static void init(
			const std::string &filename,
			boost::log::trivial::severity_level lv = boost::log::trivial::debug,
			int maxsize = 100 * 1000 * 1000)
		{
			boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

			boost::log::add_file_log(
				filename + "_%Y-%m-%d-%N.log",
				boost::log::keywords::auto_flush = true,
				boost::log::keywords::open_mode = std::ios::app,
				boost::log::keywords::rotation_size = maxsize,
				boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
				boost::log::keywords::format = "%TimeStamp% %Severity% : %Message%");

			boost::log::core::get()->set_filter(boost::log::trivial::severity >= lv);
			boost::log::add_common_attributes();
		}

		boost::log::sources::severity_logger<boost::log::trivial::severity_level>& logger()
		{
			return logger_;
		}

		// void logout(boost::log::trivial::severity_level lv, const char *fmt, ...)
		// {
		// 	char buffer[2048] = {0};
		// 	va_list args;
		// 	va_start(args, fmt);
		// 	vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
		// 	BOOST_LOG_SEV(logger_, lv) << buffer;
		// 	va_end(args);
		// }

		// void logout_direct(boost::log::trivial::severity_level lv, const char *str)
		// {
		// 	BOOST_LOG_SEV(logger_, lv) << str;
		// }

	private:
		boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger_;
	};

	// template <typename... Args>
	// inline void LOG4D(const char *fmt, Args... args)
	// {
	// 	XUtil::XLogger::instance().logout(boost::log::trivial::debug, fmt, args...);
	// }

	// template <typename... Args>
	// inline void LOG4I(const char *fmt, Args... args)
	// {
	// 	XUtil::XLogger::instance().logout(boost::log::trivial::info, fmt, args...);
	// }

	// template <typename... Args>
	// inline void LOG4W(const char *fmt, Args... args)
	// {
	// 	XUtil::XLogger::instance().logout(boost::log::trivial::warning, fmt, args...);
	// }

	// template <typename... Args>
	// inline void LOG4E(const char *fmt, Args... args)
	// {
	// 	XUtil::XLogger::instance().logout(boost::log::trivial::error, fmt, args...);
	// }

	// template <typename... Args>
	// inline void LOG4F(const char *fmt, Args... args)
	// {
	// 	XUtil::XLogger::instance().logout(boost::log::trivial::fatal, fmt, args...);
	// }
}

#define logger_trace BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::trace)
#define logger_debug BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::debug)
#define logger_info BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::info)
#define logger_warning BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::warning)
#define logger_error BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::error)
#define logger_fatal BOOST_LOG_SEV(XUtil::XLogger::instance().logger(), boost::log::trivial::fatal)

#endif//__H_XLOGGER_HPP_


