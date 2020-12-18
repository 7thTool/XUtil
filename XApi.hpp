#ifndef _H_XAPI_HPP_
#define _H_XAPI_HPP_

#pragma once

#include "XUtil.hpp"
#include "XXml.hpp"

namespace XUtil
{
	class XApi
	{
	protected:
		boost::property_tree::ptree cfg_;
		std::string name_;
		boost::filesystem::path work_path_;
		boost::filesystem::path data_path_;
	public:
		XApi() {}
		~XApi() {}

		bool init(const char *xml, int xmlflag)
		{
			cfg_from_xml(xml, xmlflag, cfg_);
			//boost::filesystem::path app_path = boost::dll::program_location();
			name_ = cfg_.get<std::string>("name", "");
			work_path_ = cfg_.get<std::string>("work_path", "");
			data_path_ = cfg_.get<std::string>("data_path", "");
			return true;
		}
		void term()
		{
		}

		inline const std::string& name() { return name_; }
		inline const boost::filesystem::path& work_path() { return work_path_; }
		inline const boost::filesystem::path& data_path() { return data_path_; }
		inline boost::property_tree::ptree& cfg() { return cfg_; }
	};
}

#endif//_H_XApI_H_

