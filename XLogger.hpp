#ifndef __H_XLOGGER_HPP_
#define __H_XLOGGER_HPP_

#pragma once

#include "XUtil.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace XUtil {
	class XLogger
	{
	public:
		static XLogger& instance() {
			static XLogger slogger;
			return slogger;
		}

		void init(
			const std::string& filename,
			boost::log::trivial::severity_level lv = boost::log::trivial::debug,
			int maxsize = 100 * 1000 * 1000
			) {
			boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

			boost::log::add_file_log(
				filename + "_%Y-%m-%d-%N.log",
				keywords::auto_flush = true,
				keywords::open_mode = std::ios::app,
				keywords::rotation_size = maxsize,
				keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
				keywords::format = "%TimeStamp% %Severity% : %Message%"
				);

			boost::log::core::get()->set_filter(boost::log::trivial::severity >= lv);
			boost::log::add_common_attributes();
		}


		void write(boost::log::trivial::severity_level lv, const char* fmt, ...) {
			char buffer[2048] = { 0 };
			va_list args;
			va_start(args, fmt);
			my_vsprintf(buffer, sizeof(buffer) - 1, fmt, args);
			BOOST_LOG_SEV(logger_, lv) << buffer;
			va_end(args);
		}

		void write_direct(boost::log::trivial::severity_level lv, const char* str) {
			BOOST_LOG_SEV(logger_, lv) << str;
		}

		void write_direct(boost::log::trivial::severity_level lv, const std::string& str) {
			BOOST_LOG_SEV(logger_, lv) << str;
		}

	private:
		XLogger() {}
		~XLogger() {}

		boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger_;
	};
}

#endif//__H_XLOGGER_HPP_


